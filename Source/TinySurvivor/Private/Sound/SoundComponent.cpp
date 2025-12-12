// Fill out your copyright notice in the Description page of Project Settings.


#include "Sound/SoundComponent.h"

#include "Kismet/GameplayStatics.h"


// Sets default values for this component's properties
USoundComponent::USoundComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

EPhysicalSurface USoundComponent::GetSurfaceTypeBelow(const FVector& Location, float TraceLength) const
{
	FHitResult HitResult;
	FVector Start = Location;
	FVector End = Location - FVector(0.f, 0.f, TraceLength);

	if (LineTrace(Start, End, HitResult))
	{
		if (HitResult.PhysMaterial.IsValid())
		{
			return HitResult.PhysMaterial->SurfaceType;
		}
	}
	return EPhysicalSurface::SurfaceType_Default;
}

EPhysicalSurface USoundComponent::GetSurfaceTypeFront(const FVector& Location, const FVector& ForwardVector,
                                                      float TraceLength) const
{
	FHitResult HitResult;
	FVector Start = Location;
	FVector End = Location + ForwardVector * TraceLength;

	if (LineTrace(Start, End, HitResult))
	{
		if (HitResult.PhysMaterial.IsValid())
		{
			return HitResult.PhysMaterial->SurfaceType;
		}
	}
	return EPhysicalSurface::SurfaceType_Default;
}

EPhysicalSurface USoundComponent::GetSurfaceTypeFromHit(const FHitResult& Hit) const
{
	FHitResult HitResult;
	FVector Start = Hit.ImpactPoint + Hit.ImpactPoint * 10.f;
	FVector End = Hit.ImpactPoint - Hit.ImpactPoint * 200.f;

	if (LineTrace(Start, End, HitResult))
	{
		if (HitResult.PhysMaterial.IsValid())
		{
			return HitResult.PhysMaterial->SurfaceType;
		}
	}
	return EPhysicalSurface::SurfaceType_Default;
}

bool USoundComponent::LineTrace(const FVector& Start, const FVector& End, FHitResult& OutHit) const
{
	FCollisionQueryParams Params;
	Params.bReturnPhysicalMaterial = true;

	return GetWorld()->LineTraceSingleByChannel(
		OutHit,
		Start,
		End,
		ECC_Visibility,
		Params);
}

void USoundComponent::PlaySoundAtLocation(USoundBase* Sound, const FVector& Location) const
{
	if (Sound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), Sound, Location,
		                                      FRotator::ZeroRotator,
		                                      1.f, 1.f, 0.f,
		                                      DefaultAttenuation);
	}
}
