// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "TSPlayerReadyData.generated.h"

USTRUCT(BlueprintType)
struct TINYSURVIVOR_API FTSPlayerReadyData
{
	GENERATED_BODY()
	
	// 초기화되어야 하는 플레이어 
	UPROPERTY()
	APlayerController* PlayerController = nullptr;
	
	// 초기화가 되었는가?
	bool IsReady = false;
	
};
