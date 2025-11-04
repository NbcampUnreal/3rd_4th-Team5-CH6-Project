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
USTRUCT(BlueprintType)
struct FResourceAmount
{
	GENERATED_BODY()

	FResourceAmount()
	: ResourceID(0)
	, Amount(0)
	{}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ResourceID; // 자원 타입 ID

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Amount;     // 필요한 양
};

/*
	건축물 데이터 구조체
	DataTable 기반으로 관리, 각 건축물의 설정 값 저장
*/
USTRUCT(BlueprintType)
struct FBuildingData : public FTableRowBase
{
	GENERATED_BODY()

	FBuildingData()
	: BuildingID(0)
	, BuildingType(EBuildingType::DEFENSE)
	, RequiredTier(ETier::T1)
	, MaxDurability(100)
	, IsErosionController(false)
	, MaintenancePeriod(0.0f)
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

	// 유지비 소모 주기 (초 단위, 0이면 소모 없음)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Building-System",
		meta=(EditCondition="IsErosionController", ClampMin="0.0"))
	float MaintenancePeriod;

	// 유지비 목록
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Building-System",
		meta=(EditCondition="IsErosionController"))
	TArray<FResourceAmount> MaintenanceCost;
	
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
