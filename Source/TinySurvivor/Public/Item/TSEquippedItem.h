// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TSEquippedItem.generated.h"

UCLASS()
class TINYSURVIVOR_API ATSEquippedItem : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATSEquippedItem();
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	UStaticMeshComponent* MeshComp;
	
	void SetMesh(UStaticMesh* Mesh);
};
