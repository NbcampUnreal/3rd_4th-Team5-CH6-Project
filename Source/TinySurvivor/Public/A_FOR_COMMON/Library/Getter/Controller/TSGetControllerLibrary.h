// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "TSGetControllerLibrary.generated.h"

/**
 * 
 */
UCLASS()
class TINYSURVIVOR_API UTSGetControllerLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	
	static APlayerController* GetPlayerControllerFromComponent(AActor* InOwner); 
	
	
};
