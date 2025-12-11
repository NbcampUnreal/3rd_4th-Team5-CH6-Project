#include "GameState/TSGameState.h"
#include "GameFramework/PlayerState.h"
#include "Character/TSCharacter.h"
#include "Controller/TSPlayerController.h"
#include "Engine/Engine.h"

void ATSGameState::CheckGameOver()
{
	if (!HasAuthority())
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
				// 캐릭터가 기절 상태도 아니고 죽은 상태도 아니면 -> 살아있는 상태
				bIsAnyoneAlive = true;
				break; // 한 명이라도 살아있으면 게임 오버 아님.
			}
		}
	}
	if (!bIsAnyoneAlive)
	{
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