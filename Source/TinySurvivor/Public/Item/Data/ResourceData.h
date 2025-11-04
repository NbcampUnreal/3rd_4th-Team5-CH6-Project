// ResourceData.h
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ResourceData.generated.h"

// 자원 원천 타입
UENUM(BlueprintType)
enum class ENodeType : uint8
{
	PLANT           UMETA(DisplayName = "Plant"),
	MINERAL         UMETA(DisplayName = "Mineral"),
	JUNK            UMETA(DisplayName = "Junk"),
	MONSTER_CORPSE  UMETA(DisplayName = "Monster Corpse")
};

// 원천 난이도 티어
UENUM(BlueprintType)
enum class ENodeTier : uint8
{
	T1  UMETA(DisplayName = "T1 - Kitchen"),
	T2  UMETA(DisplayName = "T2 - Living Room"),
	T3  UMETA(DisplayName = "T3 - Outdoor")
};

/*
	자원 원천 구조체
	DataTable 기반으로 관리, 각 자원 원천의 설정 값 저장
*/
USTRUCT(BlueprintType)
struct FResourceData : public FTableRowBase
{
	GENERATED_BODY()

public:
	FResourceData()
		: ResourceID(0)
		, NodeType(ENodeType::JUNK)
		, RequiredToolID(0)
		, NodeTier(ENodeTier::T1)
		, TotalYield(1)
		, RespawnTime(0.0f)
		, DropTableID(0)
		//, HarvestTime(1.0f)
		//, NoiseValue(0.0f)
	{}

	// 자원 원천 고유 ID
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Resource-Identifier")
	int32 ResourceID;

	// 자원 원천 이름
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Resource-Identifier")
	FText Name_KR;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Resource-Identifier")
	FText Name_EN;

	// 자원 원천 타입
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Resource-System")
	ENodeType NodeType;

	// 필수 도구 ID (0이면 도구 불필요)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Resource-System")
	int32 RequiredToolID;

	// 난이도 티어
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Resource-System")
	ENodeTier NodeTier;

	// 총 수량
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Resource-System",
		meta=(ClampMin="1"))
	int32 TotalYield;

	// 원천이 다시 사용 가능한 상태로 재생되는 시간 (초 단위)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Resource-System",
		meta=(ClampMin="0.0"))
	float RespawnTime;

	// 원천을 수확했을 때 드랍되는 재료 아이템 목록 및 확률 테이블 ID
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Resource-System")
	int32 DropTableID;

	// // 수확 시 발생하는 소음 수치 (거인 및 몬스터 AI 인지 범위에 영향)
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Resource-System",
	// 	meta=(ClampMin="0.0"))
	// float NoiseValue;
	
	// 월드 메시
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Resource-Visual")
	TSoftObjectPtr<UStaticMesh> WorldMesh;

	// 액터 클래스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Resource-Spawn")
	TSubclassOf<AActor> ActorClass;
};