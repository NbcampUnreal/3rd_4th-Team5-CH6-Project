// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "FTSInteractMessageData.generated.h"

USTRUCT(BlueprintType)
struct FTSInteractMessageData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TS | Interact")
	TWeakObjectPtr<AActor> CurrentInteractTarget = nullptr;
	
};

