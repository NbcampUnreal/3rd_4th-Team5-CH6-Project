// BuildingData.h
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "Item/Data/Common/ItemCommonEnums.h"
#include "BuildingData.generated.h"

#pragma region Enums
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
	T1  UMETA(DisplayName = "T1"),
	T2  UMETA(DisplayName = "T2"),
	T3  UMETA(DisplayName = "T3")
};
#pragma endregion

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
	
	// 생성자: FBuildingData 구조체 기본값 초기화
	FBuildingData()
		: BuildingID(0)                              // 건축물 고유 ID 초기화 (0 = 기본/미지정)
		, MainCategory(EItemMainCategory::BUILDING)  // 아이템 대분류 초기화 (건축물)
		, Name_KR(FText::GetEmpty())              // 한국어 이름 초기화 (빈 텍스트)
		, Name_EN(FText::GetEmpty())              // 영어 이름 초기화 (빈 텍스트)
		, Rarity(EItemRarity::NONE)                  // 등급 초기화 (NONE = 기본값)
		, BuildingType(EBuildingType::DEFENSE)       // 건축물 유형 초기화 (기본값: 방어용)
		, RequiredTier(ETier::T1)                    // 제작 요구 등급 초기화 (Tier 1 = 기본값)
		, MaxDurability(100)                         // 최대 내구도 초기화 (기본값: 100)
		, IsErosionController(false)                 // 침식 관리 기능 여부 초기화 (기본: 없음)
		, MaintenanceCostID(0)                       // 유지비 재료 ID 초기화 (0 = 없음)
		, MaintenanceInterval(0)                     // 유지비 소모 간격 초기화 (0초 = 없음)
		, MaintenanceCostQty(0)                      // 유지비 수량 초기화 (0 = 없음)
		, Icon(nullptr)                              // UI 아이콘 초기화 (nullptr = 없음)
		, WorldMesh(nullptr)                         // 월드 표시용 메시 초기화 (nullptr = 없음)
		, Desc_KR(FText::FromString(TEXT("")))       // 한국어 설명 텍스트 초기화 (빈 문자열)
		, Desc_EN(FText::FromString(TEXT("")))       // 영어 설명 텍스트 초기화 (빈 문자열)
		, ActorClass(nullptr)                   // 건축물 액터 클래스 초기화 (nullptr = 없음)
	{}

#pragma region Identifier
	// 건축물 고유 ID
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Identifier",
		meta=(DisplayName="Building ID", ToolTip="건축물 고유 ID"))
	int32 BuildingID;
	
	// 기획 비포함, 개발 편의상 추가: 아이템 대분류 (Item / Building / Resource)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Identifier",
		meta=(DisplayName="Main Category", ToolTip="아이템 대분류: Item / Building / Resource"))
	EItemMainCategory MainCategory;
	
	// 한국어 이름
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Identifier",
		meta = (DisplayName="Name_KR (한글명)", ToolTip="한국어 이름"))
	FText Name_KR;
	
	// 영어 이름
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Identifier",
		meta = (DisplayName="Name_EN (영문명)", ToolTip="영어 이름"))
	FText Name_EN;
#pragma endregion

#pragma region System
	// 등급
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="System",
		meta = (DisplayName="Rarity", ToolTip="등급"))
	EItemRarity Rarity;
	
	// 건축 타입
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="System",
		meta=(DisplayName="BuildingType", ToolTip="건축물 등급"))
	EBuildingType BuildingType;
	
	// 설치 구역 제한
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="System",
		meta=(DisplayName="PlacementArea", ToolTip="설치 가능 구역 태그 목록",
			Categories="Item.Building"))
	FGameplayTagContainer PlacementArea;
	
	// 제작 요구 등급
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="System",
		meta=(DisplayName="Required Tier", ToolTip="제작 요구 등급"))
	ETier RequiredTier;
	
	// 최대 내구도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="System",
		meta=(DisplayName="Max Durability", ToolTip="최대 내구도"))
	int32 MaxDurability;
	
	// 침식도 관리 여부
	// 광원 건축물만 True
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="System",
	meta=(DisplayName="Erosion Controller", ToolTip="침식도 관리 여부"))
	bool IsErosionController;
#pragma endregion
	
