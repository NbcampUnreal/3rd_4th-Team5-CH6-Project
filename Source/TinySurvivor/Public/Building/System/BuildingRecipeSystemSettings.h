// BuildingRecipeSystemSettings.h
#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "BuildingRecipeSystemSettings.generated.h"

class UBuildingRecipeTableAsset;

/*
	건축물 레시피 시스템 프로젝트 설정
	Edit > Project Settings > Game > Building Recipe System에서 설정 가능
*/
UCLASS(Config=Game, DefaultConfig, meta=(DisplayName="Building Recipe System"))
class TINYSURVIVOR_API UBuildingRecipeSystemSettings : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:
	UBuildingRecipeSystemSettings()
	{
		CategoryName = TEXT("Game");
		SectionName = TEXT("Building Recipe System");
	}
	
	/*
		건축물 레시피 데이터 테이블 에셋
		Project Settings에서 설정
	*/
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category="Building Data Assets",
		meta=(DisplayName="Building Recipe Table Asset", ToolTip="건축물 레시피 데이터 테이블 에셋"))
	TSoftObjectPtr<UBuildingRecipeTableAsset> BuildingRecipeTableAsset;
	
	// 싱글톤 접근자
	static const UBuildingRecipeSystemSettings* Get()
	{
		return GetDefault<UBuildingRecipeSystemSettings>();
	}
};
