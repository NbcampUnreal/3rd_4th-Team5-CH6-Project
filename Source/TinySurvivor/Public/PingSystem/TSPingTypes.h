#pragma once

#include "CoreMinimal.h"
#include "TSPingTypes.generated.h"


UENUM(BlueprintType)
enum class ETSPingType : uint8
{
	NONE UMETA(DisplayName = "None"),
	DANGER UMETA(DisplayName = "Danger"),
	DISCOVER UMETA(DisplayName = "Discover"),
	HELP UMETA(DisplayName = "Help"),
	LOCATION UMETA(DisplayName = "Location")
};

