// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "A_FOR_INGAME/SECTION_ITEM/Item/Data/Struct/TSItemRuntimeData.h"
#include "TSItemInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UTSItemInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class TINYSURVIVOR_API ITSItemInterface
{
	GENERATED_BODY()

public:
	
	// 아이템 데이터 포인터 가져오기
	virtual FTSItemRuntimeData& GetItemRuntimeDataPtr() = 0;
	
	// 아이템 데이터 주입 
	virtual void SetItemRuntimeData(FTSItemRuntimeData& InItemRuntimeData) = 0;
};
