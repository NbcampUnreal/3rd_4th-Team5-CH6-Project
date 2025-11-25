// ResourceData.h
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Item/Data/Common/ItemCommonEnums.h"
#include "ResourceData.generated.h"

#pragma region Enums
/*
	자원 원천 타입을 나타내는 Enum
*/
UENUM(BlueprintType)
enum class ENodeType : uint8
{
	NONE			 UMETA(DisplayName = "없음", ToolTip = "아무것도 설정하지 않을 때 Junk 선택 방지"),
	JUNK           	 UMETA(DisplayName = "Junk", ToolTip = "잡동사니 자원"),
	GLASS            UMETA(DisplayName = "Glass", ToolTip = "유리 자원"),
	PLANT            UMETA(DisplayName = "Plant", ToolTip = "식물 자원"),
	INTERACT         UMETA(DisplayName = "Interact", ToolTip = "상호작용 가능한 오브젝트"),
	WOOD             UMETA(DisplayName = "Wood", ToolTip = "나무 자원"),
	PLASTIC          UMETA(DisplayName = "Plastic", ToolTip = "플라스틱 자원"),
	DRINK            UMETA(DisplayName = "Drink", ToolTip = "음료 자원"),
	FOOD             UMETA(DisplayName = "Food", ToolTip = "식량 자원"),
	CLOTH            UMETA(DisplayName = "Cloth", ToolTip = "천 자원"),
	MINERAL          UMETA(DisplayName = "Mineral", ToolTip = "광물 자원"),
	MONSTER_CORPSE   UMETA(DisplayName = "Monster Corpse", ToolTip = "몬스터 시체 자원")
};

/*
	티어를 나타내는 Enum
*/
UENUM(BlueprintType)
enum class ENodeTier : uint8
{
	T1  UMETA(DisplayName = "T1", ToolTip = "T1: 주방"),
	T2  UMETA(DisplayName = "T2", ToolTip = "T2: 거실")
};
#pragma endregion

/*
	자원 원천 구조체
	DataTable 기반으로 관리, 각 자원 원천의 설정 값 저장
*/
USTRUCT(BlueprintType)
struct FResourceData : public FTableRowBase
{
	GENERATED_BODY()

public:
	// 생성자: FResourceData 구조체 기본값 초기화
	FResourceData()
		: ResourceID(0)                                   // 자원 고유 ID 초기화 (0 = 기본/미지정)
		, MainCategory(EItemMainCategory::RESOURCE)       // 아이템 대분류 초기화 (자원)
		, Name_KR(FText::FromString(TEXT("")))            // 한국어 이름 초기화 (빈 문자열)
		, Name_EN(FText::FromString(TEXT("")))            // 영어 이름 초기화 (빈 문자열)
		, Rarity(EItemRarity::NONE)                       // 등급 초기화 (기본값: NONE)
		, NodeType(ENodeType::JUNK)                       // 노드 타입 초기화 (기본값: JUNK)
		, RequiredToolID(0)                               // 자원 채집에 필요한 도구 ID 초기화 (0 = 없음)
		, NodeTier(ENodeTier::T1)                         // 노드 등급 초기화 (기본값: T1)
		, TotalYield(1)                                   // 총 채집 가능량 초기화 (기본값: 1)
		, DropTableID(0)                                  // 드롭 테이블 ID 초기화 (0 = 없음)
		, RespawnTime(0.0f)                               // 자원 재생 시간 초기화 (0.0 = 즉시)
		, WorldMesh(nullptr)                              // 월드 표시용 메시 초기화 (nullptr)
		, ActorClass(nullptr)                        // 스폰될 액터 클래스 초기화 (nullptr)
	{}

#pragma region Identifier
	// 자원 원천 고유 ID
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Identifier",
		meta=(DisplayName="Resource ID", ToolTip="자원 원천 고유 ID"))
	int32 ResourceID;
	
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
	
	// 자원 원천 타입
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="System",
		meta=(DisplayName="Node Type", ToolTip="자원 원천 타입"))
	ENodeType NodeType;
	
	// 필수 도구 ID (0이면 도구 불필요)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="System",
		meta=(DisplayName="Required Tool ID", ToolTip="필수 도구 ID (0이면 도구 불필요)"))
	int32 RequiredToolID;
	
	// 티어
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="System",
		meta=(DisplayName="Node Tier", ToolTip="자원 원천의 티어"))
	ENodeTier NodeTier;
	
	// 총 수량
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="System",
		meta=(DisplayName="Total Yield", ToolTip="자원 원천에서 얻을 수 있는 총 수량", ClampMin="1"))
	int32 TotalYield;
	
	// 원천을 수확했을 때 드랍되는 재료 아이템 목록 및 확률 테이블 ID
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="System",
		meta=(DisplayName="Drop Table ID", ToolTip="원천을 수확했을 때 드랍되는 재료 아이템 목록 및 확률 테이블 ID"))
	int32 DropTableID;
	
	// 원천이 다시 사용 가능한 상태로 재생되는 시간 (초 단위)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="System",
		meta=(DisplayName="Respawn Time", ToolTip="원천이 다시 사용 가능한 상태로 재생되는 시간 (초 단위)", ClampMin="0.0"))
	float RespawnTime;
#pragma endregion
	
#pragma region Visual
	// 월드 메시
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Visual",
		meta=(DisplayName="World Mesh (3D 메시)", ToolTip="월드에 배치되는 메시"))
	TSoftObjectPtr<UStaticMesh> WorldMesh;
#pragma endregion

#pragma region Spawn
	// 기획 비포함, 개발 편의상 추가: 액터 클래스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawn",
		meta=(DisplayName="Actor Class", ToolTip="스폰될 액터 클래스"))
	TSubclassOf<AActor> ActorClass;
#pragma endregion

#pragma region Debug
	/*
		디버그용 상세 정보 로그 출력
	*/
	void PrintDebugInfo() const;
#pragma endregion
};
