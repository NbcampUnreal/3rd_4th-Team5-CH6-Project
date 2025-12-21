// GameplayCueTags.cpp
#include "GameplayTags/GameplayCueTags.h"

namespace CueTags
{	//================================
	// Item 관련 태그
	//================================
	// 예시
	// UE_DEFINE_GAMEPLAY_TAG(TAG_GameplayCue_Item_VFX, "GameplayCue.Item.VFX");
	
	//================================
	// Gimmick 관련 태그
	//================================
	
	// Poison 관련 태그
	UE_DEFINE_GAMEPLAY_TAG(TAG_GameplayCue_Poison_Status, "GameplayCue.Poison.Status");
	UE_DEFINE_GAMEPLAY_TAG(TAG_GameplayCue_Poison_Material, "GameplayCue.Poison.Material");
	UE_DEFINE_GAMEPLAY_TAG(TAG_GameplayCue_Poison_Bubble, "GameplayCue.Poison.Bubble");
	
	// Electric shock 관련 태그
	UE_DEFINE_GAMEPLAY_TAG(TAG_GameplayCue_ElectricShock_Status, "GameplayCue.ElectricShock.Status");
	UE_DEFINE_GAMEPLAY_TAG(TAG_GameplayCue_ElectricShock_Material, "GameplayCue.ElectricShock.Material");
	UE_DEFINE_GAMEPLAY_TAG(TAG_GameplayCue_ElectricShock_Spark, "GameplayCue.ElectricShock.Spark");
	
	// Material 변경 관련 태그
	UE_DEFINE_GAMEPLAY_TAG(TAG_GameplayCue_Material_Frankenstein, "GameplayCue.Material.Frankenstein");
	UE_DEFINE_GAMEPLAY_TAG(TAG_GameplayCue_Material_Mummy, "GameplayCue.Material.Mummy");
}