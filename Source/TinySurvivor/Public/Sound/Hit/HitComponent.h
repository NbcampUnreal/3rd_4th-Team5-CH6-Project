// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Sound/SoundComponent.h"
#include "HitComponent.generated.h"


enum class EItemAnimType : uint8;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TINYSURVIVOR_API UHitComponent : public USoundComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UHitComponent();
	
	UPROPERTY(EditDefaultsOnly, Category = "HitSound")
	TMap<TEnumAsByte<EPhysicalSurface>, USoundBase*> HitSounds;
    
	UPROPERTY(EditDefaultsOnly, Category = "HitSound")
	USoundBase* DefaultHitSound;

	UFUNCTION(BlueprintCallable, Category = "HitSound")
	void PlayHitSound(const FHitResult& HitResult);
};
