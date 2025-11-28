#include "Controller/TSPlayerController.h"

#include "Blueprint/UserWidget.h"
#include "Character/TSCharacter.h"
#include "Components/WidgetSwitcher.h"
#include "Crafting/TSCraftingTable.h"
#include "Inventory/TSInventoryMasterComponent.h"
#include "Item/System/WorldItemInstanceSubsystem.h"
#include "UI/Interface/IWidgetActivation.h"

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
	// 타이머 정리
	if (GetWorld() && ContainerDistanceCheckTimer.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(ContainerDistanceCheckTimer);
	}

	if (HUDWidget)
	{
		HUDWidget->RemoveFromParent();
	}
	Super::EndPlay(EndPlayReason);
}

void ATSPlayerController::ServerTransferItem_Implementation(AActor* SourceActor,
                                                            AActor* TargetActor,
                                                            EInventoryType FromInventoryType, int32 FromSlotIndex,
                                                            EInventoryType ToInventoryType, int32 ToSlotIndex,
                                                            bool bIsFullStack)
{
	if (!SourceActor || !TargetActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("ServerTransferItemBetweenActors: Invalid actors"));
		return;
	}

	// 소스 인벤토리
	UTSInventoryMasterComponent* SourceInventory =
		SourceActor->FindComponentByClass<UTSInventoryMasterComponent>();

	// 타겟 인벤토리
	UTSInventoryMasterComponent* TargetInventory =
		TargetActor->FindComponentByClass<UTSInventoryMasterComponent>();

	if (!SourceInventory || !TargetInventory)
	{
		UE_LOG(LogTemp, Warning, TEXT("ServerTransferItemBetweenActors: Missing inventory components"));
		return;
	}

	// Internal 함수 호출
	SourceInventory->Internal_TransferItem(
		SourceInventory,
		TargetInventory,
		FromInventoryType,
		FromSlotIndex,
		ToInventoryType,
		ToSlotIndex,
		bIsFullStack
	);
}

bool ATSPlayerController::ServerTransferItem_Validate(AActor* SourceActor,
                                                      AActor* TargetActor,
                                                      EInventoryType FromInventoryType, int32 FromSlotIndex,
                                                      EInventoryType ToInventoryType, int32 ToSlotIndex,
                                                      bool bIsFullStack)
{
	// 기본 검증
	if (!SourceActor || !TargetActor)
	{
		return false;
	}

	if (FromSlotIndex < 0 || ToSlotIndex < 0)
	{
		return false;
	}

	// 컨트롤하는 폰
	APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn)
	{
		return false;
	}

	return true;
}

void ATSPlayerController::ServerRequestCraft_Implementation(ATSCraftingTable* CraftingTable, int32 RecipeID)
{
	if (!CraftingTable)
	{
		return;
	}
	ATSCharacter* TSCharacter = Cast<ATSCharacter>(GetPawn());
	if (!TSCharacter)
	{
		return;
	}
	CraftingTable->ServerRequestCraft(RecipeID, TSCharacter);
}

