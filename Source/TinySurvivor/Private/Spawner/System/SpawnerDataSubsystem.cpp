// SpawnerDataSubsystem.cpp
#include "Spawner/System/SpawnerDataSubsystem.h"
#include "Spawner/System/SpawnerSystemSettings.h"
#include "Engine/Engine.h"

// 로그 카테고리 정의
DEFINE_LOG_CATEGORY_STATIC(LogSpawnerDataSubsystem, Log, All);

//========================================
// Static Empty Data (빈 데이터 반환용)
//========================================
namespace
{
	static const FSpawnerData EmptySpawnerData;
}

//========================================
// Constructor
//========================================
USpawnerDataSubsystem::USpawnerDataSubsystem()
	: bIsInitialized(false)
{
}

#pragma region Lifecycle
//========================================
// Subsystem Lifecycle
//========================================
void USpawnerDataSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	UE_LOG(LogSpawnerDataSubsystem, Log, TEXT("SpawnerDataSubsystem Initialized"));
	
	// 네트워크 환경 확인 (디버그용)
	if (const UWorld* World = GetWorld())
	{
		const ENetMode NetMode = World->GetNetMode();
		UE_LOG(LogSpawnerDataSubsystem, Log, TEXT("  NetMode: %s"),
			NetMode == NM_Standalone ? TEXT("Standalone") :
			NetMode == NM_DedicatedServer ? TEXT("DedicatedServer") :
			NetMode == NM_ListenServer ? TEXT("ListenServer") :
			NetMode == NM_Client ? TEXT("Client") : TEXT("Unknown"));
	}
	
	// Project Settings에서 자동 로드
	const USpawnerSystemSettings* Settings = USpawnerSystemSettings::Get();
	if (!Settings)
	{
		UE_LOG(LogSpawnerDataSubsystem, Error, TEXT("SpawnerSystemSettings를 찾을 수 없습니다!"));
		return;
	}
	
	// Soft Reference 동기 로드
	USpawnerTableAsset* LoadedAsset = Settings->SpawnerTableAsset.LoadSynchronous();
	if (!LoadedAsset)
	{
		UE_LOG(LogSpawnerDataSubsystem, Error, TEXT("SpawnerTableAsset 로드 실패! Project Settings > Spawner System에서 설정하세요."));
		return;
	}
	
	// 자동 초기화
	if (InitializeFromAsset(LoadedAsset))
	{
		UE_LOG(LogSpawnerDataSubsystem, Log, TEXT("SpawnerDataSubsystem 자동 초기화 성공"));
	}
	else
	{
		UE_LOG(LogSpawnerDataSubsystem, Error, TEXT("SpawnerDataSubsystem 초기화 실패"));
	}
}

void USpawnerDataSubsystem::Deinitialize()
{
	UE_LOG(LogSpawnerDataSubsystem, Log, TEXT("SpawnerDataSubsystem Deinitialized"));
	
	ClearAllCaches();
	TableAsset = nullptr;
	bIsInitialized = false;
	
	Super::Deinitialize();
}
#pragma endregion

#pragma region StaticAccessHelper
//========================================
// Static Access Helper
//========================================
USpawnerDataSubsystem* USpawnerDataSubsystem::GetSpawnerDataSubsystem(const UObject* WorldContext)
{
	if (!WorldContext)
	{
		UE_LOG(LogSpawnerDataSubsystem, Warning, TEXT("GetSpawnerDataSubsystem: WorldContext가 null입니다"));
		return nullptr;
	}
	
	// WorldContext에서 UWorld 객체 가져오기
	const UWorld* World = GEngine->GetWorldFromContextObject(
		WorldContext, 
		EGetWorldErrorMode::LogAndReturnNull
	);
	
	if (!World)
	{
		UE_LOG(LogSpawnerDataSubsystem, Warning, TEXT("GetSpawnerDataSubsystem: WorldContext에서 UWorld를 가져오지 못했습니다"));
		return nullptr;
	}
	
	// World에서 GameInstance 가져오기
	UGameInstance* GameInstance = World->GetGameInstance();
	if (!GameInstance)
	{
		UE_LOG(LogSpawnerDataSubsystem, Warning, TEXT("GetSpawnerDataSubsystem: GameInstance가 null입니다"));
		return nullptr;
	}
	
	// GameInstance에서 SpawnerDataSubsystem 가져오기
	USpawnerDataSubsystem* Subsystem = GameInstance->GetSubsystem<USpawnerDataSubsystem>();
	if (!Subsystem)
	{
		UE_LOG(LogSpawnerDataSubsystem, Error, TEXT("GetSpawnerDataSubsystem: Subsystem을 찾을 수 없습니다!"));
		return nullptr;
	}
	
	// 서브시스템 초기화 확인
	if (!Subsystem->IsInitialized())
	{
		UE_LOG(LogSpawnerDataSubsystem, Warning, 
			TEXT("GetSpawnerDataSubsystem: Subsystem은 찾았지만 아직 초기화되지 않았습니다. 먼저 InitializeFromAsset()를 호출하세요."));
	}
	
	return Subsystem;
}
#pragma endregion

