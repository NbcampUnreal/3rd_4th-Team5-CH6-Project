// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"

namespace ResourceUniqueTags
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(ResourceUniqueTags);
	
	namespace Wood
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(ResourceUniqueTags_Wood);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(ResourceUniqueTags_Wood_WoodStick); // 이쑤시개

	}

	namespace Plastic
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(ResourceUniqueTags_Plastic);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(ResourceUniqueTags_Plastic_PlasticBox); // 플라스틱 박스
	}
	
	namespace Plant
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(ResourceUniqueTags_Plant);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(ResourceUniqueTags_Plant_BundledPaper); // 폐지 더미
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(ResourceUniqueTags_Plant_DriedVine); // 마른 덩굴 식물
		
	}

	namespace Oil
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(ResourceUniqueTags_Oil);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(ResourceUniqueTags_Oil_Oil); // 기름
		
	}
	namespace Junk
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(ResourceUniqueTags_Junk);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(ResourceUniqueTags_Junk_JunkCan); // 녹슨 캔
		
	}
	
	namespace Interact
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(ResourceUniqueTags_Interact);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(ResourceUniqueTags_Interact_HeavyPotLid); // 무거운 항아리 뚜껑
	}
	
	namespace Glass
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(ResourceUniqueTags_Glass);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(ResourceUniqueTags_Glass_ShatteredBottle); // 깨진 음료수 병
		
	}
	
	namespace Food
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(ResourceUniqueTags_Food);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(ResourceUniqueTags_Food_CookieCrumb); // 과자 부스러기
	}
	
	namespace Drink
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(ResourceUniqueTags_Drink);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(ResourceUniqueTags_Drink_WaterDrop); // 물방울
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(ResourceUniqueTags_Drink_SoftDrink); // 음료수 
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(ResourceUniqueTags_Drink_DiscardedDoll);
	}
	
}