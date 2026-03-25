// All CopyRight From YulRyongGameStudio //


#include "A_FOR_COMMON/Tag/Input/TSInputTag.h"

namespace InputTag
{
	namespace LeftClick
	{
		// 좌클 입력 call
		UE_DEFINE_GAMEPLAY_TAG(TS_Input_LeftClickCall, "TS.Input.LeftClick.LefClickCall");
		
		// 좌클 기본 어빌리티 발동 입력 태그 -> Action 
		UE_DEFINE_GAMEPLAY_TAG(TS_Input_LeftClick_Default, "TS.Input.LeftClick.InDefault");
		UE_DEFINE_GAMEPLAY_TAG(TS_Input_LeftClick_DefaultAction, "TS.Input.LeftClick.DefaultAction");
		UE_DEFINE_GAMEPLAY_TAG(TS_Input_LeftClick_DefaultAction_OnPlayer, "TS.Input.LeftClick.DefaultAction.OnPlayer");
		UE_DEFINE_GAMEPLAY_TAG(TS_Input_LeftClick_DefaultAction_OnGiant, "TS.Input.LeftClick.DefaultAction.OnGiant");
		UE_DEFINE_GAMEPLAY_TAG(TS_Input_LeftClick_DefaultAction_OnMonster, "TS.Input.LeftClick.DefaultAction.OnMonster");
		UE_DEFINE_GAMEPLAY_TAG(TS_Input_LeftClick_DefaultAction_OnResource, "TS.Input.LeftClick.DefaultAction.OnResource");
		UE_DEFINE_GAMEPLAY_TAG(TS_Input_LeftClick_DefaultAction_OnItem, "TS.Input.LeftClick.DefaultAction.OnItem");
		
		// 핫키 어빌리티 발동 입력 태그
		UE_DEFINE_GAMEPLAY_TAG(TS_Input_LeftClick_HotKey, "TS.Input.LeftClick.InHotKey");
	}
	
	
	namespace RightClick
	{
		// 우클 입력 call
		UE_DEFINE_GAMEPLAY_TAG(TS_Input_RightClickCall, "TS.Input.RightClick.RightClickCall");
	}	
	
}
