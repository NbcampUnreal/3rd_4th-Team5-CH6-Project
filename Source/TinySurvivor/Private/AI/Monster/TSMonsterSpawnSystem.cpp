#include "AI/Monster/TSMonsterSpawnSystem.h"
#include "AI/Monster/TSMonsterDataAsset.h"
#include "AI/Monster/TSMonsterSetting.h"
#include "AI/Monster/TSMonsterTable.h"
#include "AI/Monster/Base/MonsterInterface.h"
#include "GameFramework/Character.h"

DEFINE_LOG_CATEGORY(MonsterManager);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
	
	//---------------------------------------
	// UTPWorldDropItemSystem 라이프 사이클
	//--------------------------------------
	
UTSMonsterSpawnSystem::UTSMonsterSpawnSystem()
	: CachedWorldDropDataAsset(nullptr)
	, CachedbWantPrintLog(false)
{
}

bool UTSMonsterSpawnSystem::ShouldCreateSubsystem(UObject* Outer) const
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
		UE_LOG(MonsterManager, Log, TEXT("ShouldCreateSubsystem: 서버에서 생성 (NetMode: %d)"), 
			static_cast<int32>(World->GetNetMode()));
	}
	else
	{// 클라이언트
		UE_LOG(MonsterManager, Log, TEXT("ShouldCreateSubsystem: 클라이언트에서는 생성하지 않음"));
	}
	
	return bIsServer;
}

void UTSMonsterSpawnSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
#pragma region 개발자_세팅_설정
	// 세이브 로드 세팅 가져오기
	UTSMonsterSetting* MonsterSetting = GetMutableDefault<UTSMonsterSetting>();
	if (!IsValid(MonsterSetting))
	{
		return;
	}
	
	UTSMonsterDataAsset* WorldDropDataAsset= MonsterSetting->MonsterDataAsset.LoadSynchronous();
	if (!IsValid(WorldDropDataAsset))
	{
		UE_LOG(MonsterManager, Error, TEXT("WorldDropDataAsset: 실패"));
		return;
	}
			
	CachedWorldDropDataAsset = WorldDropDataAsset;
	
	if (!IsValid(CachedWorldDropDataAsset))
	{
		UE_LOG(MonsterManager, Error, TEXT("CachedWorldDropDataAsset: 실패"));
		return;
	}
	
	CachedbWantPrintLog = CachedWorldDropDataAsset->bWantPrintLog;
#pragma endregion

	// 테이블 캐싱
	if (IsValid(CachedWorldDropDataAsset->MonsterTable))
	{
		TArray<FTSMonsterTable*> Rows;
		CachedWorldDropDataAsset->MonsterTable->GetAllRows<FTSMonsterTable>(TEXT("CachingTables"), Rows);
		
		for (const auto& Row : Rows)
		{
			if (!Row) continue;
			if (Row->MonsterTag == FGameplayTag::EmptyTag) continue;
			if (!IsValid(Row->MonsterClass)) continue;
			
			FTSMonsterTable CachingRow;
			CachingRow.MonsterTag = Row->MonsterTag;
			CachingRow.MonsterClass = Row->MonsterClass;
			CachingRows.Add(MoveTemp(CachingRow));
			CachingMonsterMap.Add(CachingRow.MonsterTag, &CachingRows.Last());
		}
	}
}

void UTSMonsterSpawnSystem::PostInitialize()
{
	Super::PostInitialize();
}

void UTSMonsterSpawnSystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
}

void UTSMonsterSpawnSystem::Deinitialize()
{
	Super::Deinitialize();
}

UTSMonsterSpawnSystem* UTSMonsterSpawnSystem::Get(const UObject* WorldContextObject)
{
	if (!IsValid(WorldContextObject))
	{
		return nullptr;
	}
	
	UWorld* World = WorldContextObject->GetWorld();
	if (!IsValid(World))
	{
		return nullptr;
	}
			
	return World->GetSubsystem<UTSMonsterSpawnSystem>();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
	
	//---------------------------------------
	// 스폰 요청 API
	//--------------------------------------

bool UTSMonsterSpawnSystem::RequestSpawnMonsterWithBP(FTransform SpawnParms, FGameplayTag MonsterTag)
{
	return RequestMonsterSpawn(SpawnParms, MonsterTag);
}

bool UTSMonsterSpawnSystem::RequestMonsterSpawn(FTransform& SpawnParms, FGameplayTag& MonsterTag)
{
	FTSMonsterTable* FoundMonsterData = CachingMonsterMap.FindRef(MonsterTag);
	if (!FoundMonsterData) return false;
	if (!IsValid(FoundMonsterData->MonsterClass)) return false;
	
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AActor* SpawnedMonster = GetWorld()->SpawnActor(FoundMonsterData->MonsterClass, &SpawnParms, SpawnParameters);
	if (!IsValid(SpawnedMonster))
	{
		return false;
	}
	else
	{
		AController* SpawnedMonsterAIC = Cast<ACharacter>(SpawnedMonster)->GetController();
		if (!IsValid(SpawnedMonsterAIC)) return false;
		
		IMonsterInterface* MonsterAICInterface = Cast<IMonsterInterface>(SpawnedMonsterAIC);
		if (MonsterAICInterface)
		{
			MonsterAICInterface->SetSpawningPoint(SpawnParms.GetLocation());
			MonsterAICInterface->StartLogic();
			UE_LOG(MonsterManager, Log, TEXT("RequestMonsterSpawn: %s"), *SpawnedMonster->GetName());
			return true;
		}
		else
		{
			return false;
		}
	}
}
