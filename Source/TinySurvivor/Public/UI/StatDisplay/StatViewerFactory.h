// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "StatViewerFactory.generated.h"

class UStatViewer;
/**
 * 
 */
UCLASS()
class TINYSURVIVOR_API UStatViewerFactory : public UObject
{
	GENERATED_BODY()
private:


public:
	// 카테고리 태그로 적절한 Builder 반환
	UStatViewer* GetBuilder(const FGameplayTag& CategoryTag);

private:
	// 태그에 해당하는 Builder 생성
	UStatViewer* CreateBuilderForTag(const FGameplayTag& CategoryTag);
	// 카테고리별 UStatViewer 캐시 (한 번 생성하면 재사용)
	UPROPERTY()
	TMap<FGameplayTag, TObjectPtr<UStatViewer>> BuilderCache;
};
