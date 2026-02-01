// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "System/Erosion/ErosionLightSourceSubActor.h"
#include "TSStreetLamp.generated.h"

class UTSLampInventory;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFuelModeChanged, bool, bIsFueling);

UCLASS()
class TINYSURVIVOR_API ATSStreetLamp : public AErosionLightSourceSubActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATSStreetLamp();
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(BlueprintAssignable, Category = "Lamp|Events")
	FOnFuelModeChanged OnFuelModeChanged;

#pragma region IInteraction
	virtual bool CanInteract(ATSCharacter* InstigatorCharacter) override;
	virtual void Interact(ATSCharacter* InstigatorCharacter) override;
	virtual bool RunOnServer() override;
#pragma endregion

	virtual void InitializeFromBuildingData(const FBuildingData& BuildingInfo, const int32 StaticDataID) override;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void ChangeLightScaleByErosion(float CurrentErosionScale) override;
	
	UFUNCTION()
	void OnRep_IsFueling();
	UFUNCTION()
	void SetFuelTimer();

	void UseFuel();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lamp|Inventory")
	TObjectPtr<UTSLampInventory> LampInventory;

	// 연료 아이템 StaticDataID
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lamp|Fuel")
	int32 MaintenanceCostID = 0;
	// 초당 연료 소모량
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Lamp|Fuel")
	float MaintenanceInterval = 0.f;
	// 보관 가능한 연료 최대 스택
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lamp|Fuel")
	int32 MaintenanceCostQty = 0;

	FTimerHandle FuelTimerHandle;
	
	UPROPERTY(ReplicatedUsing = OnRep_IsFueling, VisibleAnywhere, BlueprintReadOnly, Category = "Lamp|Fuel")
	bool bIsFueling = false;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Lamp|Fuel")
	float StartTime = 0.f;
};
