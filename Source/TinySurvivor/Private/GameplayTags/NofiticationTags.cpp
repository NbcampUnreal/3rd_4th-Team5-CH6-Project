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
}