// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "TSMonsterSetting.generated.h"

class UTSMonsterDataAsset;
/**
 * 
 */
UCLASS(config = Game, defaultconfig, meta=( DisplayName = "MonsterDataSetting"))
class TINYSURVIVOR_API UTSMonsterSetting : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:
	UPROPERTY(Config, EditAnywhere, Category = "MonsterDataAsset", meta=(DisplayName="MonsterDataAsset (몬스터 데이터 어셋)"))
	TSoftObjectPtr<UTSMonsterDataAsset> MonsterDataAsset;
	
};
