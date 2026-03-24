// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "TSResourceSpawnSystemDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class TINYSURVIVOR_API UTSResourceSpawnSystemDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TS | ResourceSpawn")
	TArray<TObjectPtr<UDataTable>> ResourceSpawnTableArray; 
	
};
