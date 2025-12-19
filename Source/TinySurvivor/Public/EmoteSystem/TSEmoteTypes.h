#pragma once

#include "CoreMinimal.h"
#include "TSEmoteTypes.generated.h"

UENUM(BlueprintType)
enum class ETSEmoteType : uint8
{
	NONE UMETA(DisplayName = "None"),
	HELLO UMETA(DisplayName = "Hello"),
	DANCE1 UMETA(DisplayName = "Dance1"),
	DANCE2 UMETA(DisplayName = "Dance2"),
	DANCE3 UMETA(DisplayName = "Dance3")
};