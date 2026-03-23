// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "TSPlayRole.generated.h"


// 게임 플레이에서 어떤 역할을 맡고 있는지 알려주는 enum
UENUM(BlueprintType)
enum class ETSPlayRole : uint8
{
	None		UMETA(DisplayName = "None"),
	
	Player		UMETA(DisplayName = "Player"),
	
	Giant		UMETA(DisplayName = "Giant"),
	Monster		UMETA(DisplayName = "Monster"),

	Resource	UMETA(DisplayName = "Resource"),
	Item		UMETA(DisplayName = "Item"),
	
};
