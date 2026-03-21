// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "TSDecayLibrary.generated.h"

/**
 * 
 */
UCLASS()
class TINYSURVIVOR_API UTSDecayLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	
	UFUNCTION(BlueprintCallable, Category = "TSLibrary | ItemHelperLibrary")
	static double CalculateExpirationTime(const UObject* InWorldContextObject, double InCurrentExpirationTime, int32 InCurrentStack, int32 InNewItemStack, float InDecayRate);

	UFUNCTION(BlueprintCallable, Category = "TSLibrary | ItemHelperLibrary")
	static float CalculateDecayPercent(const UObject* InWorldContextObject, double InCurrentExpirationTime, float InDecayRate);
	
};
