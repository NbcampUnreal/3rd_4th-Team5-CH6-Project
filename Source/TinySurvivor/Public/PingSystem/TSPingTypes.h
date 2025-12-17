#pragma once

#include "CoreMinimal.h"
#include "TSPingTypes.generated.h"


UENUM(BlueprintType)
enum class ETSPingType : uint8
{
	NONE UMETA(DisplayName = "None"),
	DANGER UMETA(DisplayName = "Danger"),
	FINDRESOURCE UMETA(DisplayName = "FindResource"),
	GATHERING UMETA(DisplayName = "Gathering"),
	MYLOCATION UMETA(DisplayName = "MyLocation")
};

