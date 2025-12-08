// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayTags/GameplayTagDisplayData.h"
#include "GameplayDisplaySubSystem.generated.h"

/**
 * 
 */
UCLASS(Config = Game, BlueprintType)
class TINYSURVIVOR_API UGameplayTagDisplaySubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	// Subsystem 초기화
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

protected:
	// 데이터 테이블 참조
	UPROPERTY()
	UDataTable* GameplayTagDisplayTable;

	// 캐시된 데이터
	TMap<FGameplayTag, FGameplayTagDisplayData> CachedDisplayData;

public:
	// 데이터 테이블 로드
	UFUNCTION(BlueprintCallable, Category = "GameplayTag Display")
	void LoadDataTable(UDataTable* DataTable);

	// 태그의 한글 표시명 가져오기
	UFUNCTION(BlueprintCallable, Category = "GameplayTag Display")
	FText GetDisplayName_KR(const FGameplayTag& Tag) const;

	// 태그의 영어 표시명 가져오기
	UFUNCTION(BlueprintCallable, Category = "GameplayTag Display")
	FText GetDisplayName_EN(const FGameplayTag& Tag) const;

	// 태그의 단위 가져오기
	UFUNCTION(BlueprintCallable, Category = "GameplayTag Display")
	FText GetUnit(const FGameplayTag& Tag) const;

	// 전체 데이터 가져오기
	UFUNCTION(BlueprintCallable, Category = "GameplayTag Display")
	bool GetDisplayData(const FGameplayTag& Tag, FGameplayTagDisplayData& OutData) const;

	// 여러 태그를 한글명으로 변환 (쉼표 구분)
	UFUNCTION(BlueprintCallable, Category = "GameplayTag Display")
	FText GetDisplayNamesFromContainer_KR(const FGameplayTagContainer& Tags, const FString& Separator = TEXT(", ")) const;

	// 정적 접근 헬퍼
	static UGameplayTagDisplaySubsystem* Get(const UObject* WorldContextObject);
};
