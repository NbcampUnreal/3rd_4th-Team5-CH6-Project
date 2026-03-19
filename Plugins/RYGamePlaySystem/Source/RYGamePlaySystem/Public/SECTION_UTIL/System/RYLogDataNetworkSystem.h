// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "Http.h"
#include "SECTION_UTIL/Data/RYLogMessageData.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "RYLogDataNetworkSystem.generated.h"

/**
 * 일단 보류
 */
UCLASS()
class RYGAMEPLAYSYSTEM_API URYLogDataNetworkSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
//======================================================================================================================
#pragma region 로그_통신
	
	//--------------------
	// 로그 통신
	//--------------------

public:
	
	void SendLogMessage(FGameLogContainer& LogContainer);
	void OnSendReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	
#pragma endregion
//======================================================================================================================
	
};
