// Fill out your copyright notice in the Description page of Project Settings.


#include "Sound/Footstep/FootstepComponent.h"

#include "Kismet/GameplayStatics.h"


// Sets default values for this component's properties
UFootstepComponent::UFootstepComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


void UFootstepComponent::PlayFootstepSound(const FVector& Location)
{
	EPhysicalSurface SurfaceType = GetSurfaceTypeBelow(Location, 50.f);

	const auto* Sounds = FootstepSounds.Find(SurfaceType);
	if (!Sounds)
	{
		Sounds = &DefaultFootstepSound;
	}

	USoundBase* Sound = Sounds->FootStep;

	PlaySoundAtLocation(Sound, Location);
}

void UFootstepComponent::PlayFootstepSoundFromHit(const FHitResult& Hit)
{
	if (!Hit.IsValidBlockingHit())
	{
		return;
	}
	EPhysicalSurface SurfaceType = GetSurfaceTypeFromHit(Hit);

	const auto* Sounds = FootstepSounds.Find(SurfaceType);
	if (!Sounds)
	{
		Sounds = &DefaultFootstepSound;
	}

	PlaySoundAtLocation(Sounds->FootStep, Hit.ImpactPoint);
	PlaySoundAtLocation(Sounds->FootStep, Hit.ImpactPoint);
}

void UFootstepComponent::PlayClimbingSound(const FVector& Location)
{
	EPhysicalSurface SurfaceType = GetSurfaceTypeFront(Location, GetOwner()->GetActorForwardVector(), 100.f);

	const auto* Sounds = FootstepSounds.Find(SurfaceType);
	if (!Sounds)
	{
		Sounds = &DefaultFootstepSound;
	}

	USoundBase* Sound = Sounds->Climbing;
	PlaySoundAtLocation(Sound, Location);
}
