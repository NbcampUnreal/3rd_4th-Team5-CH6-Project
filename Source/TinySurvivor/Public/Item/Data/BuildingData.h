// BuildingData.h
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "BuildingData.generated.h"

/*
	건축물 기능 분류
*/
UENUM(BlueprintType)
enum class EBuildingType : uint8
{
	DEFENSE    UMETA(DisplayName = "Defense"),
	CRAFTING   UMETA(DisplayName = "Crafting"),
	LIGHT      UMETA(DisplayName = "Light"),
	STORAGE    UMETA(DisplayName = "Storage")
};

/*
	건축물 제작 요구 Tier
*/
UENUM(BlueprintType)
enum class ETier : uint8
{
	T1  UMETA(DisplayName = "Tier 1"),
	T2  UMETA(DisplayName = "Tier 2"),
	T3  UMETA(DisplayName = "Tier 3")
};

/*
	자원 소모량 구조체
*/
// USTRUCT(BlueprintType)
// struct FResourceAmount
// {
// 	GENERATED_BODY()
//
// 	FResourceAmount()
// 	: ResourceID(0)
// 	, Amount(0)
// 	{}
//
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite)
// 	int32 ResourceID; // 자원 타입 ID
//
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite)
// 	int32 Amount;     // 필요한 양
// };

/*
	건축물 데이터 구조체
	DataTable 기반으로 관리, 각 건축물의 설정 값 저장
*/
USTRUCT(BlueprintType)
struct FBuildingData : public FTableRowBase
{
	GENERATED_BODY()
	
	FBuildingData()
	: BuildingID(0)                             // 기본 건축물 ID: 0
	, Name_KR(FText::GetEmpty())                // 기본 한글 이름: 빈 텍스트
	, Name_EN(FText::GetEmpty())                // 기본 영어 이름: 빈 텍스트
	, BuildingType(EBuildingType::DEFENSE)     // 기본 건축 타입: 방어
	, RequiredTier(ETier::T1)                  // 기본 제작 요구 등급: Tier 1
	, MaxDurability(100)                       // 기본 내구도: 100
	, IsErosionController(false)               // 기본: 침식도 관리 기능 없음
	, MaintenanceCostID(0)                     // 기본 유지비 재료 ID: 0 (없음)
	, MaintenanceInterval(0)                   // 기본 유지비 소모 간격: 0초
	, MaintenanceCostQty(0)                    // 기본 유지비 수량: 0
	, Icon(nullptr)                            // 기본 아이콘: 없음
	, WorldMesh(nullptr)                       // 기본 월드 메시: 없음
	, ActorClass(nullptr)                       // 기본 액터 클래스: 없음
	{}
	
	// 건축물 고유 ID
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Building-Identifier")
	int32 BuildingID;
	
	// 건축물 이름
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Building-Identifier")
	FText Name_KR;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Building-Identifier")
	FText Name_EN;

	// 건축 타입
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Building-System")
	EBuildingType BuildingType;

	// 설치 구역 제한 (태그 기반)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Building-System",
		meta=(Categories="Item.Building.Disallowed"))
	FGameplayTagContainer DisallowedZones;

	// 제작 요구 등급
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Building-System")
	ETier RequiredTier;

	// 최대 내구도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Building-System")
	int32 MaxDurability;

	// 침식도 관리 여부
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Building-System")
	bool IsErosionController;

	// // 유지비 소모 주기 (초 단위, 0이면 소모 없음)
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Building-System", ClampMin="0.0"))
	// float MaintenancePeriod;

	// // 유지비 목록 (복수 고려)
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Building-System")
	// TArray<FResourceAmount> MaintenanceCost;
	
	// 유지비 재료 ID (침식도 관리 기능과 연동, 단일 ID)
	// TODO: 기획서에서는 STRING으로 명시되어 있으나, 기존 시스템 및 데이터에서는 자원 ID를 int로 처리하고 있음.
	// 현재는 INT로 작성했지만, 기획과 일치 여부 확인 필요.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Building-System")
	int32 MaintenanceCostID;
	
	// 유지비 소모 간격 (초 단위)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Building-System", meta=(ClampMin=0))
	int32 MaintenanceInterval;
	
	// 유지비 재료 수량
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Building-System", meta=(ClampMin=0))
	int32 MaintenanceCostQty;
	
	//========================================
	// 기획 비포함, 개발 편의상 추가
	//========================================
	// 아이콘
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Building-Visual")
	TSoftObjectPtr<UTexture2D> Icon;

	// 월드 메시
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Building-Visual")
	TSoftObjectPtr<UStaticMesh> WorldMesh;

	// 액터 클래스 (스폰용)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Building-Spawn")
	TSubclassOf<AActor> ActorClass;
};
