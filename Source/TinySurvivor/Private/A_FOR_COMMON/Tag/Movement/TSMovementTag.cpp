// All CopyRight From YulRyongGameStudio //


#include "A_FOR_COMMON/Tag/Movement/TSMovementTag.h"


namespace MovementTag
{
	UE_DEFINE_GAMEPLAY_TAG(TS_Movement_Looking, "TS.Movement.Looking");	// 시야 회전  
	UE_DEFINE_GAMEPLAY_TAG(TS_Movement_Moving, "TS.Movement.Moving");	// 움직임
	
	UE_DEFINE_GAMEPLAY_TAG(TS_Movement_Moving_Jogging,	"TS.Movement.Moving.Jogging");		// 기본 속도 
	UE_DEFINE_GAMEPLAY_TAG(TS_Movement_Moving_Sprinting, "TS.Movement.Moving.Sprinting");	// 달리기
	UE_DEFINE_GAMEPLAY_TAG(TS_Movement_Moving_Walking,	"TS.Movement.Moving.Walking");		// 걷기
	UE_DEFINE_GAMEPLAY_TAG(TS_Movement_Moving_Crouch,	"TS.Movement.Moving.Crouch");		// 앉기
	UE_DEFINE_GAMEPLAY_TAG(TS_Movement_Moving_Prone,	"TS.Movement.Moving.Prone");		// 엎드리기
	
	UE_DEFINE_GAMEPLAY_TAG(TS_Movement_Jumping, "TS.Movement.Jumping");	// 점프 
	
	UE_DEFINE_GAMEPLAY_TAG(TS_Movement_Moving_Aiming,"TS.Movement.Moving.Aiming");		 // 조준 중
	UE_DEFINE_GAMEPLAY_TAG(TS_Movement_Moving_NotAiming,"TS.Movement.Moving.NotAiming"); // 비조준 중
	
}