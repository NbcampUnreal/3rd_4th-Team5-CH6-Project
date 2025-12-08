// TSAISpawner.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "TSAISpawner.generated.h"

UCLASS()
class TINYSURVIVOR_API ATSAISpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	ATSAISpawner();

protected:
	virtual void BeginPlay() override;

public:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings")
	FName RegionKey = FName("Default");
	
	// 스폰 범위 시각화 및 계산용
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UBoxComponent* SpawnVolume;
	
	// 박스 내 랜덤 위치 반환
	FVector GetRandomPointInVolume() const;
};
