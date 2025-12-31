#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "TSGameState.generated.h"

class UGameplayEffect;

UCLASS()
class TINYSURVIVOR_API ATSGameState : public AGameState
{
	GENERATED_BODY()
	
public: 
	void CheckGameOver();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sanity")
	TSubclassOf<UGameplayEffect> OtherPlayerDownedSanityEffectClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sanity")
	TSubclassOf<UGameplayEffect> OtherPlayerDeadSanityEffectClass;
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_GameOver();
	
	void DecreaseSanityToAll(bool bIsDeath);
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_UpdateCurrentPlayer();
	
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateCurrentPlayer();
};
