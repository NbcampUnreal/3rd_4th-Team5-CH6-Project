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
	
	UE_DEFINE_GAMEPLAY_TAG(TAG_State_Increase_Stamina, "State.Increase.Stamina"); //스태미나 자동회복
	UE_DEFINE_GAMEPLAY_TAG(TAG_State_Stamina_Block, "State.Stamina.Block"); // 스태미나 자동회복 차단
	UE_DEFINE_GAMEPLAY_TAG(TAG_State_Status_Thirst, "State.Status.Thirst"); //갈증 상태이상
	UE_DEFINE_GAMEPLAY_TAG(TAG_State_Status_Hunger, "State.Status.Hunger"); //배고픔 상태이상
	UE_DEFINE_GAMEPLAY_TAG(TAG_State_Status_TempHot, "State.Status.TempHot"); // 더위 상태이상
	UE_DEFINE_GAMEPLAY_TAG(TAG_State_Status_TempCold, "State.Status.TempCold"); // 추위 상태이상
	UE_DEFINE_GAMEPLAY_TAG(TAG_State_Status_Full, "State.Status.Full")
	UE_DEFINE_GAMEPLAY_TAG(TAG_State_Status_Anxiety, "State.Status.Anxiety")
	UE_DEFINE_GAMEPLAY_TAG(TAG_State_Status_Panic, "State.Status.Panic")
	UE_DEFINE_GAMEPLAY_TAG(TAG_State_Status_InLightSourceRange, "State.Status.InLightSourceRange")
	UE_DEFINE_GAMEPLAY_TAG(TAG_State_Status_InDarkSourceRange, "State.Status.InDarkSourceRange")
	UE_DEFINE_GAMEPLAY_TAG(TAG_State_Sanity_InLightBlock, "State.Sanity.InLightBlock")
	
	UE_DEFINE_GAMEPLAY_TAG(TAG_State_Status_Attack, "State.Status.Attack");
	
	UE_DEFINE_GAMEPLAY_TAG(TAG_State_Status_Downed, "State.Status.Downed");
	UE_DEFINE_GAMEPLAY_TAG(TAG_State_Status_Dead, "State.Status.Dead");
	UE_DEFINE_GAMEPLAY_TAG(TAG_State_Status_Rescuing, "State.Status.Rescuing");
	UE_DEFINE_GAMEPLAY_TAG(TAG_State_Status_PickUpItem, "State.Status.PickUpItem");
	
	// 아이템 사용 관련 태그
	UE_DEFINE_GAMEPLAY_TAG(TAG_State_Item, "State.Item");
	UE_DEFINE_GAMEPLAY_TAG(TAG_State_Item_Consuming, "State.Item.Consuming"); // 아이템 사용 중
	
	//================================
	// State / Status 관련 상태이상
	//================================
	
	// Poison 관련 태그
	UE_DEFINE_GAMEPLAY_TAG(TAG_State_Status_Poison, "State.Status.Poison");					// 독 상태이상
	UE_DEFINE_GAMEPLAY_TAG(TAG_State_Status_Immune_Poison, "State.Status.Immune.Poison");	// 독 면역
	
	// Temperature 관련 태그
	UE_DEFINE_GAMEPLAY_TAG(TAG_State_Status_Temperature_Increase, "State.Status.Temperature.Increase");  // 온도 상승 상태이상
	UE_DEFINE_GAMEPLAY_TAG(TAG_State_Status_Temperature_Decrease, "State.Status.Temperature.Decrease");  // 온도 감소 상태이상

	// Temperature 면역 관련 태그
	UE_DEFINE_GAMEPLAY_TAG(TAG_State_Status_Immune_Temperature_Increase, "State.Status.Immune.Temperature.Increase"); // 온도 상승 면역
	UE_DEFINE_GAMEPLAY_TAG(TAG_State_Status_Immune_Temperature_Decrease, "State.Status.Immune.Temperature.Decrease"); // 온도 감소 면역
	
	//================================
	// State Modifier (방어구 효과)
	//================================
	// 스탯/효과 증가 계열
	UE_DEFINE_GAMEPLAY_TAG(TAG_State_Modifier, "State.Modifier"); // 스탯/효과 변경 계열 상위 태그
	UE_DEFINE_GAMEPLAY_TAG(TAG_State_Modifier_DAMAGE_REFLECT, "State.Modifier.DAMAGE_REFLECT");		// 피해 반사율 증가
	UE_DEFINE_GAMEPLAY_TAG(TAG_State_Modifier_DAMAGE_REDUCTION, "State.Modifier.DAMAGE_REDUCTION");	// 피해 감소율 증가
	UE_DEFINE_GAMEPLAY_TAG(TAG_State_Modifier_MOVE_SPEED, "State.Modifier.MOVE_SPEED");				// 이동 속도 증가
	
	//================================
	// GameplayCue 관련 태그 정의
	//================================
	UE_DEFINE_GAMEPLAY_TAG(TAG_GameplayCue_Status_Poisoned, "GameplayCue.Status.Poisoned");
	
	//================================
	// Input 관련 태그 정의
	//================================
	UE_DEFINE_GAMEPLAY_TAG(TAG_Input_HotKey, "Input.HotKey");
	
	//================================
	// Data 관련 태그 정의
	//================================
	UE_DEFINE_GAMEPLAY_TAG(TAG_Data_Damage_Fall, "Data.Damage.Fall");

  //================================
	// Event 관련 태그 (몽타주 노티파이 등)
	//================================
	UE_DEFINE_GAMEPLAY_TAG(TAG_Event_Montage_HitCheck, "Event.Montage.HitCheck");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Event_Armor_Hit, "Event.Armor.Hit");
	
	UE_DEFINE_GAMEPLAY_TAG(TAG_Enemy_Damage, "Enemy.Damage");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Hit, "Ability.Hit");
}

namespace MonsterTags
{
	// 식별용
	UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Type_Player, "Character.Type.Player");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Type_Monster, "Character.Type.Monster");
	
	// 상태용
	UE_DEFINE_GAMEPLAY_TAG(TAG_State_Dead, "State.Dead");
	UE_DEFINE_GAMEPLAY_TAG(TAG_State_Hit, "State.Hit");
	
	// AI 공격용
	UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Attack, "Ability.Attack");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Sprint, "Ability.Sprint");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Cooldown_Attack, "Cooldown.Attack");
	
	// 이벤트용
	UE_DEFINE_GAMEPLAY_TAG(TAG_Event_Montage_Hit, "Event.Montage.Hit");
}