#pragma region Initialization
//========================================
// Initialization
//========================================
bool USpawnerDataSubsystem::InitializeFromAsset(USpawnerTableAsset* InTableAsset)
{
	if (!ValidateTableAsset(InTableAsset))
	{// 테이블 에셋 유효성 검사
		UE_LOG(LogSpawnerDataSubsystem, Error, TEXT("초기화 실패: 유효하지 않은 TableAsset"));
		return false;
	}
	
	// 기존 캐시 초기화
	ClearAllCaches();
	
	// 데이터 에셋 저장
	TableAsset = InTableAsset;
	
	// 스폰 데이터 캐싱
	CacheSpawnerData();
	
	// 초기화 완료 플래그 설정
	bIsInitialized = true;
	
#if UE_BUILD_DEVELOPMENT || UE_BUILD_DEBUG
	// 개발/디버그 빌드에서 캐시 상태 로그 출력
	UE_LOG(LogSpawnerDataSubsystem, Log, TEXT("SpawnerDataSubsystem 초기화 성공"));
	UE_LOG(LogSpawnerDataSubsystem, Log, TEXT("- SpawnNodes: %d"), SpawnerDataCache.Num());
	
	// 에디터에서만 자동 테스트 실행
#if WITH_EDITOR
	RunInitializationTests();
#endif
#endif
	
	return true;
}
#pragma endregion

#pragma region SpawnerDataGetters
//========================================
// Spawner Data Getters
//========================================
const FSpawnerData& USpawnerDataSubsystem::GetSpawnerData(int32 SpawnNodeID) const
{
	if (!bIsInitialized)
	{// 초기화 여부 확인
		UE_LOG(LogSpawnerDataSubsystem, Warning, TEXT("GetSpawnerData 호출 시점: 아직 초기화되지 않음"));
		return EmptySpawnerData;
	}
	
	if (const FSpawnerData* Found = SpawnerDataCache.Find(SpawnNodeID))
	{// 캐시에서 스폰 데이터 검색
		return *Found;
	}
	
	// 스폰 노드 ID를 캐시에서 찾지 못했을 경우 경고 로그 출력
	UE_LOG(LogSpawnerDataSubsystem, Warning, TEXT("SpawnNodeID %d를 캐시에서 찾을 수 없음"), SpawnNodeID);
	return EmptySpawnerData;
}

bool USpawnerDataSubsystem::GetSpawnerDataSafe(int32 SpawnNodeID, FSpawnerData& OutData) const
{
	if (!bIsInitialized)
	{// 초기화 여부 확인
		UE_LOG(LogSpawnerDataSubsystem, Warning, TEXT("GetSpawnerDataSafe 호출 시점: 아직 초기화되지 않음"));
		return false;
	}
	
	if (const FSpawnerData* Found = SpawnerDataCache.Find(SpawnNodeID))
	{// 캐시에서 스폰 데이터 검색
		OutData = *Found; // 안전하게 OutData에 복사
		return true;
	}
	
	// 스폰 노드 ID를 캐시에서 찾지 못했을 경우 false 반환
	return false;
}

