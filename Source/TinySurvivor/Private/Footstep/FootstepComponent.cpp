// Fill out your copyright notice in the Description page of Project Settings.


#include "Footstep/FootstepComponent.h"

#include "Kismet/GameplayStatics.h"


// Sets default values for this component's properties
UFootstepComponent::UFootstepComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


void UFootstepComponent::PlayFootstepSound(const FVector& Location, bool& IsLeft)
{
	FHitResult HitResult;
	FVector Start = Location;
	FVector End = Location - FVector(0.f, 0.f, 50.f);

	FCollisionQueryParams Params;
	Params.bReturnPhysicalMaterial = true;

	GetWorld()->LineTraceSingleByChannel(
		HitResult,
		Start,
		End,
		ECC_Visibility,
		Params);
	EPhysicalSurface SurfaceType = EPhysicalSurface::SurfaceType_Default;
	if (HitResult.PhysMaterial.IsValid())
	{
		SurfaceType = HitResult.PhysMaterial->SurfaceType;
	}
	if (USoundBase* Sound = IsLeft
							? FootstepSounds.FindRef(SurfaceType).Left
							: FootstepSounds.FindRef(SurfaceType).Right)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), Sound, Location);
	}
}

void UFootstepComponent::PlayFootstepSoundFromHit(const FHitResult& Hit)
{
	if (!Hit.IsValidBlockingHit())
	{
		return;
	}
	FHitResult HitResult;
	FVector Start = Hit.ImpactPoint;
	FVector End = Hit.ImpactPoint - FVector(0.f, 0.f, 50.f);

	FCollisionQueryParams Params;
	Params.bReturnPhysicalMaterial = true;

	GetWorld()->LineTraceSingleByChannel(
		HitResult,
		Start,
		End,
		ECC_Visibility,
		Params);
	
	EPhysicalSurface SurfaceType = EPhysicalSurface::SurfaceType_Default;
	if (HitResult.PhysMaterial.IsValid())
	{
		SurfaceType = HitResult.PhysMaterial->SurfaceType;
	}
	if (USoundBase* Sound = FootstepSounds.FindRef(SurfaceType).Left)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), Sound, HitResult.ImpactPoint);
	}
	if (USoundBase* Sound = FootstepSounds.FindRef(SurfaceType).Right)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), Sound, HitResult.ImpactPoint);
	}
}

// Called when the game starts
void UFootstepComponent::BeginPlay()
{
	Super::BeginPlay();

	// GC 방지용
	for (const auto& Pair : FootstepSounds)
	{
		if (Pair.Value.Left)
		{
			Pair.Value.Left->AddToRoot();
		}
		if (Pair.Value.Right)
		{
			Pair.Value.Right->AddToRoot();
		}
	}
}
