// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "TSNewInventoryCompType.generated.h"

/**
 *  인벤토리가 어떤 타입인지 알려주는 enum
 */
UENUM(BlueprintType)
enum class ETSNewInventoryCompType : uint8
{
	None		UMETA(DisplayName = "None"),
	
	// 플레이어
	PlayerHotKey	UMETA(DisplayName = "HotKey"),
	PlayerBackPack	UMETA(DisplayName = "BackPack"),
	PlayerEquipment	UMETA(DisplayName = "Equipment"),
	
	// AI
	AIBackPack		UMETA(DisplayName = "AIBackPack"),
	AIEquipment		UMETA(DisplayName = "AIEquipment"),
	
	// 보관함
	Storage		UMETA(DisplayName = "Storage"),
};