TArray<int32> USpawnerDataSubsystem::GetAllSpawnNodeIDs() const
{
	// 캐시에 저장된 모든 스폰 노드 ID를 배열로 가져오기
	TArray<int32> SpawnNodeIDs;
	SpawnerDataCache.GetKeys(SpawnNodeIDs);
	return SpawnNodeIDs;
}

TArray<int32> USpawnerDataSubsystem::GetSpawnNodeIDsByResourceNode(int32 ResourceNodeID) const
{
	// 특정 자원 노드를 생성하는 스폰 데이터 필터링
	TArray<int32> FilteredIDs;
	
	for (const TPair<int32, FSpawnerData>& Pair : SpawnerDataCache)
	{
		if (Pair.Value.ResourceNodeID == ResourceNodeID)
		{
			FilteredIDs.Add(Pair.Key);
		}
	}
	
	return FilteredIDs;
}

TArray<int32> USpawnerDataSubsystem::GetSpawnNodeIDsByMarkerGroup(int32 MarkerGroupID) const
{
	// 특정 스폰 마커 그룹에 속한 스폰 데이터 필터링
	TArray<int32> FilteredIDs;
	
	for (const TPair<int32, FSpawnerData>& Pair : SpawnerDataCache)
	{
		if (Pair.Value.SpawnMarkerGroup == MarkerGroupID)
		{
			FilteredIDs.Add(Pair.Key);
		}
	}
	
	return FilteredIDs;
}

TArray<int32> USpawnerDataSubsystem::GetSpawnNodeIDsByZoneTag(EZoneTag ZoneTag) const
{
	// 특정 구역 태그에서 스폰 가능한 스폰 데이터 필터링
	TArray<int32> FilteredIDs;
	
	for (const TPair<int32, FSpawnerData>& Pair : SpawnerDataCache)
	{
		if (Pair.Value.IsZoneAllowed(ZoneTag))
		{
			FilteredIDs.Add(Pair.Key);
		}
	}
	
	return FilteredIDs;
}

TArray<int32> USpawnerDataSubsystem::GetShuffleVulnerableSpawnNodeIDs() const
{
	// 거인 셔플에 취약한 스폰 데이터 필터링
	TArray<int32> FilteredIDs;
	
	for (const TPair<int32, FSpawnerData>& Pair : SpawnerDataCache)
	{
		if (Pair.Value.IsVulnerableToShuffle())
		{
			FilteredIDs.Add(Pair.Key);
		}
	}
	
	return FilteredIDs;
}

TArray<int32> USpawnerDataSubsystem::GetRespawnableSpawnNodeIDs(int32 CurrentErosionLevel) const
{
	// 특정 침식도에서 리스폰 가능한 스폰 데이터 필터링
	TArray<int32> FilteredIDs;
	
	for (const TPair<int32, FSpawnerData>& Pair : SpawnerDataCache)
	{
		if (Pair.Value.CanRespawnAtErosionLevel(CurrentErosionLevel))
		{
			FilteredIDs.Add(Pair.Key);
		}
	}
	
	return FilteredIDs;
}

TArray<int32> USpawnerDataSubsystem::GetSpawnNodeIDsByRespawnCondition(ERespawnConditionTag ConditionTag) const
{
	// 특정 리스폰 조건을 가진 스폰 데이터 필터링
	TArray<int32> FilteredIDs;
	
	for (const TPair<int32, FSpawnerData>& Pair : SpawnerDataCache)
	{
		if (Pair.Value.RespawnConditionTag == ConditionTag)
		{
			FilteredIDs.Add(Pair.Key);
		}
	}
	
	return FilteredIDs;
}
#pragma endregion

#pragma region Validation
//========================================
// Validation
//========================================

// 특정 스폰 노드 ID가 유효한지 확인
bool USpawnerDataSubsystem::IsSpawnerDataValid(int32 SpawnNodeID) const
{
	// 초기화 여부 확인 후, 캐시에 해당 ID가 존재하는지 반환
	return bIsInitialized && SpawnerDataCache.Contains(SpawnNodeID);
}

