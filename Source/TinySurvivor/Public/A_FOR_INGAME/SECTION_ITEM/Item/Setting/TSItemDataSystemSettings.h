// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "TSItemDataSystemSettings.generated.h"

class UTSItemDataSystemDataAsset;
/**
 * 
 */
UCLASS(Config= Game, DefaultConfig, meta=(DisplayName="아이템 데이터 섹션"))
class TINYSURVIVOR_API UTSItemDataSystemSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere, Category = "ItemData")
	TSoftObjectPtr<UTSItemDataSystemDataAsset> GlobalItemDataAsset;
};