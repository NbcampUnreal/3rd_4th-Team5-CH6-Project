#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "TSGameState.generated.h"


UCLASS()
class TINYSURVIVOR_API ATSGameState : public AGameState
{
	GENERATED_BODY()
	
public: 
	void CheckGameOver();
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_GameOver();
};
