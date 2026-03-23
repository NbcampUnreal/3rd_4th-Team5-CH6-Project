// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"

// 움직임 관련 애니메이션, GA 가 공통으로 쓰는 태그 
namespace MovementTag
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TS_Movement_Looking);	// 시야 회전  

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TS_Movement_Moving);				// 움직임
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TS_Movement_Moving_Jogging);		// 기본 속도 
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TS_Movement_Moving_Sprinting);	// 달리기
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TS_Movement_Moving_Walking);		// 걷기
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TS_Movement_Jumping);	// 점프 
}