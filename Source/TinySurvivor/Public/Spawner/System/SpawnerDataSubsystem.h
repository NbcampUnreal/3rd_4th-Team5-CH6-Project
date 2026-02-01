// SpawnerDataSubsystem.h
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Spawner/Data/SpawnerData.h"
#include "Spawner/Data/SpawnerTableAsset.h"
#include "SpawnerDataSubsystem.generated.h"

/*
	스폰 데이터를 관리하고 캐싱하는 GameInstance Subsystem
	
	주요 기능:
	- 데이터 테이블에서 스폰 데이터를 초기화하고 캐싱
	- Blueprint 및 코드에서 쉽게 접근 가능하도록 Getter 제공
	- 스폰 데이터 유효성 검사 기능 포함
	- 스폰 가능 여부 확인 기능 제공
	
	초기화 방식:
	- GameMode 또는 ProjectSettings에서 InitializeFromAsset() 호출
	- 리슨 서버 환경: 서버에서 초기화, 클라이언트도 동일 에셋 로드
	
	네트워크 고려사항:
	- 모든 Getter 함수는 const로 선언되어 로컬 캐시만 조회
	- 서버/클라이언트 구분 없이 안전하게 사용 가능
	- 복제(Replication) 불필요 - 각 머신이 동일한 데이터를 로컬에 캐싱
*/
UCLASS()
class TINYSURVIVOR_API USpawnerDataSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
#pragma region ConstructorAndLifecycle
public:
	USpawnerDataSubsystem();

	//========================================
	// Subsystem Lifecycle
	//========================================
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
#pragma endregion

#pragma region DataAssets
protected:
	//========================================
	// Data Assets
	//========================================
	// 데이터 테이블을 보유한 데이터 에셋 참조
	UPROPERTY()
	TObjectPtr<USpawnerTableAsset> TableAsset;
#pragma endregion

#pragma region CachedData
protected:
	//========================================
	// Cached Data
	//========================================
	
	// 캐싱된 스폰 데이터 (Key: SpawnNodeID)
	UPROPERTY()
	TMap<int32, FSpawnerData> SpawnerDataCache;

	// 초기화 완료 여부 플래그
	bool bIsInitialized;
#pragma endregion

#pragma region StaticAccessHelper
public:
	//========================================
	// Static Access Helper
	//========================================
	
	/*
		편의성 함수: GameInstance에서 SpawnerDataSubsystem 가져오기
		Blueprint와 C++ 모두에서 안전하게 접근 가능
		
		@param WorldContext 월드 컨텍스트 (Character, GameMode, Widget 등)
		@return SpawnerDataSubsystem 인스턴스 (nullptr 가능)
		
		사용 예시 (C++):
			if (USpawnerDataSubsystem* SpawnerDataSystem = USpawnerDataSubsystem::GetSpawnerDataSubsystem(this))
			{
				FSpawnerData SpawnData;
				if (SpawnerDataSystem->GetSpawnerDataSafe(SpawnNodeID, SpawnData))
				{
					// 스폰 데이터 사용
				}
			}
		
		사용 예시 (Blueprint):
			Get Spawner Data Subsystem → Branch (Is Valid) → Get Spawner Data Safe
	*/
	UFUNCTION(BlueprintPure, Category="SpawnerData", 
		meta=(WorldContext="WorldContext", DisplayName="Get Spawner Data Subsystem"))
	static USpawnerDataSubsystem* GetSpawnerDataSubsystem(const UObject* WorldContext);
#pragma endregion
	
#pragma region Initialization
public:
	//========================================
	// Initialization
	//========================================
	/*
		데이터 에셋으로부터 시스템 초기화
		GameInstance::Init()에서 호출되어야 함
		
		@param InTableAsset 스폰 데이터 테이블을 포함한 데이터 에셋
		@return 초기화 성공 여부
	*/
	UFUNCTION(BlueprintCallable, Category="SpawnerData")
	bool InitializeFromAsset(USpawnerTableAsset* InTableAsset);
	
	/*
		초기화 완료 여부 확인
		@return 초기화되었으면 true
	*/
	UFUNCTION(BlueprintPure, Category="SpawnerData")
	bool IsInitialized() const { return bIsInitialized; }
#pragma endregion

