// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SoundComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TINYSURVIVOR_API USoundComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	USoundComponent();
protected:
	// 아래 방향 표면 타입 검출
	UFUNCTION(BlueprintCallable, Category = "Sound")
	EPhysicalSurface GetSurfaceTypeBelow(const FVector& Location, float TraceLength = 50.f) const;

	// 앞 방향 표면 타입 검출
	UFUNCTION(BlueprintCallable, Category = "Sound")
	EPhysicalSurface GetSurfaceTypeFront(const FVector& Location, const FVector& ForwardVector, float TraceLength = 100.f) const;
	
	// HitResult에서 표면 타입 검출
	UFUNCTION(BlueprintCallable, Category = "Sound")
	EPhysicalSurface GetSurfaceTypeFromHit(const FHitResult& Hit) const;
	
	bool LineTrace(const FVector& Start, const FVector& End, FHitResult& OutHit) const;
};
