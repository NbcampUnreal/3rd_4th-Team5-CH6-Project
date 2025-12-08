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

/*
	아이템 애니메이션 타입을 나타내는 Enum
	캐릭터 애니메이션 시스템에서 아이템별 애니메이션 선택에 사용
*/
UENUM(BlueprintType)
enum class EItemAnimType : uint8
{
	NONE          UMETA(DisplayName = "None", ToolTip="애니메이션 없음"),
	PICK          UMETA(DisplayName = "Pick", ToolTip="곡괭이 타입 도구"),
	AXE           UMETA(DisplayName = "Axe", ToolTip="도끼 타입 도구"),
	HAMMER        UMETA(DisplayName = "Hammer", ToolTip="망치 타입 도구"),
	WEAPON_SPEAR  UMETA(DisplayName = "Weapon Spear", ToolTip="창 타입 무기"),
	WEAPON_MELEE  UMETA(DisplayName = "Weapon Melee", ToolTip="근거리 전투용 무기 타입")
};

