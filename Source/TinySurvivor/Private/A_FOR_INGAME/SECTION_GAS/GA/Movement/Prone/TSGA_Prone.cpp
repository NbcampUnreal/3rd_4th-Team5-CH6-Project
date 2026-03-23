// All CopyRight From YulRyongGameStudio //


#include "A_FOR_INGAME/SECTION_GAS/GA/Movement/Prone/TSGA_Prone.h"
#include "A_FOR_COMMON/Tag/Movement/TSMovementTag.h"

UTSGA_Prone::UTSGA_Prone()
{
	// 에셋 태그 
	FGameplayTagContainer AssetTags;
	AssetTags.AddTag(MovementTag::TS_Movement_Moving_Prone);
	SetAssetTags(AssetTags);
	
	// 부여 태그
	ActivationOwnedTags.AddTag(MovementTag::TS_Movement_Moving_Prone);
	
	// 차단 가능한 태그 (걷기, 앉기, 엎드리기, 조준)
	CancelAbilitiesWithTag.AddTag(MovementTag::TS_Movement_Moving_Sprinting);
	CancelAbilitiesWithTag.AddTag(MovementTag::TS_Movement_Moving_Walking);
	CancelAbilitiesWithTag.AddTag(MovementTag::TS_Movement_Moving_Crouch);
	CancelAbilitiesWithTag.AddTag(MovementTag::TS_Movement_Moving_Aiming);
	
	// 버그 방지 태그
	WantChangeMoveSpeedWhenDeactivateTag.AddTag(MovementTag::TS_Movement_Moving_Sprinting);
	WantChangeMoveSpeedWhenDeactivateTag.AddTag(MovementTag::TS_Movement_Moving_Walking);
	WantChangeMoveSpeedWhenDeactivateTag.AddTag(MovementTag::TS_Movement_Moving_Crouch);
	WantChangeMoveSpeedWhenDeactivateTag.AddTag(MovementTag::TS_Movement_Moving_Aiming);
}