// 스폰 가능 여부 확인 (데이터 존재 + 유효성 + 확률)
bool USpawnerDataSubsystem::IsSpawnable(int32 SpawnNodeID) const
{
	if (!bIsInitialized)
	{
		return false;
	}
	
	if (const FSpawnerData* Found = SpawnerDataCache.Find(SpawnNodeID))
	{
		return Found->IsSpawnable();
	}
	
	return false;
}

// 현재 침식도에서 리스폰 가능한지 확인
bool USpawnerDataSubsystem::CanRespawnAtErosionLevel(int32 SpawnNodeID, int32 CurrentErosionLevel) const
{
	if (!bIsInitialized)
	{
		return false;
	}
	
	if (const FSpawnerData* Found = SpawnerDataCache.Find(SpawnNodeID))
	{
		return Found->CanRespawnAtErosionLevel(CurrentErosionLevel);
	}
	
	return false;
}
#pragma endregion

#pragma region Debug
//========================================
// Statistics & Debug
//========================================

// 캐시에 저장된 데이터 통계 정보를 가져오기
void USpawnerDataSubsystem::GetCacheStatistics(int32& OutSpawnNodeCount) const
{
	OutSpawnNodeCount = SpawnerDataCache.Num(); // 스폰 노드 캐시에 저장된 데이터 수
}

// 캐시 디버그 정보를 콘솔에 출력
void USpawnerDataSubsystem::PrintCacheDebugInfo() const
{
	UE_LOG(LogSpawnerDataSubsystem, Display, TEXT("=== SpawnerDataSubsystem Cache Statistics ==="));
	UE_LOG(LogSpawnerDataSubsystem, Display, TEXT("Initialized: %s"), bIsInitialized ? TEXT("Yes") : TEXT("No"));
	UE_LOG(LogSpawnerDataSubsystem, Display, TEXT("SpawnNodes Cached: %d"), SpawnerDataCache.Num());
	
	// 스폰 노드 통계 출력
	if (bIsInitialized && SpawnerDataCache.Num() > 0)
	{
		UE_LOG(LogSpawnerDataSubsystem, Display, TEXT("\n--- Zone Tag Statistics ---"));
		
		TMap<EZoneTag, int32> ZoneTagCounts;
		for (const TPair<int32, FSpawnerData>& Pair : SpawnerDataCache)
		{
			for (EZoneTag Tag : Pair.Value.AllowedZoneTags)
			{
				int32& Count = ZoneTagCounts.FindOrAdd(Tag, 0);
				Count++;
			}
		}
		
		for (const TPair<EZoneTag, int32>& Pair : ZoneTagCounts)
		{
			FString ZoneTagName = UEnum::GetValueAsString(Pair.Key);
			UE_LOG(LogSpawnerDataSubsystem, Display, TEXT("  %s: %d"), *ZoneTagName, Pair.Value);
		}
		
		// 거인 셔플 대상 통계
		UE_LOG(LogSpawnerDataSubsystem, Display, TEXT("\n--- Giant Shuffle Vulnerability ---"));
		int32 VulnerableCount = 0;
		int32 NonVulnerableCount = 0;
		
		for (const TPair<int32, FSpawnerData>& Pair : SpawnerDataCache)
		{
			if (Pair.Value.bVulnerableToGiantShuffle)
			{
				VulnerableCount++;
			}
			else
			{
				NonVulnerableCount++;
			}
		}
		
		UE_LOG(LogSpawnerDataSubsystem, Display, TEXT("  Vulnerable: %d"), VulnerableCount);
		UE_LOG(LogSpawnerDataSubsystem, Display, TEXT("  Non-Vulnerable: %d"), NonVulnerableCount);
		
		// 리스폰 조건 통계
		UE_LOG(LogSpawnerDataSubsystem, Display, TEXT("\n--- Respawn Condition Statistics ---"));
		TMap<ERespawnConditionTag, int32> RespawnConditionCounts;
		
		for (const TPair<int32, FSpawnerData>& Pair : SpawnerDataCache)
		{
			int32& Count = RespawnConditionCounts.FindOrAdd(Pair.Value.RespawnConditionTag, 0);
			Count++;
		}
		
		for (const TPair<ERespawnConditionTag, int32>& Pair : RespawnConditionCounts)
		{
			FString ConditionName = UEnum::GetValueAsString(Pair.Key);
			UE_LOG(LogSpawnerDataSubsystem, Display, TEXT("  %s: %d"), *ConditionName, Pair.Value);
		}
		
		// 스폰 마커 그룹 통계
		UE_LOG(LogSpawnerDataSubsystem, Display, TEXT("\n--- Spawn Marker Group Statistics ---"));
		TMap<int32, int32> MarkerGroupCounts;
		
		for (const TPair<int32, FSpawnerData>& Pair : SpawnerDataCache)
		{
			int32& Count = MarkerGroupCounts.FindOrAdd(Pair.Value.SpawnMarkerGroup, 0);
			Count++;
		}
		
		for (const TPair<int32, int32>& Pair : MarkerGroupCounts)
		{
			UE_LOG(LogSpawnerDataSubsystem, Display, TEXT("  MarkerGroup %d: %d spawn nodes"), Pair.Key, Pair.Value);
		}
		
		// 침식도 기반 리스폰 통계
		UE_LOG(LogSpawnerDataSubsystem, Display, TEXT("\n--- Respawn Erosion Max Statistics ---"));
		TMap<int32, int32> ErosionMaxCounts;
		
		for (const TPair<int32, FSpawnerData>& Pair : SpawnerDataCache)
		{
			int32& Count = ErosionMaxCounts.FindOrAdd(Pair.Value.RespawnErosionMax, 0);
			Count++;
		}
		
		for (const TPair<int32, int32>& Pair : ErosionMaxCounts)
		{
			UE_LOG(LogSpawnerDataSubsystem, Display, TEXT("  ErosionMax %d: %d spawn nodes"), Pair.Key, Pair.Value);
		}
	}
	
	UE_LOG(LogSpawnerDataSubsystem, Display, TEXT("==============================================="));
}

