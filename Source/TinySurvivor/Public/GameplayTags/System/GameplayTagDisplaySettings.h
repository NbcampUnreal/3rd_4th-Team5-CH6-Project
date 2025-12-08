// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "Engine/DataTable.h"
#include "GameplayTagDisplaySettings.generated.h"

/**
 * GameplayTagDisplay 서브시스템 설정
 * Project Settings > Game > Gameplay Tag Display에서 설정 가능
 */
UCLASS(Config=Game, DefaultConfig, meta = (DisplayName="Gameplay Tag Display"))
class TINYSURVIVOR_API UGameplayTagDisplaySettings : public UDeveloperSettings
{
	GENERATED_BODY()
public:
	
	UGameplayTagDisplaySettings()
	{
		CategoryName = TEXT("Game");
		SectionName = TEXT("Gameplay Tag Display");
	}

	/*
		GameplayTagDisplay 데이터 테이블
		Project Settings에서 설정
	*/
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category="GameplayTagDisplay DataTable",
		meta=(DisplayName="GameplayTagDisplay DataTable", ToolTip="태그별 디스플레이 데이터 테이블"))
	TSoftObjectPtr<UDataTable> GameplayTagDisplayDataTable;

	// 싱글톤 접근자
	static const UGameplayTagDisplaySettings* Get()
	{
		return GetDefault<UGameplayTagDisplaySettings>();
	}
};
