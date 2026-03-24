// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"


// init 이후 호출 
UENUM(BlueprintType)
enum class ETSInGameCycleMode : uint8
{
	LOAD	UMETA(DisplayName = "Load"),
	NEW		UMETA(DisplayName = "New"),
	PLAY	UMETA(DisplayName = "Play"),
};