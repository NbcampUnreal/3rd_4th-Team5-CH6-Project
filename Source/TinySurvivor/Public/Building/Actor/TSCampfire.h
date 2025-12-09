// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "System/Erosion/ErosionLightSourceSubActor.h"
#include "TSCampfire.generated.h"

class UNiagaraComponent;

UCLASS()
class TINYSURVIVOR_API ATSCampfire : public AErosionLightSourceSubActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATSCampfire();
	virtual void InitializeFromBuildingData(const FBuildingData& BuildingInfo, const int32 StaticDataID) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	void OnLifeTimeExpired();
	
	UPROPERTY(EditDefaultsOnly, Category = "ErosionLightSource")
	float LifeTime = 0.f;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "ErosionLightSource")
	TObjectPtr<UNiagaraComponent> FireEffect;
	
	FTimerHandle LifeTimeTimerHandle;
};
