#include "UI/StatDisplay/Provider/ItemDataProvider.h"

#include "GameplayTags/ItemGameplayTags.h"
#include "Item/Data/ItemData.h"

FGameplayTag FItemDataProvider::GetCategoryTag() const
{
	switch (ItemData.Category)
	{
	case EItemCategory::ARMOR:
		return ItemTags::TAG_Category_Armor;
	case EItemCategory::CONSUMABLE:
		return ItemTags::TAG_Category_Consumable;
	case EItemCategory::WEAPON:
		return ItemTags::TAG_Category_Weapon;
	case EItemCategory::TOOL:
		return ItemTags::TAG_Category_Tool;
	default:
		return FGameplayTag::EmptyTag;
	}
}

float FItemDataProvider::GetStatValue(const FGameplayTag& StatTag) const
{
	// 내구도
	if (StatTag == ItemTags::TAG_Display_Stat_Durability)
	{
		return ItemData.GetMaxDurability();
	}
	// 공격력
	else if (StatTag == ItemTags::TAG_Display_Stat_AttackDamage)
	{
		if (ItemData.Category == EItemCategory::TOOL)
		{
			return ItemData.ToolData.DamageValue;
		}
		else if (ItemData.Category == EItemCategory::WEAPON)
		{
			return ItemData.WeaponData.DamageValue;
		}
	}
	// 공격 사거리
	else if (StatTag == ItemTags::TAG_Display_Stat_AttackRange)
	{
		if (ItemData.Category == EItemCategory::TOOL)
		{
			return ItemData.ToolData.AttackRange;
		}
		else if (ItemData.Category == EItemCategory::WEAPON)
		{
			return ItemData.WeaponData.AttackRange;
		}
	}
	// 지속 시간(소모품 전용)
	else if (StatTag == ItemTags::TAG_Display_Stat_EffectDuration)
	{
		if (ItemData.Category == EItemCategory::CONSUMABLE)
		{
			return ItemData.ConsumableData.EffectDuration;
		}
	}
	// 부패 속도(소모품 전용)
	else if (StatTag == ItemTags::TAG_Display_Stat_DecayRate)
	{
		if (ItemData.Category == EItemCategory::CONSUMABLE)
		{
			return ItemData.ConsumableData.DecayRate;
		}
	}
	// 최대 소지 개수
	else if (StatTag == ItemTags::TAG_Display_Stat_MaxStack)
	{
		return ItemData.MaxStack;
	}
	// 효과 수치
	// 위의 태그가 아닌 것들은 이펙트 태그로 간주함
	else if (StatTag != FGameplayTag::EmptyTag)
	{
		return ItemData.EffectValue;
	}
	return 0.f;
}

FGameplayTagContainer FItemDataProvider::GetTagContainer(const FGameplayTag& StatTag) const
{
	// 채취 유형
	if (StatTag == ItemTags::TAG_Display_Stat_HarvestTarget
		&& ItemData.Category == EItemCategory::TOOL)
	{
		return ItemData.ToolData.HarvestTargetTag;
	}
	return FGameplayTagContainer::EmptyContainer;
}

FGameplayTag FItemDataProvider::GetEffectTag() const
{
	// 장착/사용 효과 태그
	if (ItemData.Category == EItemCategory::ARMOR)
	{
		return ItemData.EffectTag_Armor;
	}
	else if (ItemData.Category == EItemCategory::CONSUMABLE)
	{
		return ItemData.EffectTag_Consumable;
	}

	return FGameplayTag::EmptyTag;
}

int32 FItemDataProvider::GetEnumValue(const FGameplayTag& EnumTag) const
{
	// 방어구 장착 슬롯
	if (EnumTag == ItemTags::TAG_Display_Stat_EquipSlot)
	{
		if (ItemData.Category == EItemCategory::ARMOR)
			return static_cast<int32>(ItemData.ArmorData.EquipSlot);
	}

	return -1;
}
