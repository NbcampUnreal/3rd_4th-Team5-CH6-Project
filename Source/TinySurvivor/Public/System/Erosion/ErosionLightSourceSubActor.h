// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ErosionLightSourceSubActor.generated.h"

class UErosionLightSourceComponent;

UCLASS()
class TINYSURVIVOR_API AErosionLightSourceSubActor : public AActor
{
	GENERATED_BODY()

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// ===============================
	// UErosionLightSourceComponent 라이프 사이클
	// ===============================
	
public:
	AErosionLightSourceSubActor();

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// ===============================
	// UErosionLightSourceComponent 컴포넌트 
	// ===============================

public:
	UFUNCTION(BlueprintCallable, Category = "ErosionLightSource")
	UErosionLightSourceComponent* GetErosionLightSourceComponent() { return ErosionLightSourceComponent;}
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ErosionLightSource")
	TObjectPtr<UErosionLightSourceComponent> ErosionLightSourceComponent;
};
