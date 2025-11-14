// ItemGameplayTags.h
#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NativeGameplayTags.h"

/*
	아이템 시스템 전용 네이티브 게임플레이 태그
	기획 명세 기반 계층 구조 정의
 */
namespace ItemTags
{
#pragma region Item_Root
	//========================================
	// 아이템 루트 태그
	//========================================
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item);
#pragma endregion

#pragma region HarvestTarget
	//========================================
	// 채취 대상 자원 태그 (HarvestTarget)
	// 도구가 채취할 수 있는 자원 원천의 재료 타입
	//========================================
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_HarvestTarget);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_HarvestTarget_Paper);		// 종이 자원 (책, 신문 등)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_HarvestTarget_Plastic);		// 플라스틱 자원 (병, 용기 등)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_HarvestTarget_Wood);		// 나무 자원 (이쑤시개, 나무토막 등)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_HarvestTarget_Stone);		// 돌 자원 (자갈, 암석 등)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_HarvestTarget_Metal);		// 금속 자원 (캔, 철조각 등)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_HarvestTarget_Glass);		// 유리 자원 (유리병, 유리조각 등)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_HarvestTarget_Rubber);		// 고무 자원 (고무줄, 고무장갑 등)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_HarvestTarget_Fabric);		// 섬유 자원 (천, 실 등)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_HarvestTarget_Food);		// 식량 자원 (쌀자루, 밀가루 봉지 등)
#pragma endregion
	
#pragma region Effect
	//========================================
	// 소모품 효과 태그 (ConsumableEffect)
	// 기획서 C-4-1: 소모품의 LogicTag 매핑
	//========================================
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Effect);					// 모든 아이템 효과 기본 태그
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Effect_HP_HEAL);			// 체력 회복
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Effect_SANITY_RESTORE);		// 정신력 회복
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Effect_TEMP_SPEED_BUFF);	// 이동속도 증가 버프
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Effect_CURE_DEBUFF);		// 디버프 해제
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Effect_TEMP_ADJUST);		// 온도 조절
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Effect_THIRST_RESTORE);		// 목마름 회복
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Effect_HUNGER_RESTORE);		// 배고픔 회복
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Effect_POISON_HEAL);		// 해독 (독 해제)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Effect_HP_DAMAGE);			// 체력 감소
#pragma endregion

#pragma region Temp
	//========================================
	// (임시)
	// 설치 구역 제한용 임시 태그
	//========================================
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Building_Disallowed_CurseSource); // CURSE_SOURCE
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Building_Disallowed_GiantPath);   // GIANT_PATH
	
	//========================================
	// (임시)
	// 아이템 티어 (등급)
	//========================================
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Tier);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Tier_T1);             // T1
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Tier_T2);             // T2
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Tier_T3);             // T3
	
	//========================================
	// (임시)
	// 아이템 등급 (Rarity)
	//========================================
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Rarity);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Rarity_None);         // 등급 없음
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Rarity_Common);       // 일반
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Rarity_Normal);       // 보통
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Rarity_Rare);         // 희귀
	
	//========================================
	// (임시)
	// 사용 대상 (UseTarget)
	// 기획: 자신, 타인, 필드
	//========================================
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_UseTarget);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_UseTarget_Self);      // 자신
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_UseTarget_Other);     // 타인
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_UseTarget_Field);     // 필드
	
	//========================================
	// (임시)
	// 획득 방식 (SourceType)
	//========================================
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Acquisition);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Acquisition_Harvest);      // 자원 채취
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Acquisition_MonsterDrop);  // 몬스터 드롭
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Acquisition_Chest);        // 상자
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Acquisition_Craft);        // 크래프팅
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Acquisition_Interaction);  // 상호작용 (필드 내 오브젝트)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Acquisition_Install);      // 설치 (건축물)
#pragma endregion
}
