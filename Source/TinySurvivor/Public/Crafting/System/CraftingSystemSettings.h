// CraftingSystemSettings.h
#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "CraftingSystemSettings.generated.h"

class UCraftingTableAsset;

/*
	크래프팅 시스템 프로젝트 설정
	Edit > Project Settings > Game > Crafting System에서 설정 가능
*/
UCLASS(Config=Game, DefaultConfig, meta=(DisplayName="Crafting System"))
class TINYSURVIVOR_API UCraftingSystemSettings : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:
	UCraftingSystemSettings()
	{
		CategoryName = TEXT("Game");
		SectionName = TEXT("Crafting System");
	}

	/*
		크래프팅 레시피 데이터 테이블 에셋
		Project Settings에서 설정
	*/
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category="Crafting Data Assets",
		meta=(DisplayName="Crafting Table Asset", ToolTip="크래프팅 레시피 데이터 테이블 에셋"))
	TSoftObjectPtr<UCraftingTableAsset> CraftingTableAsset;

	// 싱글톤 접근자
	static const UCraftingSystemSettings* Get()
	{
		return GetDefault<UCraftingSystemSettings>();
	}
};