bool ATSPlayerController::ServerRequestCraft_Validate(ATSCraftingTable* CraftingTable, int32 RecipeID)
{
	return CraftingTable && RecipeID > 0;
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

void ATSPlayerController::ToggleInventory()
{
	if (!HUDWidget)
	{
		return;
	}
	UWidgetSwitcher* Switcher = Cast<UWidgetSwitcher>(HUDWidget->GetWidgetFromName(TEXT("WidgetSwitcher_Backpack")));
	if (Switcher)
	{
		int32 ActiveIndex = Switcher->GetActiveWidgetIndex();
		Switcher->SetActiveWidgetIndex(ActiveIndex == 0 ? 1 : 0);
		UWidget* ActiveWidget = Switcher->GetActiveWidget();
		UTSInventoryMasterComponent* ContainerInventory = Cast<UTSInventoryMasterComponent>(
			GetPawn()->GetComponentByClass(UTSInventoryMasterComponent::StaticClass()));

		if (!ContainerInventory)
		{
			return;
		}
		if (ActiveWidget && ActiveWidget->Implements<UIWidgetActivation>())
		{
			IIWidgetActivation::Execute_SetContainerData(ActiveWidget, GetPawn(), ContainerInventory);
		}

		UpdateInputMode();
	}
}

void ATSPlayerController::ToggleContainer(EContentWidgetIndex NewIndex, AActor* ContainerActor)
{
	if (!HUDWidget || !ContainerActor)
	{
		return;
	}
	if (CurrentContainer == ContainerActor)
	{
		CloseCurrentContainer();
		return;
	}
	if (CurrentContainer)
	{
		CloseCurrentContainer();
	}

	CurrentContainer = ContainerActor;

	UTSInventoryMasterComponent* ContainerInventory = Cast<UTSInventoryMasterComponent>(
		CurrentContainer->GetComponentByClass(UTSInventoryMasterComponent::StaticClass()));

	if (!ContainerInventory)
	{
		return;
	}

	UWidgetSwitcher* Switcher = Cast<UWidgetSwitcher>(HUDWidget->GetWidgetFromName(TEXT("WidgetSwitcher_Content")));
	if (!Switcher)
	{
		return;
	}
	SetContentWidgetIndex(Switcher, NewIndex);
	UWidget* ActiveWidget = Switcher->GetActiveWidget();
	if (ActiveWidget)
	{
		if (ActiveWidget->Implements<UIWidgetActivation>())
		{
			IIWidgetActivation::Execute_SetContainerData(ActiveWidget, ContainerActor, ContainerInventory);
		}
	}

	GetWorld()->GetTimerManager().SetTimer(
		ContainerDistanceCheckTimer,
		this,
		&ThisClass::CheckContainerDistance,
		DistanceCheckInterval,
		true
	);
	UpdateInputMode();
}

void ATSPlayerController::ToggleContentsWidget(EContentWidgetIndex NewIndex)
{
	if (!HUDWidget)
	{
		return;
	}

	UWidgetSwitcher* Switcher = Cast<UWidgetSwitcher>(HUDWidget->GetWidgetFromName(TEXT("WidgetSwitcher_Content")));
	if (!Switcher)
	{
		return;
	}
	int32 CurrentIndex = Switcher->GetActiveWidgetIndex();

	// 토글: 콘텐츠 위젯 ↔ 빈 위젯
	if (CurrentIndex == static_cast<int32>(NewIndex))
	{
		SetContentWidgetIndex(Switcher, EContentWidgetIndex::Empty_Content);
	}
	else
	{
		SetContentWidgetIndex(Switcher, NewIndex);
	}
	// 현재 열려있는 위젯이 설정 위젯인지 확인
	bIsSettingsOpen = Switcher->GetActiveWidgetIndex() == static_cast<int32>(EContentWidgetIndex::Settings);

	UpdateInputMode();
}

void ATSPlayerController::HandleEscapeKey()
{
	// 1순위: 환경설정이 열려있으면 → 환경설정만 닫기
	if (bIsSettingsOpen)
	{
		ToggleContentsWidget(EContentWidgetIndex::Settings);
		return;
	}

	// 2순위: 게임 UI가 열려있으면 → 모두 닫기
	if (IsAnyUIOpen())
	{
		CloseAllGameUI();
		return;
	}

	// 3순위: 아무것도 안 열려있으면 → 환경설정 열기
	ToggleContentsWidget(EContentWidgetIndex::Settings);
}

void ATSPlayerController::CheckContainerDistance()
{
	if (!CurrentContainer)
	{
		// 타이머 정리
		GetWorld()->GetTimerManager().ClearTimer(ContainerDistanceCheckTimer);
		return;
	}

	APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn)
	{
		return;
	}

	// 거리 체크
	float Distance = FVector::Dist(
		ControlledPawn->GetActorLocation(),
		CurrentContainer->GetActorLocation()
	);

	// 최대 거리 초과 시 자동으로 닫기
	if (Distance > MaxContainerInteractionDistance)
	{
		UE_LOG(LogTemp, Log, TEXT("Container auto-closed: too far (%.1f > %.1f)"),
		       Distance, MaxContainerInteractionDistance);

		CloseCurrentContainer();
	}
}

