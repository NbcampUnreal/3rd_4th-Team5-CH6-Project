// Fill out your copyright notice in the Description page of Project Settings.


#include "Sound/Hit/HitComponent.h"

#include "Kismet/GameplayStatics.h"


// Sets default values for this component's properties
UHitComponent::UHitComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UHitComponent::PlayHitSound(const FHitResult& HitResult)
{
	EPhysicalSurface SurfaceType = GetSurfaceTypeFromHit(HitResult);
	
	USoundBase* Sound = HitSounds.FindRef(SurfaceType);
	if (!Sound)
	{
		Sound = DefaultHitSound;
	}
    
	PlaySoundAtLocation(Sound, HitResult.ImpactPoint);
}
