// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "TSInGameMasterWidget.generated.h"

class UCommonActivatableWidgetStack;
class UOverlay;
/**
 * 
 */
UCLASS()
class TINYSURVIVOR_API UTSInGameMasterWidget : public UCommonUserWidget
{
	GENERATED_BODY()

public:

	// 오버레이
	UPROPERTY(meta=(BindWidget), BlueprintReadWrite, EditAnywhere, Category = "Widget")
	TObjectPtr<UOverlay> Overlay;

	// 디폴트 위젯 컨테이너
	UPROPERTY(meta=(BindWidget), BlueprintReadWrite, EditAnywhere, Category = "Widget")
	TObjectPtr<UCommonActivatableWidgetStack> DefaultWidgetStack;

	// 인벤토리 위젯 컨테이너
	UPROPERTY(meta=(BindWidget), BlueprintReadWrite, EditAnywhere, Category = "Widget")
	TObjectPtr<UCommonActivatableWidgetStack> InventoryWidgetStack;
};
