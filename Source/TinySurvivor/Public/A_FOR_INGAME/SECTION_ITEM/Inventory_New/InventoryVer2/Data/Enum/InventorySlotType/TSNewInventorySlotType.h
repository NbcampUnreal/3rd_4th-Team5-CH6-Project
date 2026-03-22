// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "TSNewInventorySlotType.generated.h"

/**
 * 인벤토리의 슬롯이 어떤 타입인지 알려주는 enum
 */
UENUM(BlueprintType)
enum class ETSNewInventorySlotType : uint8
{
	None			UMETA(DisplayName = "None"),
	Common			UMETA(DisplayName = "Common"),
	
	// 장비 (가방은 가방 아이템 전용을 의미)
	Head			UMETA(DisplayName = "Head"),
	Chest			UMETA(DisplayName = "Chest"),
	LeftHand		UMETA(DisplayName = "LeftHand"),
	RightHand		UMETA(DisplayName = "RightHand"),
	Backpack		UMETA(DisplayName = "Backpack"),
	
	
};
