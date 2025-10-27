// ItemGameplayTags.cpp
#include "GameplayTags/ItemGameplayTags.h"

namespace ItemTags
{
	//========================================
	// 루트 태그
	//========================================
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item, "Item");

	//========================================
	// 카테고리 (대분류)
	//========================================
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Category, "Item.Category");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Category_Material, "Item.Category.Material");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Category_Tool, "Item.Category.Tool");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Category_Weapon, "Item.Category.Weapon");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Category_Consumable, "Item.Category.Consumable");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Category_Building, "Item.Category.Building");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Category_Story, "Item.Category.Story");

	//========================================
	// 재료 세부 분류
	//========================================
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Material_Paper, "Item.Category.Material.Paper");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Material_Rubber, "Item.Category.Material.Rubber");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Material_Hair, "Item.Category.Material.Hair");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Material_Stick, "Item.Category.Material.Stick");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Material_Log, "Item.Category.Material.Log");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Material_Rock, "Item.Category.Material.Rock");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Material_Plastic, "Item.Category.Material.Plastic");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Material_Steel, "Item.Category.Material.Steel");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Material_Gunpowder, "Item.Category.Material.Gunpowder");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Material_Rice, "Item.Category.Material.Rice");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Material_Flour, "Item.Category.Material.Flour");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Material_Glass, "Item.Category.Material.Glass");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Material_Bottle, "Item.Category.Material.Bottle");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Material_Thread, "Item.Category.Material.Thread");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Material_PlasticProcessed, "Item.Category.Material.PlasticProcessed");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Material_SteelProcessed, "Item.Category.Material.SteelProcessed");
	
	//========================================
	// 도구 세부 분류
	//========================================
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Tool_Pickaxe, "Item.Category.Tool.Pickaxe");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Tool_Axe, "Item.Category.Tool.Axe");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Tool_Hammer, "Item.Category.Tool.Hammer");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Tool_Shovel, "Item.Category.Tool.Shovel");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Tool_Slingshot, "Item.Category.Tool.Slingshot");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Tool_Lantern, "Item.Category.Tool.Lantern");
	
	//========================================
	// 도구 재질별 분류
	//========================================
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Tool_Pickaxe_Wood, "Item.Category.Tool.Pickaxe.Wood");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Tool_Pickaxe_Stone, "Item.Category.Tool.Pickaxe.Stone");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Tool_Pickaxe_Plastic, "Item.Category.Tool.Pickaxe.Plastic");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Tool_Pickaxe_Steel, "Item.Category.Tool.Pickaxe.Steel");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Tool_Axe_Wood, "Item.Category.Tool.Axe.Wood");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Tool_Axe_Stone, "Item.Category.Tool.Axe.Stone");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Tool_Axe_Plastic, "Item.Category.Tool.Axe.Plastic");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Tool_Axe_Steel, "Item.Category.Tool.Axe.Steel");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Tool_Slingshot_Wood, "Item.Category.Tool.Slingshot.Wood");
	
	//========================================
	// 무기 세부 분류
	//========================================
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Weapon_Melee, "Item.Category.Weapon.Melee");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Weapon_Ranged, "Item.Category.Weapon.Ranged");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Weapon_Slingshot, "Item.Category.Weapon.Slingshot");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Weapon_Blade, "Item.Category.Weapon.Blade");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Weapon_Spear, "Item.Category.Weapon.Spear");
	
	//========================================
	// 소모품 세부 분류
	//========================================
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Consumable_Food, "Item.Category.Consumable.Food");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Consumable_Drink, "Item.Category.Consumable.Drink");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Consumable_Potion, "Item.Category.Consumable.Potion");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Consumable_Buff, "Item.Category.Consumable.Buff");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Consumable_Cookie, "Item.Category.Consumable.Food.Cookie");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Consumable_Water, "Item.Category.Consumable.Drink.Water");
	
	//========================================
	// 건축물 세부 분류
	//========================================
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Building_Workbench, "Item.Category.Building.Workbench");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Building_Cooker, "Item.Category.Building.Cooker");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Building_Chest, "Item.Category.Building.Chest");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Building_Brazier, "Item.Category.Building.Brazier");
	
	//========================================
	// 티어
	//========================================
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Tier, "Item.Tier");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Tier_T1, "Item.Tier.T1");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Tier_T2, "Item.Tier.T2");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Tier_T3, "Item.Tier.T3");
	
	//========================================
	// 등급 (Rarity)
	//========================================
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Rarity, "Item.Rarity");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Rarity_None, "Item.Rarity.None");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Rarity_Common, "Item.Rarity.Common");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Rarity_Normal, "Item.Rarity.Normal");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Rarity_Rare, "Item.Rarity.Rare");
	
	//========================================
	// 특성 태그
	//========================================
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Property, "Item.Property");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Property_Stackable, "Item.Property.Stackable");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Property_Equipable, "Item.Property.Equipable");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Property_Combustible, "Item.Property.Combustible");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Property_Consumable, "Item.Property.Consumable");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Property_Durability, "Item.Property.Durability");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Property_LightSource, "Item.Property.LightSource");
	
	//========================================
	// 로직 태그
	//========================================
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Logic, "Item.Logic");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Logic_ConsumedByCurse, "Item.Logic.ConsumedByCurse");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Logic_LightSource, "Item.Logic.LightSource");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Logic_HasFreshness, "Item.Logic.HasFreshness");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Logic_QuestFlag, "Item.Logic.QuestFlag");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Logic_NoStack, "Item.Logic.NoStack");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Logic_RequiresBullet, "Item.Logic.RequiresBullet");
	
	//========================================
	// 사용 대상
	//========================================
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_UseTarget, "Item.UseTarget");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_UseTarget_Self, "Item.UseTarget.Self");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_UseTarget_Other, "Item.UseTarget.Other");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_UseTarget_Field, "Item.UseTarget.Field");

	//========================================
	// 획득 방식
	//========================================
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Acquisition, "Item.Acquisition");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Acquisition_Harvest, "Item.Acquisition.Harvest");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Acquisition_MonsterDrop, "Item.Acquisition.MonsterDrop");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Acquisition_Chest, "Item.Acquisition.Chest");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Acquisition_Craft, "Item.Acquisition.Craft");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Acquisition_Interaction, "Item.Acquisition.Interaction");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Acquisition_Install, "Item.Acquisition.Install");
	
	//========================================
	// 크래프팅 제작대
	//========================================
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Crafting, "Item.Crafting");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Crafting_Workbench, "Item.Crafting.Workbench");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Crafting_Brazier, "Item.Crafting.Brazier");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Crafting_Cooker, "Item.Crafting.Cooker");
	
	//========================================
	// 효과 태그
	//========================================
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Effect, "Item.Effect");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Effect_Hunger, "Item.Effect.Hunger");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Effect_Thirst, "Item.Effect.Thirst");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Effect_Health, "Item.Effect.Health");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Effect_Sanity, "Item.Effect.Sanity");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Effect_Damage, "Item.Effect.Damage");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Effect_Buff, "Item.Effect.Buff");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Effect_Debuff, "Item.Effect.Debuff");
	
	//========================================
	// 구체적 아이템 ID (재료 001-016)
	//========================================
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_ID_001, "Item.ID.001"); // 종이
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_ID_002, "Item.ID.002"); // 고무
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_ID_003, "Item.ID.003"); // 머리카락
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_ID_004, "Item.ID.004"); // 막대
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_ID_005, "Item.ID.005"); // 통나무
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_ID_006, "Item.ID.006"); // 돌
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_ID_007, "Item.ID.007"); // 플라스틱 조각
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_ID_008, "Item.ID.008"); // 강철 조각
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_ID_009, "Item.ID.009"); // 화약
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_ID_010, "Item.ID.010"); // 쌀
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_ID_011, "Item.ID.011"); // 밀가루
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_ID_012, "Item.ID.012"); // 유리
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_ID_013, "Item.ID.013"); // 유리병
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_ID_014, "Item.ID.014"); // 실
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_ID_015, "Item.ID.015"); // 가공된 플라스틱
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_ID_016, "Item.ID.016"); // 가공된 강철

	//========================================
	// 구체적 아이템 ID (도구 017-025)
	//========================================
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_ID_017, "Item.ID.017"); // 나무 곡괭이
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_ID_018, "Item.ID.018"); // 돌 곡괭이
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_ID_019, "Item.ID.019"); // 플라스틱 곡괭이
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_ID_020, "Item.ID.020"); // 강철 곡괭이
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_ID_021, "Item.ID.021"); // 나무 도끼
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_ID_022, "Item.ID.022"); // 돌 도끼
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_ID_023, "Item.ID.023"); // 플라스틱 도끼
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_ID_024, "Item.ID.024"); // 강철 도끼
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_ID_025, "Item.ID.025"); // 나무 새총

	//========================================
	// 구체적 아이템 ID (소모품 026-027)
	//========================================
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_ID_026, "Item.ID.026"); // 쿠키 부스러기
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_ID_027, "Item.ID.027"); // 물이 든 물병

	//========================================
	// 구체적 아이템 ID (건축물 001-004)
	//========================================
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Building_ID_001, "Item.Building.ID.001"); // 제작대
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Building_ID_002, "Item.Building.ID.002"); // 요리솥
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Building_ID_003, "Item.Building.ID.003"); // 상자
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Building_ID_004, "Item.Building.ID.004"); // 화로
}
