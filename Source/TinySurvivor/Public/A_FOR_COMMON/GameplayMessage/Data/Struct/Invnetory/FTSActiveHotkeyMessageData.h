// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "FTSActiveHotkeyMessageData.generated.h"

USTRUCT(BlueprintType)
struct TINYSURVIVOR_API FtSActiveHotkeyMessageData
{
	GENERATED_BODY()
	
	UPROPERTY(Config, EditAnywhere, Category = "Player | Hotkey")
	int32 ActiveHotkeyIndex = -1;
	
};