void ATSPlayerController::CloseCurrentContainer()
{
	if (!CurrentContainer)
	{
		return;
	}
	GetWorld()->GetTimerManager().ClearTimer(ContainerDistanceCheckTimer);
	CurrentContainer = nullptr;

	if (!HUDWidget)
	{
		return;
	}

	UWidgetSwitcher* Switcher = Cast<UWidgetSwitcher>(HUDWidget->GetWidgetFromName(TEXT("WidgetSwitcher_Content")));
	if (Switcher)
	{
		SetContentWidgetIndex(Switcher, EContentWidgetIndex::Empty_Content);
	}
	UpdateInputMode();
}

void ATSPlayerController::SetContentWidgetIndex(UWidgetSwitcher* Switcher, EContentWidgetIndex NewIndex)
{
	if (!Switcher)
	{
		return;
	}
	int32 Index = static_cast<int32>(NewIndex);
	Switcher->SetActiveWidgetIndex(Index);
}

void ATSPlayerController::CloseAllGameUI()
{
	if (!HUDWidget)
	{
		return;
	}

	// Backpack 스위처 닫기
	UWidgetSwitcher* BackpackSwitcher = Cast<UWidgetSwitcher>(
		HUDWidget->GetWidgetFromName(TEXT("WidgetSwitcher_Backpack"))
	);

	if (BackpackSwitcher)
	{
		BackpackSwitcher->SetActiveWidgetIndex(Empty_Backpack);
	}

	// Content 스위처 닫기 (컨테이너 포함)
	if (CurrentContainer)
	{
		CloseCurrentContainer();
	}
	else
	{
		UWidgetSwitcher* ContentSwitcher = Cast<UWidgetSwitcher>(
			HUDWidget->GetWidgetFromName(TEXT("WidgetSwitcher_Content"))
		);

		if (ContentSwitcher)
		{
			SetContentWidgetIndex(ContentSwitcher, EContentWidgetIndex::Empty_Content);
		}
	}

	UpdateInputMode();
}

void ATSPlayerController::UpdateInputMode()
{
	// 환경설정이 열려있으면 UI Only
	if (bIsSettingsOpen)
	{
		SetInputMode(FInputModeGameAndUI());
		SetShowMouseCursor(true);
		return;
	}

	// 게임 UI가 열려있으면 UI And Game
	if (IsAnyUIOpen())
	{
		SetInputMode(FInputModeGameAndUI());
		SetShowMouseCursor(true);
		return;
	}

	// 아무것도 없으면 Game Only
	SetInputMode(FInputModeGameOnly());
	SetShowMouseCursor(false);
}

bool ATSPlayerController::IsAnyUIOpen() const
{
	if (!HUDWidget)
	{
		return false;
	}

	// Backpack 스위처 확인
	UWidgetSwitcher* BackpackSwitcher = Cast<UWidgetSwitcher>(
		HUDWidget->GetWidgetFromName(TEXT("WidgetSwitcher_Backpack"))
	);

	if (BackpackSwitcher && BackpackSwitcher->GetActiveWidgetIndex() != Empty_Backpack)
	{
		return true;
	}

	// Content 스위처 확인
	UWidgetSwitcher* ContentSwitcher = Cast<UWidgetSwitcher>(
		HUDWidget->GetWidgetFromName(TEXT("WidgetSwitcher_Content"))
	);

	if (ContentSwitcher && ContentSwitcher->GetActiveWidgetIndex() != static_cast<int32>(
		EContentWidgetIndex::Empty_Content))
	{
		return true;
	}

	return false;
}

void ATSPlayerController::Client_ReceiveItemChunk_Implementation(const TArray<FSlotStructMaster>& ChunkData,
                                                                 const TArray<FTransform>& ChunkTransforms)
{
	if (UWorld* World = GetWorld())
	{
		if (auto* InstanceSys = World->GetSubsystem<UWorldItemInstanceSubsystem>())
		{
			InstanceSys->HandleInitialChunkData(ChunkData, ChunkTransforms);
		}
	}
}
