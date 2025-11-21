// SpawnerData.h
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "SpawnerData.generated.h"

#pragma region Enums
/*
	스폰이 허용되는 맵 구역 태그
*/
UENUM(BlueprintType)
enum class EZoneTag : uint8
{
	NONE           UMETA(DisplayName = "None", ToolTip="미지정"),
	SAFETY_ZONE    UMETA(DisplayName = "Safety Zone", ToolTip="안전 지대"),
	DARK_ZONE      UMETA(DisplayName = "Dark Zone", ToolTip="위험 지대"),
	EROSION_ZONE   UMETA(DisplayName = "Erosion Zone", ToolTip="침식 지대")
};

/*
	리스폰 트리거 조건
*/
UENUM(BlueprintType)
enum class ERespawnConditionTag : uint8
{
	NONE           UMETA(DisplayName = "None", ToolTip="조건 없음"),
	PLAYER_AWAY    UMETA(DisplayName = "Player Away", ToolTip="플레이어가 멀리 떨어짐"),
	EROSION_RESET  UMETA(DisplayName = "Erosion Reset", ToolTip="침식도 리셋 시")
};
#pragma endregion

#pragma region FSpawnerData
/*
	스폰 데이터 테이블 구조체
*/
USTRUCT(BlueprintType)
struct FSpawnerData : public FTableRowBase
{
	GENERATED_BODY()

public:
	// 생성자: FSpawnerData 구조체 기본값 초기화
	FSpawnerData()
		: SpawnNodeID(0)
		, ResourceNodeID(0)
		, SpawnMarkerGroup(0)
		, AllowedZoneTags()
		, MinSpawnCount(1)
		, MaxSpawnCount(1)
		, SpawnProbability(1.0f)
		, RespawnErosionMax(5)
		, RespawnInterval(300.0f)
		, RespawnConditionTag(ERespawnConditionTag::NONE)
		, bVulnerableToGiantShuffle(false)
		, SpawnRadius(1000.0f)
		, MinDistanceBetweenNodes(100.0f)
		, bAlignToGround(true)
	{}

#pragma region BasicInfo
	//==================================
	// 기본 정보
	//==================================
	
	// 스폰 노드 고유 ID (Primary Key)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BasicInfo",
		meta=(ClampMin="1", ClampMax="9999",
			DisplayName="SpawnNodeID (스폰 노드 ID)", ToolTip="이 스폰 규칙의 고유 ID"))
	int32 SpawnNodeID;

	// 생성될 자원 원천 ID (Foreign Key)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BasicInfo",
		meta=(ClampMin="1", DisplayName="ResourceNodeID (자원 원천 ID)",
			ToolTip="이 규칙으로 생성될 자원 원천의 ID (DT_ResourceNode 참조)"))
	int32 ResourceNodeID;
#pragma endregion

#pragma region LocationControl
	//==================================
	// 위치 제어
	//==================================
	
	// 스폰 마커 그룹 ID
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="LocationControl",
		meta=(ClampMin="0", DisplayName="SpawnMarkerGroup (스폰 마커 그룹)",
			ToolTip="LD가 지정한 고정된 스폰 마커 묶음 ID (침식도 2 이상 시 이 그룹 내에서 위치 변경)"))
	int32 SpawnMarkerGroup;

	// 허용된 구역 태그 목록
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="LocationControl",
		meta=(DisplayName="AllowedZoneTags (허용 구역 태그)", ToolTip="스폰이 허용되는 맵 구역 태그 목록"))
	TArray<EZoneTag> AllowedZoneTags;
#pragma endregion

#pragma region SpawnQuantity
	//==================================
	// 수량 설정
	//==================================
	
	// 최소 스폰 개수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SpawnQuantity",
		meta=(ClampMin="1", DisplayName="MinSpawnCount (최소 스폰 개수)", 
			ToolTip="해당 그룹에서 최소 생성될 노드 개수"))
	int32 MinSpawnCount;

	// 최대 스폰 개수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SpawnQuantity",
		meta=(ClampMin="1", DisplayName="MaxSpawnCount (최대 스폰 개수)", 
			ToolTip="해당 그룹에서 최대 생성될 노드 개수"))
	int32 MaxSpawnCount;

	// 스폰 확률 (0.0 ~ 1.0)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SpawnQuantity",
		meta=(ClampMin="0.0", ClampMax="1.0", DisplayName="SpawnProbability (스폰 확률)", 
			ToolTip="스폰 개수가 결정될 확률 (0.0 ~ 1.0)"))
	float SpawnProbability;
#pragma endregion

#pragma region RespawnLogic
	//==================================
	// 리스폰 로직
	//==================================
	
	// 리스폰 허용 최대 침식도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="RespawnLogic",
		meta=(ClampMin="0", DisplayName="RespawnErosionMax (리스폰 최대 침식도)", 
			ToolTip="정상적인 리스폰이 허용되는 최대 침식도 레벨. 이 값 초과 시 리스폰 중단"))
	int32 RespawnErosionMax;

	// 리스폰 간격 (초)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="RespawnLogic",
		meta=(ClampMin="0.0", DisplayName="RespawnInterval (리스폰 간격)", 
			ToolTip="RespawnErosionMax 미만일 때 적용되는 기본 리스폰 간격 (초)"))
	float RespawnInterval;

	// 리스폰 조건 태그
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="RespawnLogic",
		meta=(DisplayName="RespawnConditionTag (리스폰 조건)", 
			ToolTip="리스폰을 트리거하는 추가 조건"))
	ERespawnConditionTag RespawnConditionTag;
