#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Item/Data/ResourceData.h"
#include "TSReousrceControlSystemDataTable.generated.h"

/*
 * 자원 관리 생성 시스템 데이터 테이블
 * 역할 : 각 섹터 내 어떠한 자원 생성 포인트가 존재 가능하며, 어떤 자원들이 생성 가능한지 정의.
 * 주의 : 자원 생성 포인트는 레벨 내에 직접 배치 함. 
 */

// 자원 관리 생성 시스템 데이터 테이블 구조체 (각 섹터마다 다르게)
USTRUCT(BlueprintType)
struct FResourceControlData : public FTableRowBase
{
	GENERATED_BODY()
	
	// 섹터 고유 태그 (모든 섹터는 고유한 태그를 가져야 한다.)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "섹터 태그명",  meta=(Categories= "ResourceSector"))
	FGameplayTag ResourceSectorTag = FGameplayTag::EmptyTag;
	
	// 자원 원천 ID 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "생성할 자원 원천 ID")
	int32 ResourceID = 0;
	
	// 샏성할 원천 갯수
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "샏성할 원천 갯수")
	int32 ResourceSpawnNum = 0;
	
	// 노드 타입이 볌용인가?
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "노드 타입이 볌용인가?")
	bool bIsCommonNode = true;
	
	// 노드 타입
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "노드 타입 (범용)", meta=(EditCondition="bIsCommonNode"))
	ENodeType NodeType = ENodeType::NONE;
	
	// 유니크 타입
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "노드 타입 (유니크)", meta=(EditCondition="!bIsCommonNode", Categories= "ResourceUniqueTags"))
	FGameplayTag ResourceUniqueTag = FGameplayTag::EmptyTag;
};