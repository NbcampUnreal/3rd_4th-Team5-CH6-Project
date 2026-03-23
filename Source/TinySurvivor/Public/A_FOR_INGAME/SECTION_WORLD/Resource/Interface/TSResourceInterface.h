// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "A_FOR_INGAME/SECTION_WORLD/Resource/Data/Struct/TSResourceRuntimeData.h"
#include "UObject/Interface.h"
#include "TSResourceInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UTSResourceInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class TINYSURVIVOR_API ITSResourceInterface
{
	GENERATED_BODY()

public:
	
	// 자원 데이터 가져오기 
	virtual FTSResourceRuntimeData& GetResourceRuntimeData() = 0;
	
	// 자원 데이터 주입 
	virtual void SetResourceRuntimeData(FTSResourceRuntimeData& InResourceRuntimeData) = 0;
};
