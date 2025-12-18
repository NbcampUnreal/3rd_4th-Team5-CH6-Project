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

	// 몬스터 식별 태그
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(DisplayName="몬스터 식별 태그", Categories = "Monster"))
	FGameplayTag MonsterTag;
	
	// 몬스터 CDO
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(DisplayName="몬스터 CDO"))
	TSubclassOf<ACharacter> MonsterClass;
	
//----------------------------------------------------------------------------------------------------------------------
	
	// 몬스터가 죽었을 때 드랍되는 메인 재료 아이템 ID
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="System",
		meta=(DisplayName="Main Drop ID", ToolTip="몬스터가 죽었을 때  드랍되는 메인 재료 아이템 ID"))
	int32 MainDropTableID = 0;

	// 몬스터가 죽었을 때 드랍되는 메인 재료 아이템 확률
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="System",
	meta=(DisplayName="Main Drop Rate", ToolTip="몬스터가 죽었을 때  드랍되는 메인 재료 아이템 확률")) 
	float MainDropTablePrecent = 1.0f;
	
	// 몬스터가 죽었을 때 드랍되는 메인 재료 아이템 드랍 최소 개수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="System",
		meta=(DisplayName="Main Drop Min Num", ToolTip="몬스터가 죽었을 때  드랍되는 메인 재료 아이템 드랍 최소 개수"))
	int32 MainDropMinNum = 1;
	
	// 몬스터가 죽었을 때 드랍되는 메인 재료 아이템 드랍 최대 개수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="System",
	meta=(DisplayName="Main Drop Max Num", ToolTip="몬스터가 죽었을 때  드랍되는 메인 재료 아이템 드랍 최대 개수"))
	int32 MainDropMaxNum = 1;
	
//----------------------------------------------------------------------------------------------------------------------
	
	// 몬스터가 죽었을 때 드랍되는 서브1 아이템 ID
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="System",
		meta=(DisplayName="Sub Drop1 ID", ToolTip="몬스터가 죽었을 때  드랍되는 서브 재료1 아이템 ID"))
	int32 SubDrop1TableID = 0;

	// 몬스터가 죽었을 때 드랍되는 서브1 아이템 확률
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="System",
	meta=(DisplayName="Sub Drop1 Table Rate", ToolTip="몬스터가 죽었을 때  드랍되는 서브 재료1 아이템 확률")) 
	float SubDrop1TablePrecent = 0.5f;
	
	// 몬스터가 죽었을 때 드랍되는 서브1 재료 아이템 드랍 최소 개수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="System",
		meta=(DisplayName="Sub Drop1 Min Num", ToolTip="몬스터가 죽었을 때  드랍되는 서브 재료1 아이템 드랍 최소 개수"))
	int32 SubDrop1MinNum = 0;
	
	// 몬스터가 죽었을 때 드랍되는 서브1 아이템 드랍 최대 개수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="System",
	meta=(DisplayName="Sub Drop1 Max Num", ToolTip="몬스터가 죽었을 때  드랍되는 서브 재료1 아이템 드랍 최대 개수"))
	int32 SubDrop1MaxNum = 1;
	
//----------------------------------------------------------------------------------------------------------------------
	
	// 몬스터가 죽었을 때 드랍되는 서브2 아이템 ID
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="System",
		meta=(DisplayName="Sub Drop2 ID", ToolTip="몬스터가 죽었을 때  드랍되는 서브 재료2 아이템 ID"))
	int32 SubDrop2TableID = 0;

	// 몬스터가 죽었을 때 드랍되는 서브2 아이템 확률
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="System",
	meta=(DisplayName="Sub Drop2 Table Rate", ToolTip="몬스터가 죽었을 때  드랍되는 서브 재료2 아이템 확률")) 
	float SubDrop2TablePrecent = 0.5f;
	
	// 몬스터가 죽었을 때 드랍되는 서브2 재료 아이템 드랍 최소 개수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="System",
		meta=(DisplayName="Sub Drop2 Min Num", ToolTip="몬스터가 죽었을 때  드랍되는 서브 재료2 아이템 드랍 최소 개수"))
	int32 SubDrop2MinNum = 0;
	
	// 몬스터가 죽었을 때 드랍되는 서브2 아이템 드랍 최대 개수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="System",
	meta=(DisplayName="Sub Drop2 Max Num", ToolTip="몬스터가 죽었을 때  드랍되는 서브 재료2 아이템 드랍 최대 개수"))
	int32 SubDrop2MaxNum = 1;
};