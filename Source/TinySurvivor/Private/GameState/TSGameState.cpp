#include "GameState/TSGameState.h"
#include "GameFramework/PlayerState.h"
#include "Character/TSCharacter.h"
#include "Controller/TSPlayerController.h"
#include "Engine/Engine.h"

void ATSGameState::CheckGameOver()
{
	if (!HasAuthority() || bGameOverTriggered)
	{
		return;
	}
	bool bIsAnyoneAlive = false;
	
	for (APlayerState* PS : PlayerArray)
	{
		if (!PS)
		{
			continue;
		}
		ATSCharacter* Character = Cast<ATSCharacter>(PS->GetPawn());
		if (Character)
		{
			if (!Character->IsDowned() && !Character->IsDead())
			{
				bIsAnyoneAlive = true;
				break;
			}
		}
	}
	if (!bIsAnyoneAlive)
	{
		bGameOverTriggered = true;
		for (APlayerState* PS : PlayerArray)
		{
			if (PS)
			{
				ATSCharacter* Character = Cast<ATSCharacter>(PS->GetPawn());
				if (Character)
				{
					if (!Character->IsDead())
					{
						Character->Die();
					}
				}
			}
		}
		Multicast_GameOver();
	}
}

void ATSGameState::Multicast_GameOver_Implementation()
{
	for (FConstPlayerControllerIterator PCI = GetWorld()->GetPlayerControllerIterator(); PCI; ++PCI)
	{
		ATSPlayerController* PC = Cast<ATSPlayerController>(PCI->Get());
		
		if (PC->IsLocalController())
		{
			PC->ShowGameOverUI();
			FInputModeUIOnly InputMode;
			PC->SetInputMode(InputMode);
			
			PC->bShowMouseCursor = true;
		}
	}
}

void ATSGameState::DecreaseSanityToAll(bool bIsDeath)
{
	if (!HasAuthority())
	{
		return;
	}
	TSubclassOf<UGameplayEffect> DecreaseSanityEffect; 
	if (bIsDeath)
	{
		DecreaseSanityEffect = OtherPlayerDeadSanityEffectClass;
	} else
	{
		DecreaseSanityEffect = OtherPlayerDownedSanityEffectClass;
	}
	if (!DecreaseSanityEffect)
	{
		return;
	}
	for (APlayerState* PS : PlayerArray)
	{
		ATSCharacter* Character = Cast<ATSCharacter>(PS->GetPawn());
		if (Character && Character->GetAbilitySystemComponent())
		{
			UAbilitySystemComponent* ASC = Character->GetAbilitySystemComponent();
			FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
			ContextHandle.AddSourceObject(this);
			FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(DecreaseSanityEffect, 1, ContextHandle);
			if (SpecHandle.IsValid())
			{
				ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			}
		}
	}
}

void ATSGameState::Multicast_UpdateCurrentPlayer_Implementation()
{
	UpdateCurrentPlayer();
}