#pragma region LightSystem
	/*
		광원 건축물 전용 데이터
	*/
	// // 유지비 소모 주기 (초 단위, 0이면 소모 없음)
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="LIGHT-System", ClampMin="0.0"))
	// float MaintenancePeriod;
	
	// // 유지비 목록 (복수 고려)
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="LIGHT-System")
	// TArray<FResourceAmount> MaintenanceCost;
	
	// 광원 작동에 필요한 연료 아이템
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="LIGHT-System",
		meta=(EditCondition="BuildingType==EBuildingType::LIGHT", EditConditionHides,
			DisplayName="MaintenanceCostID (광원 작동에 필요한 연료 아이템)", ToolTip="광원 작동에 필요한 연료 아이템"))
	int32 MaintenanceCostID;
	
	// 초당 연료 소모량 (단위: 1.0 = 1개 소모)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="LIGHT-System",
		meta=(EditCondition="BuildingType==EBuildingType::LIGHT", EditConditionHides, ClampMin=0,
			DisplayName="MaintenanceInterval (초당 연료 소모량)", ToolTip="초당 연료 소모량 (단위: 1.0 = 1개 소모)"))
	float MaintenanceInterval;
	
	// 광원 건축물에 한 번에 보관 가능한 최대 연료량
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="LIGHT-System",
		meta=(EditCondition="BuildingType==EBuildingType::LIGHT", EditConditionHides, ClampMin=0,
			DisplayName="MaintenanceCostQty (광원 건축물에 한 번에 보관 가능한 최대 연료량)",
			ToolTip="광원 건축물에 한 번에 보관 가능한 최대 연료량"))
	int32 MaintenanceCostQty;
	
	// 광원이 생성하는 빛 구역의 유효 반경 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="LIGHT-System",
		meta=(EditCondition="BuildingType==EBuildingType::LIGHT", EditConditionHides,
			DisplayName="LightRadius_Units (광원이 생성하는 빛 구역의 유효 반경)",
			ToolTip="광원이 생성하는 빛 구역의 유효 반경 (엔진 유닛)"))
	float LightRadius_Units;
	
	// 침식도 감소량
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="LIGHT-System",
		meta=(EditCondition="BuildingType==EBuildingType::LIGHT", EditConditionHides,
			DisplayName="ErosionReduction (침식도 감소량)",
			ToolTip="침식도 감소량, 조명을 설치 시, 즉시 감소"))
	float ErosionReduction;
	
	// 정신력 회복
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="LIGHT-System",
		meta=(EditCondition="BuildingType==EBuildingType::LIGHT", EditConditionHides,
			DisplayName="SanityRecoveryPerSec (정신력 회복)",
			ToolTip="정신력 회복, 빛 영역(LightRadius_Units) 범위 내 있는 플레이어이 초당 회복하는 정신력 수치"))
	float SanityRecoveryPerSec;
	
	// 최대 동작 시간
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="LIGHT-System",
		meta=(EditCondition="BuildingType==EBuildingType::LIGHT", EditConditionHides,
			DisplayName="MaxMaintenance (최대 동작 시간)",
			ToolTip="최대 동작 시간, 연료를 최대치까지 넣은 경우, 가능한 최대 동작 시간"))
	int32 MaxMaintenance;
#pragma endregion
	
#pragma region StorageSystem
	/*
		창고 건축물 전용 데이터
	*/
	
	// 창고 인벤토리 슬롯 용량
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="STORAGE-System",
		meta=(EditCondition="BuildingType==EBuildingType::STORAGE", EditConditionHides,
			DisplayName="StorageSlots",
			ToolTip="창고 인벤토리 슬롯 용량"))
	int32 StorageSlots;
#pragma endregion
	
#pragma region Visual
	// 아이콘
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Visual",
		meta=(DisplayName="Icon", ToolTip="건축물 아이콘"))
	TSoftObjectPtr<UTexture2D> Icon;
	
	// 월드 메시
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Visual",
		meta=(DisplayName="World Mesh (3D 메시)", ToolTip="건축물 월드 메시"))
	TSoftObjectPtr<UStaticMesh> WorldMesh;
	
	// 한국어 설명
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Visual",
		meta = (DisplayName="Description_KR (한글 설명)", ToolTip="한국어 설명 텍스트", MultiLine=true))
	FText Desc_KR;
	
	// 영어 설명
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Visual",
		meta = (DisplayName="Description_EN (영문 설명)", ToolTip="영어 설명 텍스트", MultiLine=true))
	FText Desc_EN;
#pragma endregion

#pragma region Spawn
	// 기획 비포함, 개발 편의상 추가: 액터 클래스 (스폰용)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawn",
		meta=(DisplayName="Actor Class", ToolTip="건축물 액터 클래스"))
	TSubclassOf<AActor> ActorClass;
#pragma endregion
	
#pragma region Debug
	/*
		디버그용 상세 정보 로그 출력
	*/
	void PrintDebugInfo() const;
#pragma endregion
};

