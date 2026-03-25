// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"

namespace InputTag
{
	namespace LeftClick
	{
		// 좌클 입력 call
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(TS_Input_LeftClickCall);
		
		// 좌클 기본 어빌리티 발동 입력 태그 -> Action 
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(TS_Input_LeftClick_Default);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(TS_Input_LeftClick_DefaultAction);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(TS_Input_LeftClick_DefaultAction_OnPlayer);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(TS_Input_LeftClick_DefaultAction_OnGiant);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(TS_Input_LeftClick_DefaultAction_OnMonster);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(TS_Input_LeftClick_DefaultAction_OnResource);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(TS_Input_LeftClick_DefaultAction_OnItem);
		
		// 핫키 어빌리티 발동 입력 태그 -> Action by TargetRole
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(TS_Input_LeftClick_HotKey);
	}	
	
	
	namespace RightClick
	{
		// 우클 입력
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(TS_Input_RightClickCall);
	}	
}
