// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "GameFramework/Actor.h"
#include "Mon_ProjectileBase.generated.h"

class UGameplayAbility;
class UProjectileMovementComponent;
class USphereComponent;
class UArrowComponent;
class UStaticMeshComponent;
class UNiagaraComponent;

UCLASS()
class TINYSURVIVOR_API AMon_ProjectileBase : public AActor
{
	GENERATED_BODY()

public:
	AMon_ProjectileBase();
	virtual void BeginPlay() override;
	
	FORCEINLINE void SetSpeed(float& InSpeed) { Speed = InSpeed;}
	FORCEINLINE void SetTargetLocation(FVector& InTargetLocation) { TargetLocation = InTargetLocation; }
	void TryActivateProjectile();

protected:
	UFUNCTION()
	void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
	UPROPERTY(EditAnywhere, Category = "Projectile")
	TObjectPtr<USphereComponent> ProjectileCollision;
	
	UPROPERTY(EditAnywhere, Category = "Projectile")
	TObjectPtr<UArrowComponent> ProjectileArrow;
	
	UPROPERTY(EditAnywhere, Category = "Projectile")
	TObjectPtr<UStaticMeshComponent> ProjectileMesh;
	
	UPROPERTY(EditAnywhere, Category = "Projectile")
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

	UPROPERTY(EditAnywhere, Category = "Projectile")
	TObjectPtr<UNiagaraComponent> ProjectileVFX;
	
	UPROPERTY(EditAnywhere, Category = "Projectile")
	float Speed = 1000.f;
	
	UPROPERTY(EditAnywhere, Category = "Projectile")
	FVector TargetLocation = FVector::ZeroVector;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile")
	FGameplayTag AttackTag = FGameplayTag::EmptyTag;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
	TSubclassOf<UGameplayAbility> HitAbilityClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
	float AttackDamageValue = 1.0f;
};
