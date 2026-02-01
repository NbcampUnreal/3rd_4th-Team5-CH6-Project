// SpawnerSystemSettings.h
#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "SpawnerSystemSettings.generated.h"

class USpawnerTableAsset;

/*
	스폰 시스템 프로젝트 설정
	Edit > Project Settings > Game > Spawner System에서 설정 가능
*/
UCLASS(Config=Game, DefaultConfig, meta=(DisplayName="Spawner System"))
class TINYSURVIVOR_API USpawnerSystemSettings : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:
	USpawnerSystemSettings()
	{
		CategoryName = TEXT("Game");
		SectionName = TEXT("Spawner System");
	}

	/*
		스폰 데이터 테이블 에셋
		Project Settings에서 설정
	*/
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category="Spawner Data Assets",
		meta=(DisplayName="Spawner Table Asset", ToolTip="스폰 데이터 테이블 에셋"))
	TSoftObjectPtr<USpawnerTableAsset> SpawnerTableAsset;

	// 싱글톤 접근자
	static const USpawnerSystemSettings* Get()
	{
		return GetDefault<USpawnerSystemSettings>();
	}
};