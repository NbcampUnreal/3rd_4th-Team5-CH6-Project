// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "TSGiantSwitchingAreaDataAsset.generated.h"

class AGiantSwitchingResourceArea;
/**
 * 
 */
UCLASS()
class TINYSURVIVOR_API UTSGiantSwitchingAreaDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
	
	// 섹터 CDO
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Area")
	TSubclassOf<AGiantSwitchingResourceArea> AreaClass;
	
	// 디버깅
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Erosion | Debug")
	bool bShowDebug = false;
};
