// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "TSMonsterSpawnArea.generated.h"

class UBoxComponent;

UCLASS()
class TINYSURVIVOR_API ATSMonsterSpawnArea : public AActor
{
	GENERATED_BODY()

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
	
	//---------------------------------------
	// ATSMonsterSpawnArea 라이프 사이클 
	//--------------------------------------
	
public:
	ATSMonsterSpawnArea();
	virtual void BeginPlay() override;
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
	
	//---------------------------------------
	// 스폰 요청 API
	//--------------------------------------
	
public:	
	
	UFUNCTION(BlueprintCallable)
	FGameplayTag GetSpawnMonsterTag() const { return SpawnMonsterTag; }
	
protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(Category = "SpawnSystem", DisplayName = "소환할 몬스터", Categories = "Monster"))
	FGameplayTag SpawnMonsterTag;
	
	UFUNCTION()
	void DoSpawnWhenDetectedPlayer(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
	
	//---------------------------------------
	// 컴포넌트
	//--------------------------------------
	
protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SpawnSystem")
	TObjectPtr<USceneComponent> RootComp;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SpawnSystem")
	TObjectPtr<UBoxComponent> SpawnArea;
	
};
