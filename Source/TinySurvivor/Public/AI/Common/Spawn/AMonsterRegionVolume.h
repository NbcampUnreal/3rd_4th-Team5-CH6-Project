// MonsterRegionVolume.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "AMonsterRegionVolume.generated.h"

UCLASS()
class TINYSURVIVOR_API AAMonsterRegionVolume : public AActor
{
	GENERATED_BODY()
	
public:	
	AAMonsterRegionVolume();
	
	// 구역의 이름 (DataAsset의 Key와 일치해야 함)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Region")
	FName Regionkey = FName("Default");
	
	// 구역 범위
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UBoxComponent> RegionVolume;
};
