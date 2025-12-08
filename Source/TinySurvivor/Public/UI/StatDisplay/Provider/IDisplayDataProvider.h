// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

struct FGameplayTagContainer;
struct FGameplayTag;
/**
 * FItemData, FBuildingData에 접근하기 위한 인터페이스
 */
class TINYSURVIVOR_API IDisplayDataProvider
{
public:
	virtual ~IDisplayDataProvider() = default;

	virtual	FGameplayTag GetCategoryTag() const = 0;
	virtual float GetStatValue(const FGameplayTag& StatTag) const = 0;
	virtual FGameplayTagContainer GetTagContainer(const FGameplayTag& ContainerTag) const = 0;
	virtual FGameplayTag GetEffectTag() const = 0;
	virtual int32 GetEnumValue(const FGameplayTag& EnumTag) const = 0;
	virtual FText GetItemName(const FGameplayTag& StatTag) const
	{
		return FText::FromString(TEXT("알 수 없음"));
	};
	virtual void SetWorldContext(const UObject* InWorldContext){};
};
