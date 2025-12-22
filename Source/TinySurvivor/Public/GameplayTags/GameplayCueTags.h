// GameplayCueTags.h
#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"

namespace CueTags
{
	//================================
	// Item 관련 태그
	//================================
	// 예시
	// UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_GameplayCue_Item_VFX);
	
	//================================
	// Gimmick 관련 태그
	//================================
	
	// Poison 관련 태그
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_GameplayCue_Poison_Status);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_GameplayCue_Poison_Material);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_GameplayCue_Poison_Bubble);
	
	// Electric Shock 관련 태그
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_GameplayCue_ElectricShock_Status);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_GameplayCue_ElectricShock_Material);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_GameplayCue_ElectricShock_Spark);
	
	// Material 변경 관련 태그
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_GameplayCue_Material_Frankenstein);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_GameplayCue_Material_Mummy);
}