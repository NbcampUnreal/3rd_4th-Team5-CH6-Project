// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "TSMonsterDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class TINYSURVIVOR_API UTSMonsterDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(EditDefaultsOnly, Category = "Monster", meta=(DisplayName="몬스터 이터 테이블"))
	TObjectPtr<UDataTable> MonsterTable;
	
	UPROPERTY(EditDefaultsOnly, Category = "Log", meta=(DisplayName="bWantPrintLog (로그 표시 여부)"))
	bool bWantPrintLog = true;
};
