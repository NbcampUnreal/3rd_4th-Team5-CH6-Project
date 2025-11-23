#pragma once

#include "CoreMinimal.h"
#include "TSReousrceControlSystemData.generated.h"

/*
 * 자원 관리 생성 시스템 데이터 테이블
 * 역할 : 각 섹터 내 어떠한 자원 생성 포인트가 존재 가능하며, 어떤 자원들이 생성 가능한지 정의.
 * 주의 : 자원 생성 포인트는 레벨 내에 직접 배치 함. 
 */

// 자원 관리 생성 시스템 데이터 테이블 구조체
USTRUCT(BlueprintType)
struct FResourceControlData : public FTableRowBase
{
	GENERATED_BODY()
	
	// 섹터 고유 태그 (모든 섹터는 고유한 태그를 가져야 한다.)
	
	// 각 섹터가 가질 수 있는 생성 포인트들. (여기에 빠진 경우 포인트를 가져가지 않음.)
	
	// 각 섹터가 가질 수 있는 자원 원천들.
};