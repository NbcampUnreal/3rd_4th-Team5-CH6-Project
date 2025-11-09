// TSItemPoolActor.h

#pragma once

#include "CoreMinimal.h"
#include "ActorPool.h"
#include "TSItemPoolActor.generated.h"


UCLASS()
class TINYSURVIVOR_API ATSItemPoolActor : public AActorPool
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pool Setting", meta = (ClampMin = "1"))
	int32 InitialPoolsize = 50;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pool Setting")
	EPoolStrategy PoolStrategy = EPoolStrategy::CreateNew;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pool Setting", meta = (ClampMin = "1"))
	int32 BatchSize = 5;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pool Setting", meta = (ClampMin = "0.01"))
	float BatchInterval = 0.1f;
};
