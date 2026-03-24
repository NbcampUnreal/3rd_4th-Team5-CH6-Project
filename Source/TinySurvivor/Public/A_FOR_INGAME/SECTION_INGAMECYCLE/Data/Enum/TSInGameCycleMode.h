// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"


// init 이후 호출 
UENUM(BlueprintType)
enum class ETSInGameCycleMode : uint8
{
	NEW		UMETA(DisplayName = "New"),
	LOAD	UMETA(DisplayName = "Load"),
	PLAY	UMETA(DisplayName = "Play"),
};