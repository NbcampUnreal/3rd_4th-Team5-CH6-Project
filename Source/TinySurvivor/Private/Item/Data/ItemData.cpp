// ItemData.cpp
#include "Item/Data/ItemData.h"

// 로그 카테고리 정의
DEFINE_LOG_CATEGORY_STATIC(LogFItemData, Log, All);

void FItemData::PrintDebugInfo() const
{
	UE_LOG(LogFItemData, Display, TEXT("============= Item Debug Info ============="));
	
	// Base Identifiers
	UE_LOG(LogFItemData, Display, TEXT("---[Identifier]"));
	UE_LOG(LogFItemData, Display, TEXT("ItemID: %d"), ItemID);
	UE_LOG(LogFItemData, Display, TEXT("MainCategory: %s"), *UEnum::GetValueAsString(MainCategory));
	UE_LOG(LogFItemData, Display, TEXT("Name_KR: %s"), *Name_KR.ToString());
	UE_LOG(LogFItemData, Display, TEXT("Name_EN: %s"), *Name_EN.ToString());
	
	// Base System
	UE_LOG(LogFItemData, Display, TEXT("\n---[System]"));
	UE_LOG(LogFItemData, Display, TEXT("Category: %s"), *UEnum::GetValueAsString(Category));
	UE_LOG(LogFItemData, Display, TEXT("AnimType: %s"), *UEnum::GetValueAsString(AnimType))
	UE_LOG(LogFItemData, Display, TEXT("Rarity: %s"), *UEnum::GetValueAsString(Rarity));
	UE_LOG(LogFItemData, Display, TEXT("MaxStack: %d"), MaxStack);
	
	// Category-specific data
	UE_LOG(LogFItemData, Display, TEXT("\n---[Category Data]"));
	switch (Category)
	{
		case EItemCategory::WEAPON:
			UE_LOG(LogFItemData, Display, TEXT("WeaponData:"));
			UE_LOG(LogFItemData, Display, TEXT("  Equipable: %s"), WeaponData.bEquipable ? TEXT("True") : TEXT("False"));
			UE_LOG(LogFItemData, Display, TEXT("  DamageValue: %.2f"), WeaponData.DamageValue);
			UE_LOG(LogFItemData, Display, TEXT("  AttackSpeed: %.2f"), WeaponData.AttackSpeed);
			UE_LOG(LogFItemData, Display, TEXT("  AttackRange: %.2f"), WeaponData.AttackRange);
			UE_LOG(LogFItemData, Display, TEXT("  MaxDurability: %d"), WeaponData.MaxDurability);
			UE_LOG(LogFItemData, Display, TEXT("  DurabilityLossAmount: %d"), WeaponData.DurabilityLossAmount);
			break;
		
		case EItemCategory::TOOL:
			UE_LOG(LogFItemData, Display, TEXT("ToolData:"));
			UE_LOG(LogFItemData, Display, TEXT("  Equipable: %s"), ToolData.bEquipable ? TEXT("True") : TEXT("False"));
			UE_LOG(LogFItemData, Display, TEXT("  DamageValue: %.2f"), ToolData.DamageValue);
			UE_LOG(LogFItemData, Display, TEXT("  AttackSpeed: %.2f"), ToolData.AttackSpeed);
			UE_LOG(LogFItemData, Display, TEXT("  AttackRange: %.2f"), ToolData.AttackRange);
			UE_LOG(LogFItemData, Display, TEXT("  MaxDurability: %d"), ToolData.MaxDurability);
			UE_LOG(LogFItemData, Display, TEXT("  DurabilityLossAmount: %d"), ToolData.DurabilityLossAmount);
			UE_LOG(LogFItemData, Display, TEXT("  HarvestTargetTag: %s"), *ToolData.HarvestTargetTag.ToString());
			break;
		
		case EItemCategory::CONSUMABLE:
			UE_LOG(LogFItemData, Display, TEXT("ConsumableData:"));
			UE_LOG(LogFItemData, Display, TEXT("  ConsumptionTime: %.2f"), ConsumableData.ConsumptionTime);
			UE_LOG(LogFItemData, Display, TEXT("  EffectDuration: %.2f"), ConsumableData.EffectDuration);
			UE_LOG(LogFItemData, Display, TEXT("  DecayEnabled: %s"), ConsumableData.bDecayEnabled ? TEXT("True") : TEXT("False"));
			UE_LOG(LogFItemData, Display, TEXT("  DecayRate: %.2f"), ConsumableData.DecayRate);
			break;
	
		case EItemCategory::ARMOR:
			UE_LOG(LogFItemData, Display, TEXT("ArmorData:"));
			UE_LOG(LogFItemData, Display, TEXT("  EquipSlot: %s"), *UEnum::GetValueAsString(ArmorData.EquipSlot));
			UE_LOG(LogFItemData, Display, TEXT("  HealthBonus: %.2f"), ArmorData.HealthBonus);
			UE_LOG(LogFItemData, Display, TEXT("  DamageReductionRate: %.2f"), ArmorData.DamageReductionRate);
			UE_LOG(LogFItemData, Display, TEXT("  MoveSpeedBonus: %.2f"), ArmorData.MoveSpeedBonus);
			UE_LOG(LogFItemData, Display, TEXT("  SpecialEffectID: %d"), ArmorData.SpecialEffectID);
			UE_LOG(LogFItemData, Display, TEXT("  RequiredRarity: %d"), ArmorData.RequiredRarity);
			UE_LOG(LogFItemData, Display, TEXT("  MaxDurability: %d"), ArmorData.MaxDurability);
			break;
	
		case EItemCategory::MATERIAL:
			UE_LOG(LogFItemData, Display, TEXT("Material: (No additional data)"));
			break;
	}
	
	// Effect
	UE_LOG(LogFItemData, Display, TEXT("\n---[Effect]"));
	if (EffectTag.IsValid())
	{
		UE_LOG(LogFItemData, Display, TEXT("EffectTag: %s"), *EffectTag.ToString());
		UE_LOG(LogFItemData, Display, TEXT("EffectValue: %.2f"), EffectValue);
	}
	else
	{
		UE_LOG(LogFItemData, Display, TEXT("No effect"));
	}
	
	// Visual
	UE_LOG(LogFItemData, Display, TEXT("\n---[Visual]"));
	UE_LOG(LogFItemData, Display, TEXT("Icon: %s"), Icon.IsNull() ? TEXT("None") : *Icon.ToString());
	UE_LOG(LogFItemData, Display, TEXT("WorldMesh: %s"), WorldMesh.IsNull() ? TEXT("None") : *WorldMesh.ToString());
	
	// Spawn
	UE_LOG(LogFItemData, Display, TEXT("\n---[Spawn]"));
	UE_LOG(LogFItemData, Display, TEXT("bSpawnAsActor: %s"), bSpawnAsActor ? TEXT("True") : TEXT("False"));
	UE_LOG(LogFItemData, Display, TEXT("ActorClass: %s"),
		ActorClass ? *ActorClass->GetName() : TEXT("None"));
	
	// Ability
	UE_LOG(LogFItemData, Display, TEXT("\n---[Ability]"));
	UE_LOG(LogFItemData, Display, TEXT("AbilityBP: %s"),
		AbilityBP ? *AbilityBP->GetName() : TEXT("None"));
	
	// Final helper info
	UE_LOG(LogFItemData, Display, TEXT("\n---[Helper Checks]"));
	UE_LOG(LogFItemData, Display, TEXT("IsEquipable: %s"), IsEquipable() ? TEXT("True") : TEXT("False"));
	UE_LOG(LogFItemData, Display, TEXT("HasDurability: %s"), HasDurability() ? TEXT("True") : TEXT("False"));
	UE_LOG(LogFItemData, Display, TEXT("MaxDurability: %d"), GetMaxDurability());
	UE_LOG(LogFItemData, Display, TEXT("IsStackable: %s"), IsStackable() ? TEXT("True") : TEXT("False"));
	UE_LOG(LogFItemData, Display, TEXT("IsIconValid: %s"), IsIconValid() ? TEXT("True") : TEXT("False"));
	UE_LOG(LogFItemData, Display, TEXT("IsWorldMeshValid: %s"), IsWorldMeshValid() ? TEXT("True") : TEXT("False"));
	UE_LOG(LogFItemData, Display, TEXT("CanSpawnAsActor: %s"), CanSpawnAsActor() ? TEXT("True") : TEXT("False"));
	UE_LOG(LogFItemData, Display, TEXT("HasAbility: %s"), HasAbility() ? TEXT("True") : TEXT("False"));
	UE_LOG(LogFItemData, Display, TEXT("ValidateCategoryData: %s"), ValidateCategoryData() ? TEXT("True") : TEXT("False"));
	
	UE_LOG(LogFItemData, Display, TEXT("============================================"));
}