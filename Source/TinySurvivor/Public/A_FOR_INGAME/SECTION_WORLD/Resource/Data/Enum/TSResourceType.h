// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"

/**
 * 자원의 타입 -> 자원이 어떤 카테고리인지 의미한다. 
 * 이 얼거형은 자원 스폰 시스템이 자원을 스폰할 때 범용 노드 소환 판단에도 쓰인다.
 * 자원의 타입은 반드시 하나만 가져야 하며, 절대 중복되게 가질 수 없다.
 */

UENUM(BlueprintType)
enum class ETSResourceType : uint8
{
	None		UMETA(DisplayName = "None"),
};