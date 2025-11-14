// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IWidgetActivation.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UIWidgetActivation : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class TINYSURVIVOR_API IIWidgetActivation
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	// ✅ 위젯이 활성화될 때 호출
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Widget")
	void OnWidgetActivated();
};
