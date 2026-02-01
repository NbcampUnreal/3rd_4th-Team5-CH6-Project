#include "GameplayTags/NofiticationTags.h"

namespace NotificationTags
{
#pragma region Crafting
	//========================================
	// 크래프팅 알림
	//========================================
	UE_DEFINE_GAMEPLAY_TAG(TAG_Notification_Crafting_Failed, "Notification.Crafting.Failed");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Notification_Crafting_LackingIngredients, "Notification.Crafting.LackingIngredients");
#pragma endregion
#pragma region Building
	//========================================
	// 빌딩 알림
	//========================================
	UE_DEFINE_GAMEPLAY_TAG(TAG_Notification_Building_Failed, "Notification.Building.Failed");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Notification_Building_LackingIngredients, "Notification.Building.LackingIngredients");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Notification_Building_CannotPlace, "Notification.Building.CannotPlace");
#pragma endregion
#pragma region Inventory
	//========================================
	// 인벤토리 알림
	//========================================
	UE_DEFINE_GAMEPLAY_TAG(TAG_Notification_Inventory_Failed, "Notification.Inventory.Failed");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Notification_Inventory_CannotPlace, "Notification.Inventory.CannotPlace");
#pragma endregion
#pragma region Resource
	//========================================
	// 자원원천 알림
	//========================================
	UE_DEFINE_GAMEPLAY_TAG(TAG_Notification_Resource_NotToolMatch, "Notification.Resource.NotToolMatch");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Notification_Resource_NotInteract, "Notification.Resource.NotInteract");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Notification_Resource_Interact, "Notification.Resource.Interact");
#pragma endregion
}