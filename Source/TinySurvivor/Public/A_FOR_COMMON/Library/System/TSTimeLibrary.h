// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "TSTimeLibrary.generated.h"

/**
 * 
 */
UCLASS()
class TINYSURVIVOR_API UTSTimeLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:	
	
	UFUNCTION(BlueprintCallable, Category = "TSLibrary | TSTimeLibrary")
	static float GetCurrentTime(const UObject* InWorldContextObject);
	
};
