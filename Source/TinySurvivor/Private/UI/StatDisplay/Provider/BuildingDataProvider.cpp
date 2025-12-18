#include "UI/StatDisplay/Provider/BuildingDataProvider.h"

#include "GameplayTagContainer.h"
#include "GameplayTags/ItemGameplayTags.h"
#include "Item/Data/BuildingData.h"
#include "Item/System/ItemDataSubsystem.h"

FGameplayTag FBuildingDataProvider::GetCategoryTag() const
{
	switch (BuildingData.BuildingType)
	{
	case EBuildingType::CRAFTING:
		return ItemTags::TAG_Category_Crafting;
	case EBuildingType::STRUCT:
		return ItemTags::TAG_Category_Struct;
	case EBuildingType::LIGHT:
		return ItemTags::TAG_Category_Lighting;
	case EBuildingType::STORAGE:
		return ItemTags::TAG_Category_Storage;
	default:
		return FGameplayTag::EmptyTag;
	}
}

float FBuildingDataProvider::GetStatValue(const FGameplayTag& StatTag) const
{
	// 내구도
	if (StatTag == ItemTags::TAG_Display_Stat_Durability)
	{
		return BuildingData.MaxDurability;
	}
	// 조명 범위
	else if (StatTag == ItemTags::TAG_Display_Stat_LightRadius)
	{
		if (BuildingData.BuildingType == EBuildingType::LIGHT)
		{
			return BuildingData.LightRadius_Units;
		}
	}
	// 침식도 감소량
	else if (StatTag == ItemTags::TAG_Display_Stat_ErosionReduction)
	{
		if (BuildingData.BuildingType == EBuildingType::LIGHT)
		{
			return BuildingData.ErosionReduction;
		}
	}
	// 최대 동작 시간
	else if (StatTag == ItemTags::TAG_Display_Stat_MaxMaintenance)
	{
		if (BuildingData.BuildingType == EBuildingType::LIGHT)
		{
			return BuildingData.MaxMaintenance;
		}
	}
	// 연료 재료(유지비 소모 간격)
	else if (StatTag == ItemTags::TAG_Display_Stat_MaintenanceInterval)
	{
		if (BuildingData.BuildingType == EBuildingType::LIGHT)
		{
			return BuildingData.MaintenanceInterval;
		}
	}
	// 용량
	else if (StatTag == ItemTags::TAG_Display_Stat_StorageSlots)
	{
		if (BuildingData.BuildingType == EBuildingType::STORAGE)
		{
			return BuildingData.StorageSlots;
		}
	}
	// 정신력 회복
	// 위의 태그가 아닌 것들은 이펙트 태그로 간주함
	else if (StatTag != FGameplayTag::EmptyTag)
	{
		if (BuildingData.BuildingType == EBuildingType::LIGHT)
		{
			return BuildingData.SanityRecoveryPerSec;
		}
	}
	return 0.f;
}

FGameplayTagContainer FBuildingDataProvider::GetTagContainer(const FGameplayTag& ContainerTag) const
{
	return FGameplayTagContainer::EmptyContainer;
}

FGameplayTag FBuildingDataProvider::GetEffectTag() const
{
	return FGameplayTag::EmptyTag;
}

int32 FBuildingDataProvider::GetEnumValue(const FGameplayTag& EnumTag) const
{
	// Defense 등급
	if (EnumTag == ItemTags::TAG_Display_Stat_Rarity)
	{
		if (BuildingData.BuildingType == EBuildingType::STRUCT)
		{
			return static_cast<int32>(BuildingData.Rarity);
		}
	}
	return -1;
}

FText FBuildingDataProvider::GetItemName(const FGameplayTag& StatTag) const
{
	// 연료 아이템 이름 반환
	if (!WorldContext || StatTag != ItemTags::TAG_Display_Stat_MaintenanceInterval)
	{
		return FText::FromString(TEXT("알 수 없음"));
	}
	
	if (UWorld* World = WorldContext->GetWorld())
	{
		if (UGameInstance* GameInstance = World->GetGameInstance())
		{
			if (UItemDataSubsystem* ItemDataSys = GameInstance->GetSubsystem<UItemDataSubsystem>())
			{
				FItemData MaintenanceCostData;
				if(ItemDataSys->GetItemDataSafe(BuildingData.MaintenanceCostID,MaintenanceCostData))
				{
					return MaintenanceCostData.Name_KR;
				}
			}
		}
	}
	return FText::FromString(TEXT("알 수 없음"));
}

int32 FBuildingDataProvider::GetMaintenanceCostID() const
{
	if (BuildingData.BuildingType == EBuildingType::LIGHT)
	{
		return BuildingData.MaintenanceCostID;
	}
	return -1;
}