#pragma region SpawnerDataGetters
public:
	//========================================
	// Spawner Data Getters
	//========================================
	
	/*
		스폰 데이터 조회 (ID가 확실히 존재할 때만 사용)
		주의: 존재하지 않는 ID 조회 시 빈 데이터 반환
		안전한 조회가 필요하면 GetSpawnerDataSafe() 사용 권장
		
		@param SpawnNodeID 조회할 스폰 노드 ID
		@return 스폰 데이터 (존재하지 않으면 빈 데이터)
	*/
	UFUNCTION(BlueprintPure, Category="SpawnerData")
	const FSpawnerData& GetSpawnerData(int32 SpawnNodeID) const;
	
	/*
		스폰 데이터 조회 (안전 버전 - 권장)
		@param SpawnNodeID 조회할 스폰 노드 ID
		@param OutData 출력될 스폰 데이터
		@return 데이터가 존재하면 true, 없으면 false
	*/
	UFUNCTION(BlueprintPure, Category="SpawnerData")
	bool GetSpawnerDataSafe(int32 SpawnNodeID, FSpawnerData& OutData) const;
	
	/*
		모든 스폰 노드 ID 목록 조회
		@return 캐싱된 모든 스폰 노드 ID 배열
	*/
	UFUNCTION(BlueprintPure, Category="SpawnerData")
	TArray<int32> GetAllSpawnNodeIDs() const;
	
	/*
		특정 자원 노드를 생성하는 스폰 데이터 찾기
		@param ResourceNodeID 자원 노드 ID
		@return 해당 자원 노드를 생성하는 스폰 노드 ID 배열
	*/
	UFUNCTION(BlueprintPure, Category="SpawnerData")
	TArray<int32> GetSpawnNodeIDsByResourceNode(int32 ResourceNodeID) const;
	
	/*
		특정 스폰 마커 그룹에 속한 스폰 데이터 찾기
		@param MarkerGroupID 스폰 마커 그룹 ID
		@return 해당 그룹의 스폰 노드 ID 배열
	*/
	UFUNCTION(BlueprintPure, Category="SpawnerData")
	TArray<int32> GetSpawnNodeIDsByMarkerGroup(int32 MarkerGroupID) const;
	
	/*
		특정 구역 태그에서 스폰 가능한 스폰 데이터 찾기
		@param ZoneTag 구역 태그
		@return 해당 구역에서 스폰 가능한 스폰 노드 ID 배열
	*/
	UFUNCTION(BlueprintPure, Category="SpawnerData")
	TArray<int32> GetSpawnNodeIDsByZoneTag(EZoneTag ZoneTag) const;
	
	/*
		거인 셔플에 취약한 스폰 데이터 목록
		@return 거인 AI가 위치를 변경할 수 있는 스폰 노드 ID 배열
	*/
	UFUNCTION(BlueprintPure, Category="SpawnerData")
	TArray<int32> GetShuffleVulnerableSpawnNodeIDs() const;
	
	/*
		특정 침식도에서 리스폰 가능한 스폰 데이터 찾기
		@param CurrentErosionLevel 현재 침식도
		@return 해당 침식도에서 리스폰 가능한 스폰 노드 ID 배열
	*/
	UFUNCTION(BlueprintPure, Category="SpawnerData")
	TArray<int32> GetRespawnableSpawnNodeIDs(int32 CurrentErosionLevel) const;
	
	/*
		특정 리스폰 조건을 가진 스폰 데이터 찾기
		@param ConditionTag 리스폰 조건 태그
		@return 해당 조건의 스폰 노드 ID 배열
	*/
	UFUNCTION(BlueprintPure, Category="SpawnerData")
	TArray<int32> GetSpawnNodeIDsByRespawnCondition(ERespawnConditionTag ConditionTag) const;
#pragma endregion

#pragma region Validation
public:
	//========================================
	// Validation
	//========================================
	
	/*
		스폰 데이터 유효성 확인
		@param SpawnNodeID 확인할 스폰 노드 ID
		@return 유효한 데이터가 존재하면 true
	*/
	UFUNCTION(BlueprintPure, Category="SpawnerData")
	bool IsSpawnerDataValid(int32 SpawnNodeID) const;
	
	/*
		스폰 가능 여부 확인 (데이터 존재 + 유효성 + 확률)
		@param SpawnNodeID 확인할 스폰 노드 ID
		@return 스폰 가능하면 true
	*/
	UFUNCTION(BlueprintPure, Category="SpawnerData")
	bool IsSpawnable(int32 SpawnNodeID) const;
	
	/*
		현재 침식도에서 리스폰 가능한지 확인
		@param SpawnNodeID 확인할 스폰 노드 ID
		@param CurrentErosionLevel 현재 침식도
		@return 리스폰 가능하면 true
	*/
	UFUNCTION(BlueprintPure, Category="SpawnerData")
	bool CanRespawnAtErosionLevel(int32 SpawnNodeID, int32 CurrentErosionLevel) const;
#pragma endregion
	
#pragma region Debug
public:
	//========================================
	// Statistics & Debug
	//========================================
	
	/*
		캐싱된 데이터 통계 정보 조회
		@param OutSpawnNodeCount 스폰 노드 개수
	*/
	UFUNCTION(BlueprintPure, Category="SpawnerData")
	void GetCacheStatistics(int32& OutSpawnNodeCount) const;
	
	/*
		디버그용 캐시 정보 로그 출력
	*/
	UFUNCTION(BlueprintCallable, Category="SpawnerData|Debug")
	void PrintCacheDebugInfo() const;
	
	/*
		디버그용 스폰 노드 정보 로그 출력
	*/
	UFUNCTION(BlueprintCallable, Category="SpawnerData|Debug")
	void PrintSpawnNodeDebugInfo(int32 SpawnNodeID) const;
	
#if WITH_EDITOR
	/*
		에디터 전용: 캐시 리프레시
		데이터 테이블 변경 후 에디터에서 실시간 반영용
	*/
	UFUNCTION(CallInEditor, Category="SpawnerData|Editor")
	void RefreshCache();
	
	/*
		에디터 전용: 초기화 후 자동 테스트
		InitializeFromAsset() 완료 후 호출됨
	*/
	void RunInitializationTests();
#endif
#pragma endregion

#pragma region InternalCachingFunctions
private:
	//========================================
	// Internal Caching Functions
	//========================================
	/*
		스폰 데이터 테이블을 TMap으로 캐싱
	*/
	void CacheSpawnerData();

	/*
		모든 캐시 초기화
	*/
	void ClearAllCaches();

	/*
		데이터 에셋 유효성 검증
		@param InTableAsset 검증할 데이터 에셋
		@return 유효하면 true
	*/
	bool ValidateTableAsset(const USpawnerTableAsset* InTableAsset) const;
#pragma endregion
};
