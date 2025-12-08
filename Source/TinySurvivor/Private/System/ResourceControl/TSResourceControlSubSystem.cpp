#include "System/ResourceControl/TSResourceControlSubSystem.h"
#include "AI/Gaint/System/GiantSwitchingResourceAreaSubSystem.h"
#include "Character/TSCharacter.h"
#include "Engine/AssetManager.h"
#include "Item/System/ItemDataSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "System/Erosion/TSErosionSubSystem.h"
#include "System/ResourceControl/TSReousrceControlSystemDataTable.h"
#include "System/ResourceControl/TSResourceBaseActor.h"
#include "System/ResourceControl/TSResourceControlDataAsset.h"
#include "System/ResourceControl/TSResourcePoint.h"
#include "System/Time/TimeTickManager.h"

DEFINE_LOG_CATEGORY(ResourceControlSystem)

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	

	//------------------------------
	// UTSResourceControlSubSystem 라이프 사이클 
	//------------------------------
	
bool UTSResourceControlSubSystem::ShouldCreateSubsystem(UObject* Outer) const
{
	// 부모 클래스 체크
	if (!Super::ShouldCreateSubsystem(Outer))
	{
		return false;
	}
	
	// World 유효성 체크
	UWorld* World = Cast<UWorld>(Outer);
	if (!World)
	{// World가 없으면 서브시스템을 생성하지 않음
		return false;
	}
	
	// 게임 월드인지 확인 (에디터 프리뷰, PIE 등은 제외)
	if (!World->IsGameWorld())
	{// 게임 실행 중인 월드가 아니면 생성하지 않음
		return false;
	}
	
	// 서버에서만 생성 (클라이언트에서는 생성하지 않음)
	// NetMode 종류:
	//  - NM_Standalone      (0): 싱글플레이 → 서버 역할
	//  - NM_DedicatedServer (1): 데디케이티드 서버 → 서버
	//  - NM_ListenServer    (2): 리슨 서버 → 서버
	//  - NM_Client          (3): 클라이언트 → 생성 안 함!
	const bool bIsServer = World->GetNetMode() != NM_Client;
	if (bIsServer)
	{// 서버
		UE_LOG(ResourceControlSystem, Log, TEXT("ShouldCreateSubsystem: 서버에서 생성 (NetMode: %d)"), 
			static_cast<int32>(World->GetNetMode()));
	}
	else
	{// 클라이언트
		UE_LOG(ResourceControlSystem, Log, TEXT("ShouldCreateSubsystem: 클라이언트에서는 생성하지 않음"));
	}
	
	return bIsServer;
}

void UTSResourceControlSubSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

#pragma region 데이터 에셋 가져오기
	
	// 1) AssetManager 접근
	UAssetManager& AssetMgr = UAssetManager::Get();

	// 2) PrimaryAssetId 정의
	const FPrimaryAssetId ConfigId(TEXT("ResourceControlData"), TEXT("DA_ResourceControlData"));
	UE_LOG(ResourceControlSystem, Warning, TEXT("[ResourceControlSystem] Trying to sync-load asset: %s"), *ConfigId.ToString());

	// 3) AssetManager에서 경로 얻기
	FSoftObjectPath AssetPath = AssetMgr.GetPrimaryAssetPath(ConfigId);
	if (!AssetPath.IsValid())
	{
		UE_LOG(ResourceControlSystem, Error, TEXT("[ResourceControlSystem] Invalid asset path for: %s"), *ConfigId.ToString());
		return;
	}

	// 4) 완전 동기 로드 (Blocking)
	// 인 게임 시작 전 필요한 필수 데이터 에셋은 웬만하면 동기 로드로 타이밍 안전성 확보할 것.
	UObject* LoadedObject = AssetPath.TryLoad();
	if (!LoadedObject)
	{
		UE_LOG(ResourceControlSystem, Error, TEXT("[ResourceControlSystem] Failed to synchronously load asset: %s"), *AssetPath.ToString());
		return;
	}

	// 5) 캐스팅
	ResourceControlConfig = Cast<UTSResourceControlDataAsset>(LoadedObject);
	if (ResourceControlConfig)
	{
		UE_LOG(ResourceControlSystem, Warning, TEXT("[ResourceControlSystem] Sync loaded config: %s"), *ResourceControlConfig->GetName());
		bShowDebug = ResourceControlConfig->bShowDebug;
	}
	else
	{
		UE_LOG(ResourceControlSystem, Error, TEXT("[ResourceControlSystem] Loaded asset is not UErosionConfigData type! Path: %s"), *AssetPath.ToString());
	}
	
