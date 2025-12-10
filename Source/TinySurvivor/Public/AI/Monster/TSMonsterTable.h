// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "TSMonsterTable.generated.h"

USTRUCT(BlueprintType)
struct TINYSURVIVOR_API FTSMonsterTable : public FTableRowBase
{
	GENERATED_BODY()

	// 아이템 카테고리
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(DisplayName="아이템 카테고리", Categories = "Monster"))
	FGameplayTag MonsterTag;
	
	// 아이템 CDO
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(DisplayName="아이템 CDO"))
	TSubclassOf<ACharacter> MonsterClass;
};