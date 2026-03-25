// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "TSDebugNetLibrary.generated.h"

/**
 * 
 */
UCLASS()
class TINYSURVIVOR_API UTSDebugNetLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	
	// 특정 액터의 로컬/리모트 롤 확인
	UFUNCTION(BlueprintCallable, Category = " TS | Debug | Network")
	static void GetActorNetworkRoles(const AActor* TargetActor);

	// 현재 실행 중인 환경의 NetMode 및 권한 확인
	UFUNCTION(BlueprintCallable, Category = "TS | Debug | Network")
	static void CheckCurrentNetContext(const UObject* WorldContextObject);
};
