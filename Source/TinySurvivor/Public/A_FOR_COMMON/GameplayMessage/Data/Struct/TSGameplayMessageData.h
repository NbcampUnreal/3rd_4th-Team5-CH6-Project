// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "TSGameplayMessageData.generated.h"

USTRUCT(BlueprintType)
struct FTSGameplayMessageData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GameplayMessage")
	int32 MessageID = 0;
	
};

