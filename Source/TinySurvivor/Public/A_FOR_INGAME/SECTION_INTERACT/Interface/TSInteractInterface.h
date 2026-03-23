// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "A_FOR_COMMON/Data/Enum/TSPlayRole.h"
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
	// 게임 플레이 롤 가져오기
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "TS | Interact")
	ETSPlayRole GetPlayRole();
	
	// 토글 위젯 켜기 
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "TS | Interact")
	void ToggleInteractWidget(bool InWantOn);
	
};
