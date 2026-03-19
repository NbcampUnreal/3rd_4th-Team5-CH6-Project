// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "Enum_RYGameCycle.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "RYGameCycleLibrary.generated.h"

/**
 * 
 */
UCLASS()
class RYGAMESCORESYSTEM_API URYGameCycleLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	// 게임 사이클 매니저의 델리게이트를 구독하는 API
	UFUNCTION(BlueprintCallable, Category = "RY | Core | GameCycle")
	static void SubscribeGameCycleDelegate(UObject* Object, const FName FunctionName);
	
	// 게임 사이클 매니저에게 특정 상황임을 알리는 API
	UFUNCTION(BlueprintCallable, Category = "RY | Core | GameCycle")
	static void SetGameCycleMode(const UObject* Object, const ERYGameCycleMode NewGameCycleMode);
	
	// 게임 사이클 매니저로부터 현재 어떤 상황임을 확인받는 API
	UFUNCTION(BlueprintCallable, Category = "RY | Core | GameCycle")
	static ERYGameCycleMode GetGameCycleMode(const UObject* Object);
	
};
