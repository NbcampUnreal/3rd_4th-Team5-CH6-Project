#include "Controller/TSPlayerController.h"

#include "Blueprint/UserWidget.h"
#include "Character/TSCharacter.h"

void ATSPlayerController::AcknowledgePossession(class APawn* P)
{
	Super::AcknowledgePossession(P);
	if (IsLocalController() && P)
	{
		ATSCharacter* TSCharacter = Cast<ATSCharacter>(P);
		if (TSCharacter)
		{
			InitializePlayerHUD();
			if (!IsUpdated)
			{
				UpdateHUDWithCharacter(TSCharacter);
			}
		}
	}
}

void ATSPlayerController::OnUnPossess()
{
	Super::OnUnPossess();
	IsUpdated = false;
}

void ATSPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (HUDWidget)
	{
		HUDWidget->RemoveFromParent();
	}
	Super::EndPlay(EndPlayReason);
}

void ATSPlayerController::InitializePlayerHUD()
{
	if (HUDWidget)
	{
		return;
	}
	if (!HUDWidgetClass)
	{
		UE_LOG(LogTemp, Error, TEXT("  HUDWidgetClass is NULL!"));
		return;
	}

	HUDWidget = CreateWidget<UUserWidget>(this, HUDWidgetClass);

	if (HUDWidget)
	{
		APlayerController* OwningPC = HUDWidget->GetOwningPlayer();

		HUDWidget->AddToViewport();

		// 이미 Pawn이 있다면 즉시 업데이트
		APawn* CurrentPawn = GetPawn();

		if (CurrentPawn)
		{
			ATSCharacter* TSCharacter = Cast<ATSCharacter>(CurrentPawn);
			if (TSCharacter)
			{
				UpdateHUDWithCharacter(TSCharacter);
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("  ⚠️ No Pawn yet, will update in OnPossess"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("  ❌ CreateWidget failed!"));
	}
}

void ATSPlayerController::UpdateHUDWithCharacter(ATSCharacter* TSCharacter)
{
	if (!HUDWidget || !TSCharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("UpdateHUDWithCharacter: Invalid HUDWidget or Character!"));
		return;
	}
	// WBP_InGameWidget에서 "UpdateHUDWithCharacter" 이벤트를 구현
	UFunction* UpdateFunc = HUDWidget->FindFunction(FName("UpdateHUDWithCharacter"));
	if (UpdateFunc)
	{
		struct FParams
		{
			ATSCharacter* Character;
		};
		FParams Params;
		Params.Character = TSCharacter;
		HUDWidget->ProcessEvent(UpdateFunc, &Params);
		IsUpdated = true;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("  ⚠️ UpdateHUDWithCharacter function not found in widget BP"));
	}
}
