// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/StatDisplay/StatViewerFactory.h"

#include "GameplayTags/ItemGameplayTags.h"
#include "UI/StatDisplay/Viewer/Building/CraftingStatViewer.h"
#include "UI/StatDisplay/Viewer/Building/DefenseStatViewer.h"
#include "UI/StatDisplay/Viewer/Building/LightingStatViewer.h"
#include "UI/StatDisplay/Viewer/Building/StorageStatViewer.h"
#include "UI/StatDisplay/Viewer/Item/ArmorStatViewer.h"
#include "UI/StatDisplay/Viewer/Item/ConsumableStatViewer.h"
#include "UI/StatDisplay/Viewer/Item/ToolStatViewer.h"
#include "UI/StatDisplay/Viewer/Item/WeaponStatViewer.h"

UStatViewer* UStatViewerFactory::GetBuilder(const FGameplayTag& CategoryTag)
{
	// 캐시에 있으면 재사용
	if (TObjectPtr<UStatViewer>* CachedBuilder = BuilderCache.Find(CategoryTag))
	{
		return *CachedBuilder;
	}

	// 없으면 새로 생성
	UStatViewer* NewBuilder = CreateBuilderForTag(CategoryTag);
	if (NewBuilder)
	{
		BuilderCache.Add(CategoryTag, NewBuilder);
	}

	return NewBuilder;
}

UStatViewer* UStatViewerFactory::CreateBuilderForTag(const FGameplayTag& CategoryTag)
{
	// 아이템 카테고리
	if (CategoryTag == ItemTags::TAG_Category_Tool)
	{
		return NewObject<UToolStatViewer>(this);
	}
	else if (CategoryTag == ItemTags::TAG_Category_Weapon)
	{
		return NewObject<UWeaponStatViewer>(this);
	}
	else if (CategoryTag == ItemTags::TAG_Category_Armor)
	{
		return NewObject<UArmorStatViewer>(this);
	}
	else if (CategoryTag == ItemTags::TAG_Category_Consumable)
	{
		return NewObject<UConsumableStatViewer>(this);
	}
	// 빌딩 카테고리
	else if (CategoryTag == ItemTags::TAG_Category_Storage)
	{
		return NewObject<UStorageStatViewer>(this);
	}
	else if (CategoryTag == ItemTags::TAG_Category_Crafting)
	{
		return NewObject<UCraftingStatViewer>(this);
	}
	else if (CategoryTag == ItemTags::TAG_Category_Lighting)
	{
		return NewObject<ULightingStatViewer>(this);
	}
	else if (CategoryTag == ItemTags::TAG_Category_Struct)
	{
		return NewObject<UDefenseStatViewer>(this);
	}
	return nullptr;
}
