// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "TSASCLibrary.generated.h"

/**
 * 
 */
UCLASS()
class TINYSURVIVOR_API UTSASCLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	
	// 컴포넌트로가 ASC 가져오려면 이 API를 호출
	UFUNCTION(BlueprintCallable, Category = "TSLibrary | ASCLibrary")
	static UAbilitySystemComponent* GetASCFromComp(AActor* InOwner);
	
	
};
