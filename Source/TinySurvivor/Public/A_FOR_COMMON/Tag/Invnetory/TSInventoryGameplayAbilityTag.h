// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"

// 인벤토리 관련 GA tag 모음 

namespace GAInventoryTag
{
	// [try add 실행 시 어떤 GA 인지 알려주는 태그이자 오너에게 부여하는 태그]
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TS_GA_Inventory_TryAddITemToHotKey);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TS_GA_Inventory_TryAddITemToBackPack);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TS_GA_Inventory_TryAddITemToEquip);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TS_GA_Inventory_TryAddITemToFuelProduct);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TS_GA_Invnetory_TryAddItemToStorage)
	
}