#pragma endregion

#pragma region LocationShuffleCondition
	//==================================
	// 위치 변경 조건
	//==================================
	
	// 거인 AI 위치 변경 대상 여부
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="LocationShuffleCondition",
		meta=(DisplayName="VulnerableToGiantShuffle (거인 셔플 대상)", 
			ToolTip="이 자원 그룹이 거인 AI의 위치 변경 이벤트 대상이 될 수 있는지 여부"))
	bool bVulnerableToGiantShuffle;
#pragma endregion

#pragma region WorldPlacement
	//==================================
	// 월드 배치
	//==================================
	
	// 스폰 반경
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WorldPlacement",
		meta=(ClampMin="0.0", DisplayName="SpawnRadius (스폰 반경)", 
			ToolTip="스폰 마커를 중심으로 노드가 퍼지는 반경"))
	float SpawnRadius;

	// 노드 간 최소 거리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WorldPlacement",
		meta=(ClampMin="0.0", DisplayName="MinDistanceBetweenNodes (노드 간 최소 거리)", 
			ToolTip="생성된 자원 노드 간의 최소 간격"))
	float MinDistanceBetweenNodes;

	// 지형 정렬 여부
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WorldPlacement",
		meta=(DisplayName="AlignToGround (지형 정렬)", 
			ToolTip="오브젝트를 지형 기울기에 맞춰 정렬할지 여부"))
	bool bAlignToGround;
#pragma endregion

#pragma region HelperFunctions
	//========================================
	// 헬퍼 함수
	//========================================

public:
	/*
		특정 구역 태그가 허용되는지 확인
		@param ZoneTag 확인할 구역 태그
		@return 허용되면 true
	*/
	bool IsZoneAllowed(EZoneTag ZoneTag) const
	{
		return AllowedZoneTags.Contains(ZoneTag);
	}

	/*
		스폰 개수 범위가 유효한지 확인
		@return 최소값이 최대값보다 작거나 같으면 true
	*/
	bool HasValidSpawnCountRange() const
	{
		return MinSpawnCount <= MaxSpawnCount && MinSpawnCount > 0;
	}

	/*
		랜덤 스폰 개수 계산
		@return MinSpawnCount와 MaxSpawnCount 사이의 랜덤 값
	*/
	int32 GetRandomSpawnCount() const
	{
		if (!HasValidSpawnCountRange())
		{
			return MinSpawnCount;
		}
		return FMath::RandRange(MinSpawnCount, MaxSpawnCount);
	}

	/*
		확률 기반 스폰 가능 여부 확인
		@return SpawnProbability에 따라 스폰 가능하면 true
	*/
	bool ShouldSpawnByProbability() const
	{
		return FMath::FRand() <= SpawnProbability;
	}

	/*
		현재 침식도에서 리스폰 가능한지 확인
		@param CurrentErosionLevel 현재 침식도
		@return 리스폰 가능하면 true
	*/
	bool CanRespawnAtErosionLevel(int32 CurrentErosionLevel) const
	{
		return CurrentErosionLevel <= RespawnErosionMax;
	}

	/*
		거인 AI 셔플 대상인지 확인
		@return 셔플 대상이면 true
	*/
	bool IsVulnerableToShuffle() const
	{
		return bVulnerableToGiantShuffle;
	}

	/*
		지형 정렬이 필요한지 확인
		@return 지형 정렬이 필요하면 true
	*/
	bool ShouldAlignToGround() const
	{
		return bAlignToGround;
	}
#pragma endregion

#pragma region Validation
	/*
		스폰 데이터 유효성 검증
		@return 스폰 데이터가 올바르게 설정되어 있으면 true
	*/
	bool ValidateSpawnerData() const
	{
		// 기본 ID 검증
		if (SpawnNodeID <= 0 || ResourceNodeID <= 0)
		{
			return false;
		}

		// 스폰 개수 범위 검증
		if (!HasValidSpawnCountRange())
		{
			return false;
		}

		// 확률 범위 검증
		if (SpawnProbability < 0.0f || SpawnProbability > 1.0f)
		{
			return false;
		}

		// 리스폰 간격 검증
		if (RespawnInterval < 0.0f)
		{
			return false;
		}

		// 침식도 최대값 검증
		if (RespawnErosionMax < 0)
		{
			return false;
		}

		// 스폰 반경 검증
		if (SpawnRadius <= 0.0f)
		{
			return false;
		}

		// 노드 간 거리 검증
		if (MinDistanceBetweenNodes < 0.0f)
		{
			return false;
		}

		// 구역 태그 검증 (최소 1개 이상 필요)
		if (AllowedZoneTags.Num() == 0)
		{
			return false;
		}

		return true;
	}

	/*
		스폰 가능 여부 종합 검증
		@return 데이터 유효성 + 확률 검증 통과 시 true
	*/
	bool IsSpawnable() const
	{
		return ValidateSpawnerData() && ShouldSpawnByProbability();
	}
#pragma endregion

#pragma region Debug
	/*
		디버그용 상세 정보 로그 출력
	*/
	void PrintDebugInfo() const;
#pragma endregion
};
#pragma endregion
