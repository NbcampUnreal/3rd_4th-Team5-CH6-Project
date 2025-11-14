// ItemGameplayTags.cpp
#include "GameplayTags/ItemGameplayTags.h"

namespace ItemTags
{
#pragma region Item_Root
	//========================================
	// 루트 태그
	//========================================
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item, "Item");
#pragma endregion

#pragma region HarvestTarget
	//========================================
	// 채취 대상 자원 태그
	//========================================
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_HarvestTarget, "Item.HarvestTarget");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_HarvestTarget_Paper, "Item.HarvestTarget.Paper");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_HarvestTarget_Plastic, "Item.HarvestTarget.Plastic");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_HarvestTarget_Wood, "Item.HarvestTarget.Wood");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_HarvestTarget_Stone, "Item.HarvestTarget.Stone");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_HarvestTarget_Metal, "Item.HarvestTarget.Metal");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_HarvestTarget_Glass, "Item.HarvestTarget.Glass");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_HarvestTarget_Rubber, "Item.HarvestTarget.Rubber");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_HarvestTarget_Fabric, "Item.HarvestTarget.Fabric");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_HarvestTarget_Food, "Item.HarvestTarget.Food");
#pragma endregion
	
#pragma region Effect
	//========================================
	// 소모품 효과 태그 (기획서 C-4-1 매핑)
	//========================================
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Effect, "Item.Effect");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Effect_HP_HEAL, "Item.Effect.HP_HEAL");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Effect_SANITY_RESTORE, "Item.Effect.SANITY_RESTORE");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Effect_TEMP_SPEED_BUFF, "Item.Effect.TEMP_SPEED_BUFF");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Effect_CURE_DEBUFF, "Item.Effect.CURE_DEBUFF");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Effect_TEMP_ADJUST, "Item.Effect.TEMP_ADJUST");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Effect_THIRST_RESTORE, "Item.Effect.THIRST_RESTORE");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Effect_HUNGER_RESTORE, "Item.Effect.HUNGER_RESTORE");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Effect_POISON_HEAL, "Item.Effect.POISON_HEAL");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Effect_HP_DAMAGE, "Item.Effect.HP_DAMAGE");
#pragma endregion

#pragma region Temp
	//========================================
	// (임시)
	// 설치 구역 제한용 임시 태그
	//========================================
	UE_DEFINE_GAMEPLAY_TAG(TAG_Building_Disallowed_CurseSource, "Item.Building.Disallowed.CurseSource");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Building_Disallowed_GiantPath, "Item.Building.Disallowed.GiantPath");

	//========================================
	// (임시)
	// 아이템 티어 (등급)
	//========================================
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Tier, "Item.Tier");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Tier_T1, "Item.Tier.T1");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Tier_T2, "Item.Tier.T2");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Tier_T3, "Item.Tier.T3");
	
	//========================================
	// (임시)
	// 아이템 등급 (Rarity)
	//========================================
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Rarity, "Item.Rarity");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Rarity_None, "Item.Rarity.None");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Rarity_Common, "Item.Rarity.Common");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Rarity_Normal, "Item.Rarity.Normal");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Rarity_Rare, "Item.Rarity.Rare");
	
	//========================================
	// (임시)
	// 사용 대상 (UseTarget)
	// 기획: 자신, 타인, 필드
	//========================================
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_UseTarget, "Item.UseTarget");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_UseTarget_Self, "Item.UseTarget.Self");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_UseTarget_Other, "Item.UseTarget.Other");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_UseTarget_Field, "Item.UseTarget.Field");

	//========================================
	// (임시)
	// 획득 방식 (SourceType)
	//========================================
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Acquisition, "Item.Acquisition");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Acquisition_Harvest, "Item.Acquisition.Harvest");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Acquisition_MonsterDrop, "Item.Acquisition.MonsterDrop");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Acquisition_Chest, "Item.Acquisition.Chest");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Acquisition_Craft, "Item.Acquisition.Craft");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Acquisition_Interaction, "Item.Acquisition.Interaction");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Acquisition_Install, "Item.Acquisition.Install");
#pragma endregion
}