#pragma endregion
	
	BuildSectorSpawnRequests();
}

void UTSResourceControlSubSystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	
	if (!IsValid(GetWorld())) return;

	FTimerHandle SectorSpawnTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(SectorSpawnTimerHandle, this, &UTSResourceControlSubSystem::AssignResourcesToPoints, 3.f, false);
}

void UTSResourceControlSubSystem::Deinitialize()
{
	if (IsValid(GetWorld()))
	{
		GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
	}
	Super::Deinitialize();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	

	//------------------------------
	// UTSResourceControlSubSystem 외부 유틸
	//------------------------------

UTSResourceControlSubSystem* UTSResourceControlSubSystem::GetUTSResourceControlSubSystem(const UObject* WorldContextObject)
{
	if (IsValid(WorldContextObject))
	{
		if (UWorld* World = WorldContextObject->GetWorld(); IsValid(World))
		{
			return World->GetSubsystem<UTSResourceControlSubSystem>();
		}
	}
	return nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	

	//------------------------------
	// UTSResourceControlSubSystem 자원 포인트 관리
	//------------------------------

void UTSResourceControlSubSystem::AssignReSourcePointToSector(const TObjectPtr<ATSResourcePoint> ResourcePoint)
{
	if (!IsValid(ResourcePoint))
	{
		if (bShowDebug) UE_LOG(ResourceControlSystem, Warning, TEXT("등록하려는 자원 포인트가 유요하지 않음."));
		return;
	}
	
	const FGameplayTag SectionTag = ResourcePoint->GetSectionTag();
	const ENodeType NodeType = ResourcePoint->GetResourceItemType();

	// 섹터 Bucket 가져오기 (없으면 새로 생성)
	FResourcePointBucket& Bucket = ResourcePointMap.FindOrAdd(SectionTag);

	// Bucket 안에서 현재 NodeType과 동일한 그룹(FResourcePoints)을 찾기
	FResourcePoints* FoundGroup = nullptr;
	for (FResourcePoints& Group : Bucket.ResourcePointBucket)
	{
		if (Group.NodeType == NodeType)
		{
			FoundGroup = &Group;
			break;
		}
	}
	
	// NodeType 그룹이 없으면 새로 만들기
	if (!FoundGroup)
	{
		if (bShowDebug) UE_LOG(ResourceControlSystem, Warning, TEXT("새로운 노드 타입 그룹 생성됨. Section: %s, NodeType: %d"), *SectionTag.ToString(), (int32)NodeType);
		FResourcePoints NewGroup;
		NewGroup.NodeType = NodeType;
		NewGroup.CachedResourcePoints.AddUnique(ResourcePoint);
		Bucket.ResourcePointBucket.Add(MoveTemp(NewGroup));
		DebugPrintAllBuckets();
		return;
	}

	// 4) 기존 그룹이 있다면 포인트 추가
	if (!FoundGroup->CachedResourcePoints.Contains(ResourcePoint))
	{
		FoundGroup->CachedResourcePoints.AddUnique(ResourcePoint);
		if (bShowDebug) UE_LOG(ResourceControlSystem, Warning, TEXT("기존 노드 타입 그룹에 포인트 추가됨. Section: %s, NodeType: %d"),*SectionTag.ToString(), (int32)NodeType);
		DebugPrintAllBuckets();
	}
}

void UTSResourceControlSubSystem::RequestRespawn(const FGameplayTag& SectionTag, int32 ResourceID, float RespawnTime,ENodeType NodeType, const FGameplayTag& UniqueTag, bool bIsCommon)
{
	FRespawnRequest Req;
	Req.SectionTag = SectionTag;
	Req.ResourceID = ResourceID;
	Req.TimeRemaining = RespawnTime;
	Req.bIsCommon = bIsCommon;
	Req.NodeType = NodeType;
	Req.UniqueTag = UniqueTag;

	PendingRespawns.Add(Req);

	if (bShowDebug) UE_LOG(ResourceControlSystem, Warning, TEXT("Respawn 요청: Sector=%s ResourceID=%d Common=%d NodeType=%d UniqueTag=%s RespawnTime=%.1f"), *SectionTag.ToString(), ResourceID, bIsCommon, (int32)NodeType, *UniqueTag.ToString(), RespawnTime);
}

void UTSResourceControlSubSystem::BuildSectorSpawnRequests()
{
	if (bShowDebug) UE_LOG(ResourceControlSystem, Warning, TEXT("===== BuildSectorSpawnRequests: 시작 ====="));

	if (!ResourceControlConfig)
	{
		if (bShowDebug) UE_LOG(ResourceControlSystem, Error, TEXT("ResourceControlConfig 없음! 데이터 에셋 로드 실패."));
		return;
	}

	const TArray<FResourceControlData*>& AllRows = ResourceControlConfig->GetAllRows();

	if (bShowDebug) UE_LOG(ResourceControlSystem, Warning, TEXT("총 %d 개의 데이터 테이블 Row 발견"), AllRows.Num());

	for (int32 i = 0; i < AllRows.Num(); i++)
	{
		const FResourceControlData* Row = AllRows[i];
		if (!Row)
		{
			if (bShowDebug) UE_LOG(ResourceControlSystem, Error, TEXT("[%d]번째 Row가 null"), i);
			continue;
		}

		if (bShowDebug) UE_LOG(ResourceControlSystem, Warning,TEXT("[%d] Row → Sector:%s | ResourceID:%d | Num:%d | Common:%d | UniqueTag:%s"),i,*Row->ResourceSectorTag.ToString(),Row->ResourceID,Row->ResourceSpawnNum,Row->bIsCommonNode,*Row->ResourceUniqueTag.ToString());

		FResourceSpawnRequest Req;
		Req.ResourceID = Row->ResourceID;
		Req.SpawnNum = Row->ResourceSpawnNum;
		Req.bIsCommon = Row->bIsCommonNode;
		Req.UniqueTag = Row->ResourceUniqueTag;

		SectorSpawnRequests.FindOrAdd(Row->ResourceSectorTag).Add(Req);
	}

	if (bShowDebug) UE_LOG(ResourceControlSystem, Warning, TEXT("===== BuildSectorSpawnRequests: 완료 ====="));
}

void UTSResourceControlSubSystem::AssignResourcesToPoints()
{
    if (bShowDebug) UE_LOG(ResourceControlSystem, Warning, TEXT("===== AssignResourcesToPoints: 시작 ====="));

    for (auto& SectorElem : SectorSpawnRequests)
    {
        const FGameplayTag SectorTag = SectorElem.Key;
        const TArray<FResourceSpawnRequest>& Requests = SectorElem.Value;

       if (bShowDebug)  UE_LOG(ResourceControlSystem, Warning, TEXT("섹터 [%s] 처리 시작 (요구된 자원 종류 %d개)"), *SectorTag.ToString(), Requests.Num());

        FResourcePointBucket* BucketPtr = ResourcePointMap.Find(SectorTag);
        if (!BucketPtr)
        {
           if (bShowDebug)  UE_LOG(ResourceControlSystem, Error, TEXT("섹터 [%s] → ResourcePoint 없음! 스폰 불가"), *SectorTag.ToString());
            continue;
        }

        FResourcePointBucket& Bucket = *BucketPtr;
        if (bShowDebug) UE_LOG(ResourceControlSystem, Warning, TEXT("섹터 [%s] → 총 NodeType 그룹 %d개 발견"), *SectorTag.ToString(), Bucket.ResourcePointBucket.Num());


        // ============================================================
        // 1) 유니크 자원 → 랜덤 포인트 배정
        // ============================================================
        if (bShowDebug) UE_LOG(ResourceControlSystem, Warning, TEXT("섹터 [%s] - 유니크 자원 랜덤 매칭 시작"), *SectorTag.ToString());
        for (const FResourceSpawnRequest& Req : Requests)
        {
            if (Req.bIsCommon) continue; // 유니크만 처리

            TArray<ATSResourcePoint*> Candidates;

            for (FResourcePoints& Group : Bucket.ResourcePointBucket)
            {
                for (ATSResourcePoint* RP : Group.CachedResourcePoints)
                {
                    if (!RP || RP->IsAllocated()) continue;

                    if (RP->GetSectionResourceUniqueTag() == Req.UniqueTag)
                    {
                        Candidates.Add(RP);
                    }
                }
            }

            if (Candidates.IsEmpty())
            {
               if (bShowDebug)  UE_LOG(ResourceControlSystem, Error, TEXT("!!! 유니크 %s 매칭 실패: 후보 포인트 없음"), *Req.UniqueTag.ToString());
                continue;
            }

            // 위치 랜덤 선택
            ATSResourcePoint* TargetRP = Candidates[FMath::RandRange(0, Candidates.Num() - 1)];
           if (bShowDebug)  UE_LOG(ResourceControlSystem, Warning, TEXT("유니크 스폰! Sector %s → %s 포인트를 랜덤 선택하여 ResourceID %d 스폰"), *SectorTag.ToString(), *TargetRP->GetName(), Req.ResourceID);
            SpawnResourceAtPoint(TargetRP, Req.ResourceID);
        }


        // ============================================================
        // 2) 범용(Common) 자원 → 랜덤 포인트 배정
        // ============================================================
       if (bShowDebug)  UE_LOG(ResourceControlSystem, Warning, TEXT("섹터 [%s] - 범용 자원 랜덤 매칭 시작"), *SectorTag.ToString());

        for (const FResourceSpawnRequest& Req : Requests)
        {
            if (!Req.bIsCommon) continue;

            int32 Remaining = Req.SpawnNum;
            ENodeType NodeType = GetNodeTypeFromResourceID(Req.ResourceID);

            // 먼저 후보 리스트 수집
            TArray<ATSResourcePoint*> Candidates;

            for (FResourcePoints& Group : Bucket.ResourcePointBucket)
            {
                if (Group.NodeType != NodeType) continue;

                for (ATSResourcePoint* RP : Group.CachedResourcePoints)
                {
                    if (!RP || RP->IsAllocated()) continue;
                    Candidates.Add(RP);
                }
            }

            if (Candidates.IsEmpty())
            {
               if (bShowDebug)  UE_LOG(ResourceControlSystem, Error, TEXT("섹터 [%s] → 범용 ResourceID %d 노드타입(%d) 후보 없음"), *SectorTag.ToString(), Req.ResourceID, (int32)NodeType);
                continue;
            }

            // 랜덤 선택 루프
            while (Remaining > 0 && Candidates.Num() > 0)
            {
                int32 Index = FMath::RandRange(0, Candidates.Num() - 1);
                ATSResourcePoint* RP = Candidates[Index];
                SpawnResourceAtPoint(RP, Req.ResourceID);

                // 사용한 포인트 제거
                Candidates.RemoveAt(Index);
                Remaining--;
            }

            if (Remaining > 0)
            {
               if (bShowDebug)  UE_LOG(ResourceControlSystem, Error, TEXT("범용 ResourceID %d 랜덤 스폰 실패 (남은 %d개)"), Req.ResourceID, Remaining);
            }
        }
       if (bShowDebug)  UE_LOG(ResourceControlSystem, Warning, TEXT("섹터 [%s] 처리 완료"), *SectorTag.ToString());
    }
    if (bShowDebug) UE_LOG(ResourceControlSystem, Warning, TEXT("===== AssignResourcesToPoints: 완료 ====="));

	
    // === Respawn 타이머 연결 ===
    UTimeTickManager* TimeTickManager = GetWorld()->GetSubsystem<UTimeTickManager>();
    if (!IsValid(TimeTickManager))
    {
        if (bShowDebug) UE_LOG(ResourceControlSystem, Error, TEXT("ResourceControlSystem: TimeTickManager 없음"));
        return;
    }

    TimeTickManager->OnSecondTick.AddDynamic(this, &UTSResourceControlSubSystem::UpdateRespawnControl);
  if (bShowDebug)   UE_LOG(ResourceControlSystem, Log, TEXT("DecayManager 초기화 완료 (TimeTickManager에 구독됨)"));
	
	// === 침식도 매니저 연결 ===
	UTSErosionSubSystem* ErosionSystem = GetWorld()->GetSubsystem<UTSErosionSubSystem>();
	if (!IsValid(ErosionSystem))
	{
		if (bShowDebug) UE_LOG(ResourceControlSystem, Error, TEXT("ResourceControlSystem: ErosionSystem 없음"));
		return;
	}
	
	ErosionSystem->OnErosionChangedDelegate.AddDynamic(this, &UTSResourceControlSubSystem::OnErosionChanged);
	
}

ENodeType UTSResourceControlSubSystem::GetNodeTypeFromResourceID(int32 ResourceID)
{
	FResourceData Data;
	if (UItemDataSubsystem* ItemSub = GetWorld()->GetGameInstance()->GetSubsystem<UItemDataSubsystem>())
	{
		if (ItemSub->GetResourceDataSafe(ResourceID, Data))
		{
			return Data.NodeType;
		}
	}
	return ENodeType::NONE;
}

void UTSResourceControlSubSystem::SpawnResourceAtPoint(ATSResourcePoint* RP, int32 ResourceID)
{
	FResourceData Data;
	if (!GetWorld()->GetGameInstance()->GetSubsystem<UItemDataSubsystem>()->GetResourceDataSafe(ResourceID, Data))
	{
		if (bShowDebug) UE_LOG(ResourceControlSystem, Error, TEXT("ResourceID %d의 Data를 찾지 못함."), ResourceID);
		return;
	}

	// 액터 클래스를 Data에서 가져옴
	if (!Data.ActorClass)
	{
		if (bShowDebug) UE_LOG(ResourceControlSystem, Error, TEXT("ResourceID %d에 ActorClass 없음!"), ResourceID);
		return;
	}

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	ATSResourceBaseActor* Spawned = GetWorld()->SpawnActor<ATSResourceBaseActor>(
		Data.ActorClass,
		RP->GetActorLocation(),
		RP->GetActorRotation(),
		Params
	);

	if (IsValid(Spawned))
	{
		Spawned->SetSpawnPoint(RP);
		Spawned->InitFromResourceData(Data);
		RP->SetAllocatedResource(Spawned);
	}
}

void UTSResourceControlSubSystem::UpdateRespawnControl()
{
	if (PendingRespawns.Num() == 0) return;

	// T침식도 2 이 이상이면 STOP
	if (bIsErosionUpper60) return;
	
	for (int32 i = PendingRespawns.Num() - 1; i >= 0; i--)
	{
		FRespawnRequest& Req = PendingRespawns[i];
		Req.TimeRemaining -= 1;

		if (Req.TimeRemaining <= 0.f)
		{
			bool bSuccess = RespawnResource(Req);
			if (bSuccess)
			{
				PendingRespawns.RemoveAt(i);
			}
		}
	}
}

void UTSResourceControlSubSystem::OnErosionChanged(float ErosionValue)
{
	if (ErosionValue >= 60.f)
	{
		bIsErosionUpper60 = true;
	}
	else
	{
		bIsErosionUpper60 = false;
	}
}

bool UTSResourceControlSubSystem::RespawnResource(const FRespawnRequest& Req)
{
	FResourcePointBucket* BucketPtr = ResourcePointMap.Find(Req.SectionTag);
	if (!BucketPtr)
	{
		if (bShowDebug) UE_LOG(ResourceControlSystem, Error,TEXT("Respawn 실패: Sector %s 에 Bucket 없음"), *Req.SectionTag.ToString());
		return false;
	}

	TArray<ATSResourcePoint*> Candidates;

	for (FResourcePoints& Group : BucketPtr->ResourcePointBucket)
	{
		for (ATSResourcePoint* RP : Group.CachedResourcePoints)
		{
			if (!RP || RP->IsAllocated()) continue;

			// Unique 자원 처리
			if (!Req.bIsCommon)
			{
				if (RP->GetSectionResourceUniqueTag() == Req.UniqueTag)
					Candidates.Add(RP);
			}
			else
			{
				// Common 자원 처리
				if (RP->GetResourceItemType() == Req.NodeType)
					Candidates.Add(RP);
			}
		}
	}

	if (Candidates.IsEmpty())
	{
		if (bShowDebug) UE_LOG(ResourceControlSystem, Warning, TEXT("Respawn 실패: Sector %s 내에 유효한 포인트 없음"), *Req.SectionTag.ToString());
		return false;
	}

	ATSResourcePoint* TargetRP = Candidates[FMath::RandRange(0, Candidates.Num() - 1)];

	if (bShowDebug) UE_LOG(ResourceControlSystem, Warning,TEXT("Respawn 성공: Sector %s 의 %s 포인트에 ResourceID %d 스폰"),*Req.SectionTag.ToString(), *TargetRP->GetName(), Req.ResourceID);

	// 모든 플레이어, 모든 몬스터, 거인과 체크해서 일정 거리 떨어져야 스폰 가능 (끼임 사고 방지)
	if (IsValid(GetWorld()))
	{
		const FVector RP_Location = TargetRP->GetActorLocation();
		const float MinDistance = 1000.f;   // 10m = 1000cm

		// 모든 플레이어, 모든 몬스터, 거인 가져오기
		TArray<AActor*> Players;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACharacter::StaticClass(), Players);

		for (AActor* PlayerActor : Players)
		{
			if (!IsValid(PlayerActor)) continue;

			const float Dist = FVector::Dist(PlayerActor->GetActorLocation(), RP_Location);

			if (Dist <= MinDistance)
			{
				if (bShowDebug) UE_LOG(ResourceControlSystem, Warning, TEXT("[Respawn 실패] 플레이어, 모든 몬스터, 거인과 너무 가까움. 거리: %.1f cm"), Dist);
				return false;   // 스폰 금지
			}
		}
	}
	
	SpawnResourceAtPoint(TargetRP, Req.ResourceID);
	return true;
}

void UTSResourceControlSubSystem::DebugPrintAllBuckets()
{
	if (bShowDebug) UE_LOG(ResourceControlSystem, Warning, TEXT("=========== 전체 버킷 디버그 출력 시작 ==========="));

	for (const auto& Elem : ResourcePointMap)
	{
		const FGameplayTag& SectionTag = Elem.Key;
		const FResourcePointBucket& Bucket = Elem.Value;

		if (bShowDebug) UE_LOG(ResourceControlSystem, Warning, TEXT("섹터: %s"), *SectionTag.ToString());

		for (const FResourcePoints& Group : Bucket.ResourcePointBucket)
		{
			const UEnum* EnumPtr = StaticEnum<ENodeType>();
			FString NodeTypeName = EnumPtr ? EnumPtr->GetDisplayNameTextByValue(static_cast<int64>(Group.NodeType)).ToString() : TEXT("Unknown");

			if (bShowDebug) UE_LOG(ResourceControlSystem, Warning, TEXT("  NodeType: %s"), *NodeTypeName);

			for (const TObjectPtr<ATSResourcePoint>& RP : Group.CachedResourcePoints)
			{
				if (IsValid(RP))
				{
					const FVector Loc = RP->GetActorLocation();
					if (bShowDebug) UE_LOG(ResourceControlSystem, Warning, TEXT("      • %s at (%.1f, %.1f, %.1f)"), *RP->GetName(), Loc.X, Loc.Y, Loc.Z
					);
				}
				else
				{
					if (bShowDebug) UE_LOG(ResourceControlSystem, Warning, TEXT("      • Invalid ResourcePoint (null)"));
				}
			}
		}
	}
	if (bShowDebug) UE_LOG(ResourceControlSystem, Warning, TEXT("=========== 전체 버킷 디버그 출력 끝 ==========="));
}
