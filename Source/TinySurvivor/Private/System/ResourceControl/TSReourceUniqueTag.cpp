// Fill out your copyright notice in the Description page of Project Settings.

#include "System/ResourceControl/TSReourceUniqueTag.h"

namespace ResourceUniqueTags
{
	UE_DEFINE_GAMEPLAY_TAG(ResourceUniqueTags, "ResourceUniqueTags");
	
	namespace Wood
	{
		UE_DEFINE_GAMEPLAY_TAG(ResourceUniqueTags_Wood, "ResourceUniqueTags.Wood");
		UE_DEFINE_GAMEPLAY_TAG(ResourceUniqueTags_Wood_WoodStick, "ResourceUniqueTags.Wood.WoodStick");  // 이쑤시개

	}

	namespace Plastic
	{
		UE_DEFINE_GAMEPLAY_TAG(ResourceUniqueTags_Plastic, "ResourceUniqueTags.Plastic");
		UE_DEFINE_GAMEPLAY_TAG(ResourceUniqueTags_Plastic_PlasticBox, "ResourceUniqueTags.Plastic.PlasticBox");  // 플라스틱 박스
	}
	
	namespace Plant
	{
		UE_DEFINE_GAMEPLAY_TAG(ResourceUniqueTags_Plant, "ResourceUniqueTags.Plant");
		UE_DEFINE_GAMEPLAY_TAG(ResourceUniqueTags_Plant_BundledPaper, "ResourceUniqueTags.Plant.BundledPaper"); // 폐지 더미
		UE_DEFINE_GAMEPLAY_TAG(ResourceUniqueTags_Plant_DriedVine, "ResourceUniqueTags.Plant.DriedVine");  // 마른 덩굴 식물
		
	}

	namespace Oil
	{
		UE_DEFINE_GAMEPLAY_TAG(ResourceUniqueTags_Oil, "ResourceUniqueTags.Oil");
		UE_DEFINE_GAMEPLAY_TAG(ResourceUniqueTags_Oil_Oil, "ResourceUniqueTags.Oil.Oil");  // 기름
		
	}
	namespace Junk
	{
		UE_DEFINE_GAMEPLAY_TAG(ResourceUniqueTags_Junk, "ResourceUniqueTags.Oil");
		UE_DEFINE_GAMEPLAY_TAG(ResourceUniqueTags_Junk_JunkCan, "ResourceUniqueTags.Junk.JunkCan"); // 녹슨 캔
		
	}
	
	namespace Interact
	{
		UE_DEFINE_GAMEPLAY_TAG(ResourceUniqueTags_Interact, "ResourceUniqueTags.Interact");
		UE_DEFINE_GAMEPLAY_TAG(ResourceUniqueTags_Interact_HeavyPotLid, "ResourceUniqueTags.Interact.HeavyPotLid"); // 무거운 항아리 뚜껑
	}
	
	namespace Glass
	{
		UE_DEFINE_GAMEPLAY_TAG(ResourceUniqueTags_Glass, "ResourceUniqueTags.Glass");
		UE_DEFINE_GAMEPLAY_TAG(ResourceUniqueTags_Glass_ShatteredBottle, "ResourceUniqueTags.Glass.ShatteredBottle");  // 깨진 음료수 병
	}
	
	namespace Food
	{
		UE_DEFINE_GAMEPLAY_TAG(ResourceUniqueTags_Food, "ResourceUniqueTags.Food");
		UE_DEFINE_GAMEPLAY_TAG(ResourceUniqueTags_Food_CookieCrumb, "ResourceUniqueTags.Food.CookieCrumb"); // 과자 부스러기
	}
	
	namespace Drink
	{
		UE_DEFINE_GAMEPLAY_TAG(ResourceUniqueTags_Drink, "ResourceUniqueTags.Drink");
		UE_DEFINE_GAMEPLAY_TAG(ResourceUniqueTags_Drink_WaterDrop, "ResourceUniqueTags.Drink.WaterDrop"); // 물방울
		UE_DEFINE_GAMEPLAY_TAG(ResourceUniqueTags_Drink_SoftDrink, "ResourceUniqueTags.Drink.SoftDrink"); // 음료수 
	}
	
	namespace Cloth
	{
		UE_DEFINE_GAMEPLAY_TAG(ResourceUniqueTags_Cloth, "ResourceUniqueTags.Cloth");
		UE_DEFINE_GAMEPLAY_TAG(ResourceUniqueTags_Cloth_DiscardedDoll, "ResourceUniqueTags.Cloth.DiscardedDoll"); // 헝겊 인형
	}
}