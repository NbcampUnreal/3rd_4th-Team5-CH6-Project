#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "AbilitySystemSetting.generated.h"

class UTSAbilityDataAsset;

UCLASS(Config = Game, DefaultConfig, meta=(DisplayName="Ability System"))
class TINYSURVIVOR_API UAbilitySystemSetting : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:
	static const UAbilitySystemSetting* GeAbilitySystemSetting() { return GetDefault<UAbilitySystemSetting>();}
	
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, meta=(DisplayName="어빌리티 데이터 에셋"))
	TSoftObjectPtr<UTSAbilityDataAsset> AbilityDataAsset;
};
