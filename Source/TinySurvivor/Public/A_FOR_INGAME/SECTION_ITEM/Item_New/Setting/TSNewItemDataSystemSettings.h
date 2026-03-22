// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "TSNewItemDataSystemSettings.generated.h"

class UTSNewItemDataSystemDataAsset;
/**
 * 
 */
UCLASS(Config= Game, DefaultConfig, meta=(DisplayName="아이템 데이터 섹션 New"))
class TINYSURVIVOR_API UTSNewItemDataSystemSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere, Category = "ItemData")
	TSoftObjectPtr<UTSNewItemDataSystemDataAsset> GlobalItemDataAsset;
};