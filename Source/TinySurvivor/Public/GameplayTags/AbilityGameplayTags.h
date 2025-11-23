// AbilityGameplayTags.h
#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NativeGameplayTags.h"

/*
     Ability, State 전용 네이티브 게임플레이 태그! 
 */
namespace AbilityTags
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Ability_Move_JumpOrClimb);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Ability_Move_Roll);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Ability_Move_Sprint);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Ability_Move_Crouch);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Ability_Interact_Build);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Ability_Interact_Interact);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Ability_Interact_Ping);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Ability_Interact_LeftClick);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Ability_Interact_RightClick);

	
	
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Ability_HotKey);
	
	
	
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_State_Move_Sprint);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_State_Move_Crouch);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_State_Move_Roll);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_State_Move_Jump);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_State_Move_Climb);
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Input_HotKey);
	
	
	
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_State_Increase_Stamina);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_State_Stamina_Block);
	
}