void USpawnerDataSubsystem::PrintSpawnNodeDebugInfo(int32 SpawnNodeID) const
{
	if (!bIsInitialized)
	{
		UE_LOG(LogSpawnerDataSubsystem, Warning, TEXT("[PrintSpawnNodeDebugInfo] SpawnerDataSubsystem is NOT initialized."));
		return;
	}
	
	const FSpawnerData* FoundData = SpawnerDataCache.Find(SpawnNodeID);
	
	if (!FoundData)
	{
		UE_LOG(LogSpawnerDataSubsystem, Warning, TEXT("[PrintSpawnNodeDebugInfo] SpawnNodeID %d not found in SpawnerDataCache."), SpawnNodeID);
		return;
	}
	
	UE_LOG(LogSpawnerDataSubsystem, Display, TEXT("\n========== Debug Info for SpawnNodeID %d =========="), SpawnNodeID);
	FoundData->PrintDebugInfo();
	UE_LOG(LogSpawnerDataSubsystem, Display, TEXT("====================================================\n"));
}

#if WITH_EDITOR
// 캐시를 새로고침 (TableAsset 기반으로 다시 로드)
void USpawnerDataSubsystem::RefreshCache()
{
	// TableAsset이 설정되지 않았으면 경고 후 종료
	if (!TableAsset)
	{
		UE_LOG(LogSpawnerDataSubsystem, Warning, TEXT("RefreshCache: TableAsset이 설정되지 않음"));
		return;
	}
	
	UE_LOG(LogSpawnerDataSubsystem, Log, TEXT("TableAsset에서 캐시 새로고침 시작..."));
	
	// 기존 캐시 초기화 후 데이터 재캐싱
	ClearAllCaches();
	CacheSpawnerData();
	
	UE_LOG(LogSpawnerDataSubsystem, Log, TEXT("캐시 새로고침 완료"));
	// 새로고침 후 캐시 상태 디버그 출력
	PrintCacheDebugInfo();
}

