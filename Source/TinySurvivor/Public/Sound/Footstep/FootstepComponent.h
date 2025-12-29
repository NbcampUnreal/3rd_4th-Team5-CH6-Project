// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Sound/SoundComponent.h"
#include "FootstepComponent.generated.h"

class USoundBase;

USTRUCT()
struct FFootstepSound
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, Category = "Footstep")
	TObjectPtr<USoundBase> FootStep = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Footstep")
	TObjectPtr<USoundBase> Climbing = nullptr;
};
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TINYSURVIVOR_API UFootstepComponent : public USoundComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UFootstepComponent();
	
	// 표면별 사운드
	UPROPERTY(EditDefaultsOnly, Category = "Footstep")
	TMap<TEnumAsByte<EPhysicalSurface>, FFootstepSound> FootstepSounds;
	
	// Default 사운드
	UPROPERTY(EditDefaultsOnly, Category = "Footstep")
	FFootstepSound DefaultFootstepSound;
	
	// 걷기, 달리기 소리
	UFUNCTION(BlueprintCallable, Category = "Footstep")
	void PlayFootstepSound(const FVector& Location);
	
	// 착지할 때 소리
	UFUNCTION(BlueprintCallable, Category = "Footstep")
	void PlayFootstepSoundFromHit(const FHitResult& Hit);
	
	// 벽 타는 소리
	UFUNCTION(BlueprintCallable, Category = "Footstep")
	void PlayClimbingSound(const FVector& Location);
};
