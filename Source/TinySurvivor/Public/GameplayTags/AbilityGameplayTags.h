// AbilityGameplayTags.h
#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NativeGameplayTags.h"

/*
	Ability, State 전용 네이티브 게임플레이 태그
	- Ability 관련 태그
	- State / Status 관련 태그
	- Input 관련 태그
	- Poison 관련 태그 (독 디버프, 면역)
*/
namespace AbilityTags
{
	//================================
	// Ability 관련 태그
	//================================
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
	
	//================================
	// State / Status 관련 태그
	//================================
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_State_Move_WASD);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_State_Move_Sprint);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_State_Move_Crouch);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_State_Move_Roll);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_State_Move_Jump);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_State_Move_Climb);
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_State_Increase_Stamina);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_State_Stamina_Block);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_State_Status_Thirst); // 갈증 상태이상
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_State_Status_Hunger); // 배고픔 상태이상
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_State_Status_TempHot); // 더위 상태이상
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_State_Status_TempCold); // 추위 상태이상
	
	// Poison 관련 태그
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_State_Status_Poison);		// 독 상태이상
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_State_Status_Immune_Poison); // 독 면역 (해독제)
	
	// 아이템 사용 관련 태그
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_State_Item);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_State_Item_Consuming); // 아이템 사용 중
	
	//================================
	// Input 관련 태그
	//================================
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Input_HotKey);
	
	//================================
	// Data 관련 태그 정의
	//================================
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Data_Damage_Fall);
}

