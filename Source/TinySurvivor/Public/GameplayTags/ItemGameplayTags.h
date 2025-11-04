// ItemGameplayTags.h
#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NativeGameplayTags.h"

/*
	아이템 시스템 전용 네이티브 게임플레이 태그
	기획 명세 기반 계층 구조 정의
	기획 명세: 아이템 ID는 숫자(001~999)이지만,
	내부적으로 GameplayTag로 관리하여 GAS 통합 및 계층 구조 활용
 */
namespace ItemTags
{
	//========================================
	// 아이템 루트 태그
	//========================================
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item);

	//========================================
	// 아이템 카테고리 (대분류)
	// 기획: MATERIAL, TOOL, WEAPON, CONSUMABLE, BUILDING, STORY
	//========================================
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Category);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Category_Material);		// 재료 (ID: 001-199)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Category_Tool);			// 도구 (ID: 200-299)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Category_Weapon);		// 무기 (ID: 300-399)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Category_Consumable);	// 소모품
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Category_Building);		// 건축물
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Category_Story);		// 단서/로어

	//========================================
	// 채취 대상 자원 태그 (HarvestTarget)
	// 도구가 채취할 수 있는 자원 원천의 재료 타입
	//========================================
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_HarvestTarget);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_HarvestTarget_Paper);       // 종이 자원 (책, 신문 등)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_HarvestTarget_Plastic);     // 플라스틱 자원 (병, 용기 등)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_HarvestTarget_Wood);        // 나무 자원 (이쑤시개, 나무토막 등)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_HarvestTarget_Stone);       // 돌 자원 (자갈, 암석 등)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_HarvestTarget_Metal);       // 금속 자원 (캔, 철조각 등)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_HarvestTarget_Glass);       // 유리 자원 (유리병, 유리조각 등)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_HarvestTarget_Rubber);      // 고무 자원 (고무줄, 고무장갑 등)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_HarvestTarget_Fabric);      // 섬유 자원 (천, 실 등)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_HarvestTarget_Food);        // 식량 자원 (쌀자루, 밀가루 봉지 등)

	//========================================
	// 소모품 효과 태그 (ConsumableEffect)
	// 기획서 C-4-1: 소모품의 LogicTag 매핑
	//========================================
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Effect);                     // 모든 아이템 효과 기본 태그
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Effect_HP_HEAL);             // 체력 회복
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Effect_SANITY_RESTORE);      // 정신력 회복
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Effect_TEMP_SPEED_BUFF);     // 이동속도 증가 버프
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Effect_CURE_DEBUFF);         // 디버프 해제
	// UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Effect_HUNGER_RESTORE);   // 배고픔 회복
	// UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Effect_THIRST_RESTORE);   // 갈증 회복

	//========================================
	// 설치 구역 제한용 임시 태그
	//========================================
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Building_Disallowed_CurseSource); // CURSE_SOURCE
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Building_Disallowed_GiantPath);   // GIANT_PATH
	
	//========================================
	// (임시)
	// 재료 세부 분류 (Material Subtypes)
	//========================================
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Material_Paper);			// 종이 (ID: 001)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Material_Rubber);			// 고무 (ID: 002)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Material_Hair);				// 머리카락 (ID: 003)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Material_Stick);			// 막대 (ID: 004)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Material_Log);				// 통나무 (ID: 005)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Material_Rock);				// 돌 (ID: 006)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Material_Plastic);			// 플라스틱 조각 (ID: 007)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Material_Steel);			// 강철 조각 (ID: 008)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Material_Gunpowder);		// 화약 (ID: 009)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Material_Rice);				// 쌀 (ID: 010)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Material_Flour);			// 밀가루 (ID: 011)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Material_Glass);			// 유리 (ID: 012)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Material_Bottle);			// 유리병 (ID: 013)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Material_Thread);			// 실 (ID: 014)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Material_PlasticProcessed);	// 가공된 플라스틱 (ID: 015)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Material_SteelProcessed);	// 가공된 강철 (ID: 016)

	//========================================
	// (임시)
	// 도구 세부 분류 (Tool Subtypes)
	//========================================
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Tool_Pickaxe);        // 곡괭이
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Tool_Axe);            // 도끼
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Tool_Hammer);         // 망치
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Tool_Shovel);         // 삽
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Tool_Slingshot);      // 새총
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Tool_Lantern);        // 랜턴

	//========================================
	// (임시)
	// 도구 재질별 분류 (Tool Material)
	//========================================
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Tool_Pickaxe_Wood);      // 나무 곡괭이 (ID: 017)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Tool_Pickaxe_Stone);     // 돌 곡괭이 (ID: 018)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Tool_Pickaxe_Plastic);   // 플라스틱 곡괭이 (ID: 019)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Tool_Pickaxe_Steel);     // 강철 곡괭이 (ID: 020)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Tool_Axe_Wood);          // 나무 도끼 (ID: 021)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Tool_Axe_Stone);         // 돌 도끼 (ID: 022)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Tool_Axe_Plastic);       // 플라스틱 도끼 (ID: 023)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Tool_Axe_Steel);         // 강철 도끼 (ID: 024)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Tool_Slingshot_Wood);    // 나무 새총 (ID: 025)
	
	//========================================
	// (임시)
	// 무기 세부 분류 (Weapon Subtypes)
	//========================================
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Weapon_Melee);        // 근접 무기
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Weapon_Ranged);       // 원거리 무기
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Weapon_Slingshot);    // 새총
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Weapon_Blade);        // 칼날류
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Weapon_Spear);        // 창류

	//========================================
	// (임시)
	// 소모품 세부 분류 (Consumable Subtypes)
	//========================================
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Consumable_Food);     // 음식
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Consumable_Drink);    // 음료
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Consumable_Potion);   // 포션
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Consumable_Buff);     // 버프 아이템
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Consumable_Cookie);   // 쿠키 부스러기 (ID: 026)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Consumable_Water);    // 물이 든 물병 (ID: 027)

	//========================================
	// (임시)
	// 건축물 세부 분류 (Building Subtypes)
	//========================================
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Building_Workbench);  // 제작대 (ID: 001)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Building_Cooker);     // 요리솥 (ID: 002)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Building_Chest);      // 상자 (ID: 003)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Building_Brazier);    // 화로 (ID: 004)
	
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
	// 아이템 특성 태그
	//========================================
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Property);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Property_Stackable);       // 겹칠 수 있음
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Property_Equipable);       // 장착 가능
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Property_Combustible);     // 연소 가능
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Property_Consumable);      // 소모 가능
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Property_Durability);      // 내구도 있음
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Property_LightSource);     // 광원
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Property_ConsumedByCurse); // 저주 유지비
	
	//========================================
	// (임시)
	// 아이템 로직 태그 (Logic)
	//========================================
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Logic);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Logic_ConsumedByCurse);   // 저주 유지비 소모 대상
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Logic_LightSource);       // 광원 (Sanity 회복)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Logic_HasFreshness);      // 신선도 시스템 적용
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Logic_QuestFlag);         // 퀘스트 관련 아이템
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Logic_NoStack);           // 중첩 불가
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Logic_RequiresBullet);    // 총알 필요 (새총)
	
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
	// 획득 방식 태그 (SourceType)
	//========================================
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Acquisition);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Acquisition_Harvest);      // 자원 채취
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Acquisition_MonsterDrop);  // 몬스터 드롭
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Acquisition_Chest);        // 상자
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Acquisition_Craft);        // 크래프팅
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Acquisition_Interaction);  // 상호작용 (필드 내 오브젝트)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Acquisition_Install);      // 설치 (건축물)
	
	//========================================
	// (임시)
	// 크래프팅 제작대 태그
	//========================================
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Crafting);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Crafting_Workbench);     // 제작대 (작업대)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Crafting_Brazier);       // 화로
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Crafting_Cooker);        // 요리솥
	
	//========================================
	// 효과 태그 (Effect) - 임시
	// 소모품 효과, 도구 효과 등
	//========================================
	// UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Effect);
	// UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Effect_Hunger);          // 배고픔 회복
	// UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Effect_Thirst);          // 갈증 회복
	// UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Effect_Health);          // HP 회복
	// UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Effect_Sanity);          // Sanity 회복
	// UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Effect_Damage);          // 대미지
	// UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Effect_Buff);            // 버프
	// UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Effect_Debuff);          // 디버프

	//========================================
	// (임시)
	// 구체적 아이템 ID 예시 (기획 명세 ID 매핑)
	// 재료: 001-199 (추정)
	// 도구: 200-299 (추정)
	// 무기: 300-399 (추정)
	//========================================
	
	// [재료 001-016]
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_ID_001);  // 종이
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_ID_002);  // 고무
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_ID_003);  // 머리카락
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_ID_004);  // 막대
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_ID_005);  // 통나무
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_ID_006);  // 돌
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_ID_007);  // 플라스틱 조각
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_ID_008);  // 강철 조각
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_ID_009);  // 화약
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_ID_010);  // 쌀
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_ID_011);  // 밀가루
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_ID_012);  // 유리
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_ID_013);  // 유리병
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_ID_014);  // 실
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_ID_015);  // 가공된 플라스틱
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_ID_016);  // 가공된 강철

	// [도구 017-025]
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_ID_017);  // 나무 곡괭이
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_ID_018);  // 돌 곡괭이
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_ID_019);  // 플라스틱 곡괭이
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_ID_020);  // 강철 곡괭이
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_ID_021);  // 나무 도끼
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_ID_022);  // 돌 도끼
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_ID_023);  // 플라스틱 도끼
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_ID_024);  // 강철 도끼
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_ID_025);  // 나무 새총

	// [소모품 026-027]
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_ID_026);  // 쿠키 부스러기
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_ID_027);  // 물이 든 물병

	// [건축물 001-004] (별도 ID 체계)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Building_ID_001);  // 제작대
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Building_ID_002);  // 요리솥
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Building_ID_003);  // 상자
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Building_ID_004);  // 화로
}
