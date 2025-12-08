// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "StatViewer.generated.h"

class UListView;
class IDisplayDataProvider;
class UGameplayTagDisplaySubsystem;

/**
 * 스탯 리스트 생성 부모 클래스
 * 아이템/빌딩의 각 카테고리별로 구현
 */
UCLASS()
class TINYSURVIVOR_API UStatViewer : public UObject
{
	GENERATED_BODY()
public:
	// 위젯의 리스트 뷰에 스탯 추가
	// 자식 클래스에서 구현
	virtual void ShowStatList(
		UListView* ListView, 
		const IDisplayDataProvider& DataProvider,
		UGameplayTagDisplaySubsystem* DisplaySystem);

protected:
	// 공통 헬퍼 함수
	void AddStatToList(
		UListView* ListView,
		const FText& StatName,
		const FText& StatValue,
		const FText& StatUnit = FText::GetEmpty());
};
