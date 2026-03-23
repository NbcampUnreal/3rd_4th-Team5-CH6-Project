// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "TSResourceDataSystemDataAsset.generated.h"

class UTSResourceTablesDataAsset;
/**
 * 
 */
UCLASS()
class TINYSURVIVOR_API UTSResourceDataSystemDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TS | Resource")
	TArray<TObjectPtr<UTSResourceTablesDataAsset>> ResourceInfoDataAssetArray;
	
};
