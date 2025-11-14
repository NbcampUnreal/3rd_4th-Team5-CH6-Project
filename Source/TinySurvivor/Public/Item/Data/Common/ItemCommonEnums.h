// ItemCommonEnums.h
#pragma once

#include "CoreMinimal.h"
#include "ItemCommonEnums.generated.h"

/*
	아이템 대분류를 나타내는 Enum
*/
UENUM(BlueprintType)
enum class EItemMainCategory : uint8
{
	ITEM      UMETA(DisplayName = "Item", ToolTip = "아이템: 플레이어 인벤토리에 보관 및 운반 가능"),
	BUILDING  UMETA(DisplayName = "Building", ToolTip = "건축물: 재료를 소모하여 설치하는 구조물"),
	RESOURCE  UMETA(DisplayName = "Resource", ToolTip = "자원 원천: 재료 획득용 필드 환경 오브젝트")
};

/*
	아이템의 희귀도를 나타내는 Enum
*/
UENUM(BlueprintType)
enum class EItemRarity : uint8
{
	NONE    UMETA(DisplayName = "None", ToolTip="등급 없음"),
	COMMON  UMETA(DisplayName = "Common", ToolTip="일반"),
	NORMAL  UMETA(DisplayName = "Normal", ToolTip="보통"),
	RARE    UMETA(DisplayName = "Rare", ToolTip="희귀"),
	UNIQUE  UMETA(DisplayName = "Unique", ToolTip="유일/특별")
};

