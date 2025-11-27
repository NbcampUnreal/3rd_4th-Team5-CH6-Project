// AbilityGameplayTags.cpp
#include "GameplayTags/AbilityGameplayTags.h"

namespace AbilityTags
{
	//================================
	// Ability 관련 태그 정의
	//================================
	UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Move_JumpOrClimb, "Ability.Move.JumpOrClimb");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Move_Roll, "Ability.Move.Roll");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Move_Sprint, "Ability.Move.Sprint");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Move_Crouch, "Ability.Move.Crouch");
	
	UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Interact_Build, "Ability.Interact.Build");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Interact_Interact, "Ability.Interact.Interact");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Interact_Ping, "Ability.Interact.Ping");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Interact_LeftClick, "Ability.Interact.LeftClick");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Interact_RightClick, "Ability.Interact.RightClick");
	
	UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_HotKey, "Ability.HotKey");
	
	//================================
	// State / Status 관련 태그 정의
	//================================
	UE_DEFINE_GAMEPLAY_TAG(TAG_State_Move_WASD, "State.Move.WASD");
	UE_DEFINE_GAMEPLAY_TAG(TAG_State_Move_Sprint, "State.Move.Sprint");
	UE_DEFINE_GAMEPLAY_TAG(TAG_State_Move_Crouch, "State.Move.Crouch");
	UE_DEFINE_GAMEPLAY_TAG(TAG_State_Move_Roll, "State.Move.Roll");
	UE_DEFINE_GAMEPLAY_TAG(TAG_State_Move_Jump, "State.Move.Jump");
	UE_DEFINE_GAMEPLAY_TAG(TAG_State_Move_Climb, "State.Move.Climb");
	
	UE_DEFINE_GAMEPLAY_TAG(TAG_State_Increase_Stamina, "State.Increase.Stamina");
	UE_DEFINE_GAMEPLAY_TAG(TAG_State_Stamina_Block, "State.Stamina.Block");
	UE_DEFINE_GAMEPLAY_TAG(TAG_State_Status_Thirst, "State.Status.Thirst");
	
	// Poison 관련 태그
	UE_DEFINE_GAMEPLAY_TAG(TAG_State_Status_Poison, "State.Status.Poison");					// 독 상태이상
	UE_DEFINE_GAMEPLAY_TAG(TAG_State_Status_Immune_Poison, "State.Status.Immune.Poison");	// 독 면역
	
	// 아이템 사용 관련 태그
	UE_DEFINE_GAMEPLAY_TAG(TAG_State_Item, "State.Item");
	UE_DEFINE_GAMEPLAY_TAG(TAG_State_Item_Consuming, "State.Item.Consuming"); // 아이템 사용 중
	
	//================================
	// Input 관련 태그 정의
	//================================
	UE_DEFINE_GAMEPLAY_TAG(TAG_Input_HotKey, "Input.HotKey");
}