void USpawnerDataSubsystem::RunInitializationTests()
{
	UE_LOG(LogSpawnerDataSubsystem, Display, TEXT("========================================"));
	UE_LOG(LogSpawnerDataSubsystem, Display, TEXT("SpawnerDataSubsystem 초기화 테스트 시작"));
	UE_LOG(LogSpawnerDataSubsystem, Display, TEXT("========================================"));
	
	// 1. 통계 출력
	PrintCacheDebugInfo();
	
	// 2. 스폰 노드 조회 테스트
	UE_LOG(LogSpawnerDataSubsystem, Display, TEXT("\n--- 스폰 노드 조회 테스트 ---"));
	FSpawnerData SpawnData;
	if (GetSpawnerDataSafe(1, SpawnData))
	{
		UE_LOG(LogSpawnerDataSubsystem, Display, TEXT("✅ 스폰 노드 ID 1 찾음:"));
		UE_LOG(LogSpawnerDataSubsystem, Display, TEXT("  - ResourceNodeID: %d"), SpawnData.ResourceNodeID);
		UE_LOG(LogSpawnerDataSubsystem, Display, TEXT("  - SpawnMarkerGroup: %d"), SpawnData.SpawnMarkerGroup);
		UE_LOG(LogSpawnerDataSubsystem, Display, TEXT("  - MinSpawnCount: %d"), SpawnData.MinSpawnCount);
		UE_LOG(LogSpawnerDataSubsystem, Display, TEXT("  - MaxSpawnCount: %d"), SpawnData.MaxSpawnCount);
		UE_LOG(LogSpawnerDataSubsystem, Display, TEXT("  - RespawnInterval: %.2f초"), SpawnData.RespawnInterval);
		UE_LOG(LogSpawnerDataSubsystem, Display, TEXT("  - AllowedZoneTags: %d"), SpawnData.AllowedZoneTags.Num());
	}
	else
	{
		UE_LOG(LogSpawnerDataSubsystem, Warning, TEXT("❌ 스폰 노드 ID 1을 찾을 수 없습니다"));
	}
	
	if (GetSpawnerDataSafe(5, SpawnData))
	{
		UE_LOG(LogSpawnerDataSubsystem, Display, TEXT("✅ 스폰 노드 ID 5 찾음:"));
		PrintSpawnNodeDebugInfo(5);
	}
	else
	{
		UE_LOG(LogSpawnerDataSubsystem, Warning, TEXT("❌ 스폰 노드 ID 5를 찾을 수 없습니다"));
	}
	
	// 3. 존재하지 않는 ID 테스트
	UE_LOG(LogSpawnerDataSubsystem, Display, TEXT("\n--- 존재하지 않는 ID 테스트 ---"));
	if (GetSpawnerDataSafe(9999, SpawnData))
	{
		UE_LOG(LogSpawnerDataSubsystem, Display, TEXT("✅ ID 9999 찾음 (이상함!)"));
	}
	else
	{
		UE_LOG(LogSpawnerDataSubsystem, Display, TEXT("❌ ID 9999를 찾을 수 없습니다 (정상)"));
	}
	
	// 4. 자원 노드별 필터링 테스트
	UE_LOG(LogSpawnerDataSubsystem, Display, TEXT("\n--- 자원 노드별 필터링 테스트 ---"));
	
	// ResourceNodeID 100을 생성하는 스폰 노드 검색
	TArray<int32> SpawnNodesForResource100 = GetSpawnNodeIDsByResourceNode(100);
	UE_LOG(LogSpawnerDataSubsystem, Display, TEXT("ResourceNodeID 100을 생성하는 스폰 노드: %d개"), SpawnNodesForResource100.Num());
	if (SpawnNodesForResource100.Num() > 0)
	{
		UE_LOG(LogSpawnerDataSubsystem, Display, TEXT("  예시: SpawnNodeID %d"), SpawnNodesForResource100[0]);
	}
	
	// 5. 스폰 마커 그룹별 필터링 테스트
	UE_LOG(LogSpawnerDataSubsystem, Display, TEXT("\n--- 스폰 마커 그룹별 필터링 테스트 ---"));
	
	TArray<int32> MarkerGroup1Nodes = GetSpawnNodeIDsByMarkerGroup(1);
	UE_LOG(LogSpawnerDataSubsystem, Display, TEXT("MarkerGroup 1의 스폰 노드: %d개"), MarkerGroup1Nodes.Num());
	
	// 6. 구역 태그별 필터링 테스트
	UE_LOG(LogSpawnerDataSubsystem, Display, TEXT("\n--- 구역 태그별 필터링 테스트 ---"));
	
	TArray<int32> SafetyZoneNodes = GetSpawnNodeIDsByZoneTag(EZoneTag::SAFETY_ZONE);
	UE_LOG(LogSpawnerDataSubsystem, Display, TEXT("SAFETY_ZONE 스폰 노드: %d개"), SafetyZoneNodes.Num());
	
	TArray<int32> DarkZoneNodes = GetSpawnNodeIDsByZoneTag(EZoneTag::DARK_ZONE);
	UE_LOG(LogSpawnerDataSubsystem, Display, TEXT("DARK_ZONE 스폰 노드: %d개"), DarkZoneNodes.Num());
	
	// 7. 거인 셔플 대상 필터링 테스트
	UE_LOG(LogSpawnerDataSubsystem, Display, TEXT("\n--- 거인 셔플 대상 필터링 테스트 ---"));
	
	TArray<int32> ShuffleVulnerableNodes = GetShuffleVulnerableSpawnNodeIDs();
	UE_LOG(LogSpawnerDataSubsystem, Display, TEXT("거인 셔플 대상 스폰 노드: %d개"), ShuffleVulnerableNodes.Num());
	
	// 8. 침식도 기반 리스폰 가능 노드 테스트
	UE_LOG(LogSpawnerDataSubsystem, Display, TEXT("\n--- 침식도 기반 리스폰 테스트 ---"));
	
	// 침식도 레벨 3에서 리스폰 가능한 노드
	TArray<int32> RespawnableAtLevel3 = GetRespawnableSpawnNodeIDs(3);
	UE_LOG(LogSpawnerDataSubsystem, Display, TEXT("침식도 3에서 리스폰 가능한 노드: %d개"), RespawnableAtLevel3.Num());
	
	// 침식도 레벨 10에서 리스폰 가능한 노드
	TArray<int32> RespawnableAtLevel10 = GetRespawnableSpawnNodeIDs(10);
	UE_LOG(LogSpawnerDataSubsystem, Display, TEXT("침식도 10에서 리스폰 가능한 노드: %d개"), RespawnableAtLevel10.Num());
	
	// 9. 리스폰 조건별 필터링 테스트
	UE_LOG(LogSpawnerDataSubsystem, Display, TEXT("\n--- 리스폰 조건별 필터링 테스트 ---"));
	
	TArray<int32> PlayerAwayNodes = GetSpawnNodeIDsByRespawnCondition(ERespawnConditionTag::PLAYER_AWAY);
	UE_LOG(LogSpawnerDataSubsystem, Display, TEXT("PLAYER_AWAY 조건 노드: %d개"), PlayerAwayNodes.Num());
	
	TArray<int32> ErosionResetNodes = GetSpawnNodeIDsByRespawnCondition(ERespawnConditionTag::EROSION_RESET);
	UE_LOG(LogSpawnerDataSubsystem, Display, TEXT("EROSION_RESET 조건 노드: %d개"), ErosionResetNodes.Num());
	
	// 10. 스폰 가능 여부 검증 테스트
	UE_LOG(LogSpawnerDataSubsystem, Display, TEXT("\n--- 스폰 가능 여부 테스트 ---"));
	
	if (IsSpawnerDataValid(1))
	{
		UE_LOG(LogSpawnerDataSubsystem, Display, TEXT("✅ SpawnNodeID 1 유효함"));
		if (IsSpawnable(1))
		{
			UE_LOG(LogSpawnerDataSubsystem, Display, TEXT("  ✅ 스폰 가능 (확률 통과)"));
		}
		else
		{
			UE_LOG(LogSpawnerDataSubsystem, Display, TEXT("  ❌ 스폰 불가능 (확률 실패 또는 데이터 오류)"));
		}
		
		// 침식도별 리스폰 가능 여부 테스트
		if (CanRespawnAtErosionLevel(1, 3))
		{
			UE_LOG(LogSpawnerDataSubsystem, Display, TEXT("  ✅ 침식도 3에서 리스폰 가능"));
		}
		else
		{
			UE_LOG(LogSpawnerDataSubsystem, Display, TEXT("  ❌ 침식도 3에서 리스폰 불가능"));
		}
	}
	else
	{
		UE_LOG(LogSpawnerDataSubsystem, Warning, TEXT("❌ SpawnNodeID 1 유효하지 않음"));
	}
	
	// 11. 랜덤 스폰 개수 테스트
	UE_LOG(LogSpawnerDataSubsystem, Display, TEXT("\n--- 랜덤 스폰 개수 테스트 ---"));
	if (GetSpawnerDataSafe(1, SpawnData))
	{
		UE_LOG(LogSpawnerDataSubsystem, Display, TEXT("SpawnNodeID 1의 스폰 개수 범위: %d ~ %d"), 
			SpawnData.MinSpawnCount, SpawnData.MaxSpawnCount);
		
		// 5번 랜덤 생성 테스트
		UE_LOG(LogSpawnerDataSubsystem, Display, TEXT("5번 랜덤 생성 결과:"));
		for (int32 i = 0; i < 5; ++i)
		{
			int32 RandomCount = SpawnData.GetRandomSpawnCount();
			UE_LOG(LogSpawnerDataSubsystem, Display, TEXT("  [%d] %d"), i + 1, RandomCount);
		}
	}
	
	UE_LOG(LogSpawnerDataSubsystem, Display, TEXT("========================================"));
	UE_LOG(LogSpawnerDataSubsystem, Display, TEXT("테스트 완료"));
	UE_LOG(LogSpawnerDataSubsystem, Display, TEXT("========================================"));
}
#endif
#pragma endregion

