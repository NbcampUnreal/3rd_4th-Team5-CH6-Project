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
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_State_Status_Full); // 배부름
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_State_Status_Anxiety); // 30 < Sanity <= 70 
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_State_Status_Panic); // 0 <= Sanity < 70
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_State_Status_InLightSourceRange); // 빛 구역이면 활성화 되는 태그
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_State_Status_InDarkSourceRange); // 어둠 구역이면 활성화 되는 태그
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_State_Sanity_InLightBlock); // 80 이상이면 빛구역이어도 회복 못하도록
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_State_Status_Attack);
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_State_Status_Downed); // 내 캐릭터 기절
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_State_Status_Dead); // 내 캐릭터 사망
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_State_Status_Rescuing); // 친구를 살려주고 있는가
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_State_Status_PickUpItem);
	
	// 아이템 사용 관련 태그
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_State_Item);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_State_Item_Consuming); // 아이템 사용 중
	
	//================================
	// State / Status 관련 상태이상
	//================================
	
	// Poison 관련 태그
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_State_Status_Poison);		// 독 상태이상
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_State_Status_Immune_Poison);	// 독 면역 (해독제)
	
	// Temperature 관련 상태이상 태그
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_State_Status_Temperature_Increase);	// 온도 상승 상태이상
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_State_Status_Temperature_Decrease);	// 온도 감소 상태이상
	
	// Temperature 면역 관련 상태이상 태그
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_State_Status_Immune_Temperature_Increase);	// 온도 상승 면역
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_State_Status_Immune_Temperature_Decrease);	// 온도 감소 면역
	
	//================================
	// State Modifier (방어구 효과)
	//================================
	// 스탯/효과 증가 계열
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_State_Modifier); // 스탯/효과 변경 계열 상위 태그
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_State_Modifier_DAMAGE_REFLECT);		// 피해 반사율 증가
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_State_Modifier_DAMAGE_REDUCTION);	// 피해 감소율 증가
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_State_Modifier_MOVE_SPEED);			// 이동 속도 증가
	
	//================================
	// Input 관련 태그
	//================================
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Input_HotKey);
	
	//================================
	// Data 관련 태그 정의
	//================================
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Data_Damage_Fall);
	
	//================================
	// Event 관련 태그 (몽타주 노티파이 등)
	//================================
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Event_Montage_HitCheck);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Event_Armor_Hit);
	
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Enemy_Damage);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Ability_Hit);
}

namespace MonsterTags
{
	// 식별용
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Character_Type_Player);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Character_Type_Monster);
	
	// 상태용
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_State_Dead);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_State_Hit);
	
	// AI 공격용
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Ability_Attack);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Ability_Sprint);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Cooldown_Attack);
	
	// 이벤트용
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Event_Montage_Hit);
}