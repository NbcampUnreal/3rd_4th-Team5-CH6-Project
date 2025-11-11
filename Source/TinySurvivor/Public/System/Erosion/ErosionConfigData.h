// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ErosionConfigData.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class TINYSURVIVOR_API UErosionConfigData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
	
	// 자연 상승값 (처음 기본 값)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Erosion | 자연 상승값 (처음 기본 값)")
	float NaturalErosionRate = 1.0f;

	// 자연 상승 속도 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Erosion | 자연 상승 속도 (처음 기본 값)")
	float NaturalErosionSpeed = 0.f;
	
	// 침식도 하한값
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Erosion | 침식도 하한값", meta=(ClampMin="0"))
	float MinErosion = 0.0f;

	// 침식도 상한값 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Erosion | 침식도 상한값", meta=(ClampMin="100"))
	float MaxErosion = 100.0f;

	// 디버깅
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Erosion | Debug")
	bool bShowDebug = false;
};
