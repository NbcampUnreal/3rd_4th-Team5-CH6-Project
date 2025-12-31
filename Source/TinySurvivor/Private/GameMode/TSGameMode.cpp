#include "GameMode/TSGameMode.h"

#include "Character/TSCharacter.h"
#include "Controller/TSPlayerController.h"
#include "Item/System/TSItemPoolActor.h"
#include "Item/System/WorldItemInstanceSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerState/TSPlayerState.h"
#include "GameState/TSGameState.h"

ATSGameMode::ATSGameMode()
{
	PlayerStateClass = ATSPlayerState::StaticClass();
}

void ATSGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	
	if (auto* Subsys = GetWorld()->GetSubsystem<UWorldItemInstanceSubsystem>())
	{
		TArray<FSlotStructMaster> AllData;
		TArray<FTransform> AllTransfroms;
		
		Subsys->GetAllInstsnceData(AllData, AllTransfroms);
		
		const int32 TotalNum = AllData.Num();
		const int32 ChunkSize = 20;
		
		for (int32 i = 0; i < TotalNum; i += ChunkSize)
		{
			int32 Count = FMath::Min(ChunkSize, TotalNum - i);
			
			TArray<FSlotStructMaster> ChunkData;
			TArray<FTransform> ChunkTrans;
			
			for (int32 j = 0; j < Count; ++j)
			{
				ChunkData.Add(AllData[i + j]);
				ChunkTrans.Add(AllTransfroms[i + j]);
			}
			
			if (auto* MyPC = Cast<ATSPlayerController>(NewPlayer))
			{
				MyPC->Client_ReceiveItemChunk(ChunkData, ChunkTrans);
			}
		}
	}
	if (ATSGameState* GS = GetGameState<ATSGameState>())
	{
		GS->Multicast_UpdateCurrentPlayer();
	}
}

void ATSGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	if (ATSGameState* GS = GetGameState<ATSGameState>())
	{
		GS->Multicast_UpdateCurrentPlayer();
	}
}
