// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "GameplayTagDisplayData.generated.h"


UENUM(BlueprintType)
enum class EDisplayUnit : uint8
{
	NONE UMETA(DisplayName = "None", ToolTip="없음"),
	PERCENT UMETA(DisplayName = "%", ToolTip="퍼센트"),
	SEC UMETA(DisplayName = "'s", ToolTip="시간: 초")
};

/**
 * GameplayTag 표시 정보 데이터 테이블 구조체
 */
USTRUCT(BlueprintType)
struct FGameplayTagDisplayData : public FTableRowBase
{
	GENERATED_BODY()

public:
	FGameplayTagDisplayData()
		: Tag()
		  , DisplayName_KR(FText::GetEmpty())
		  , DisplayName_EN(FText::GetEmpty())
		  , Unit(EDisplayUnit::NONE)
		  , bShowSign(false)
	{
	}

	// 태그
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Display")
	FGameplayTag Tag;

	// 한글 표시명
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Display")
	FText DisplayName_KR;

	// 영어 표시명
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Display")
	FText DisplayName_EN;

	// 단위 (%, 초, m/s 등)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Display")
	EDisplayUnit Unit = EDisplayUnit::NONE;

	// 부호 표시 여부
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Display")
	bool bShowSign = false;
};
