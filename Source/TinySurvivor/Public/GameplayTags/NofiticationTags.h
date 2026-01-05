// ItemGameplayTags.h
#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NativeGameplayTags.h"

/*
	HUD 알림 전용 네이티브 게임플레이 태그
 */
namespace NotificationTags
{
#pragma region Crafting
	//========================================
	// 크래프팅 알림
	//========================================
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Notification_Crafting_Failed);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Notification_Crafting_LackingIngredients);
#pragma endregion
#pragma region Building
	//========================================
	// 빌딩 알림
	//========================================
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Notification_Building_Failed);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Notification_Building_LackingIngredients);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Notification_Building_CannotPlace);
#pragma endregion
#pragma region Inventory
	//========================================
	// 인벤토리 알림
	//========================================
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Notification_Inventory_Failed);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Notification_Inventory_CannotPlace);
#pragma endregion
}
