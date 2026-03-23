// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"

// 가능한 상호작용 타입 태그 
// GA 와 Loot Comp 에서 씀. 

namespace PossibleInteractTag
{
	// F 키를 눌러서 일반 상호작용 
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TS_Interact_Normal);
	
	// 공격으로 상호작용 
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TS_Interact_Attack);
	
}