#pragma region InternalCachingFunctions
//========================================
// Internal Caching Functions
//========================================

// 스폰 데이터를 캐시에 저장
void USpawnerDataSubsystem::CacheSpawnerData()
{
	// TableAsset 또는 SpawnerDataTable이 유효하지 않으면 경고 후 종료
	if (!TableAsset || !TableAsset->SpawnerDataTable)
	{
		UE_LOG(LogSpawnerDataSubsystem, Warning, TEXT("CacheSpawnerData: 유효하지 않은 SpawnerDataTable"));
		return;
	}
	
	const UDataTable* DataTable = TableAsset->SpawnerDataTable;
	TArray<FSpawnerData*> AllRows;
	// 데이터테이블에서 모든 스폰 데이터 가져오기
	DataTable->GetAllRows<FSpawnerData>(TEXT("CacheSpawnerData"), AllRows);
	
	for (FSpawnerData* RowData : AllRows)
	{
		// 유효한 스폰 노드 ID를 가진 데이터만 캐시에 추가
		if (RowData && RowData->SpawnNodeID > 0)
		{
			SpawnerDataCache.Add(RowData->SpawnNodeID, *RowData);
		}
		else
		{
			UE_LOG(LogSpawnerDataSubsystem, Warning, TEXT("유효하지 않은 SpawnerData 행 (ID: %d)"), 
				RowData ? RowData->SpawnNodeID : -1);
		}
	}
	
	UE_LOG(LogSpawnerDataSubsystem, Log, TEXT("총 %d개의 스폰 노드 캐싱 완료"), SpawnerDataCache.Num());
}

// 모든 캐시 데이터를 초기화
void USpawnerDataSubsystem::ClearAllCaches()
{
	// 스폰 노드 캐시 비우기
	SpawnerDataCache.Empty();
	
	UE_LOG(LogSpawnerDataSubsystem, Log, TEXT("All caches cleared"));
}

// TableAsset이 유효한지 검증
bool USpawnerDataSubsystem::ValidateTableAsset(const USpawnerTableAsset* InTableAsset) const
{
	// TableAsset이 null인지 확인
	if (!InTableAsset)
	{
		UE_LOG(LogSpawnerDataSubsystem, Error, TEXT("TableAsset이 nullptr입니다"));
		return false;
	}
	
	bool bIsValid = true;
	
	// 스폰 데이터 테이블 존재 여부 확인
	if (!InTableAsset->SpawnerDataTable)
	{
		UE_LOG(LogSpawnerDataSubsystem, Error, TEXT("TableAsset에 SpawnerDataTable이 설정되어 있지 않습니다!"));
		bIsValid = false;
	}

	return bIsValid;
}
#pragma endregion