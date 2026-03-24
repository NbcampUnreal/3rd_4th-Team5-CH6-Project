// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "TSResourceSpawnSetting.generated.h"

class UTSResourceSpawnSystemDataAsset;
/**
 * 
 */
UCLASS(Config = Game, DefaultConfig, meta=(DisplayName="TS 자원 스폰 로직 데이터 섹션"))
class TINYSURVIVOR_API UTSResourceSpawnSetting : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere, Category = "ItemData")
	TSoftObjectPtr<UTSResourceSpawnSystemDataAsset> GlobalResourceSpawnDataAsset = nullptr;
	
	UPROPERTY(Config, EditAnywhere, Category = "ItemData")
	bool bWantPrintDeBugLog = false;
};
