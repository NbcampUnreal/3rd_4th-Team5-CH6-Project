// ItemSystemSettings.h
#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "ItemSystemSettings.generated.h"

class UItemTableAsset;

/*
	아이템 시스템 프로젝트 설정
	Edit > Project Settings > Game > Item System에서 설정 가능
*/
UCLASS(Config=Game, DefaultConfig, meta=(DisplayName="Item System"))
class TINYSURVIVOR_API UItemSystemSettings : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:
	UItemSystemSettings()
	{
		CategoryName = TEXT("Game");
		SectionName = TEXT("Item System");
	}

	/*
		아이템 데이터 테이블 에셋
		Project Settings에서 설정
	*/
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category="Item Data Assets",
		meta=(DisplayName="Item Table Asset", ToolTip="아이템 데이터 테이블 에셋"))
	TSoftObjectPtr<UItemTableAsset> ItemTableAsset;

	// 싱글톤 접근자
	static const UItemSystemSettings* Get()
	{
		return GetDefault<UItemSystemSettings>();
	}
};
