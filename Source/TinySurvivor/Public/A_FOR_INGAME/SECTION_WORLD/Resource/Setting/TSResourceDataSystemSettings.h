// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "TSResourceDataSystemSettings.generated.h"

class UTSResourceDataSystemDataAsset;
/**
 * 
 */
UCLASS(Config = Game, DefaultConfig, meta=(DisplayName="TS 자원 데이터 섹션"))
class TINYSURVIVOR_API UTSResourceDataSystemSettings : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:
	UPROPERTY(Config, EditAnywhere, Category = "ResourceData")
	TSoftObjectPtr<UTSResourceDataSystemDataAsset> GlobalResourceDataAsset;
	
	UPROPERTY(Config, EditAnywhere, Category = "ResourceData")
	bool bWantPrintDeBugLog = false;
};
