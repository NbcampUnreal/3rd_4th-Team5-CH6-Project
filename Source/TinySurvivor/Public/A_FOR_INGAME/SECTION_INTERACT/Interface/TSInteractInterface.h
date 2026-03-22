// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TSInteractInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UTSInteractInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class TINYSURVIVOR_API ITSInteractInterface
{
	GENERATED_BODY()

public:
	
	// 토글 위젯 켜기 
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "TS | Interact")
	void ToggleInteractWidget(bool InWantOn);
	
};
