#pragma once

#include "CoreMinimal.h"
#include "ChaserTargetTaskData.generated.h"

USTRUCT()
struct FChaserTargetTaskData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category = "Input")
	AActor* TargetActor = nullptr;
};
