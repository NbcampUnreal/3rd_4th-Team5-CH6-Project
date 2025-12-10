// MonsterSpawnSubsystem.cpp

#include "AI/Common/Spawn/MonsterSpawnSubsystem.h"
#include "AI/Common/Spawn/MonsterSpawnConfig.h"
#include "Engine/AssetManager.h"

void UMonsterSpawnSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	CurrentMaxMonsterCount = 5;
}

void UMonsterSpawnSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	
	if (GetWorld()->GetNetMode() == NM_Client)
		return;
	
	if (UTSErosionSubSystem* ErosionSys = UTSErosionSubSystem::GetErosionSubSystem(this))
	{
		ErosionSys->OnErosionChangedDelegate.AddDynamic(this, &UMonsterSpawnSubsystem::OnErosionChanged);
	}
}

void UMonsterSpawnSubsystem::Deinitialize()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(SpawnTimerHandle);
	}
	
	if (UTSErosionSubSystem* ErosionSys = UTSErosionSubSystem::GetErosionSubSystem(this))
	{
		ErosionSys->OnErosionChangedDelegate.RemoveAll(this);
	}
	
	Super::Deinitialize();
}

void UMonsterSpawnSubsystem::InitializeSpawnConfig(UMonsterSpawnConfig* NewConfig)
{
	if (!NewConfig)
		return;
	
	MonsterConfig = NewConfig;
	
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(SpawnTimerHandle);
		World->GetTimerManager().SetTimer(SpawnTimerHandle, this, &UMonsterSpawnSubsystem::CheckSpawnRules, 5.0f, true);
	}
}

void UMonsterSpawnSubsystem::RegisterSpawner(ATSAISpawner* Spawner)
{
	if (Spawner)
	{
		ActiveSpawners.AddUnique(Spawner);
	}
}

void UMonsterSpawnSubsystem::OnErosionChanged(float FinalValue)
{
	// 침식도가 높을수록 몬스터 수 증가
	if (FinalValue >= 90.0f)
		CurrentMaxMonsterCount = 15;	
	else if (FinalValue >= 60.0f)
		CurrentMaxMonsterCount = 10;
	else
		CurrentMaxMonsterCount = 5;
}

void UMonsterSpawnSubsystem::CheckSpawnRules()
{
	if (GetWorld()->GetNetMode() == NM_Client || !MonsterConfig)
		return;
	if (ActiveMonsters.Num() >= CurrentMaxMonsterCount)
		return;
	if (ActiveSpawners.Num() == 0)
		return;
	
	// 랜덤 스포너 선택
	int32 RandomIndex = FMath::RandRange(0, ActiveSpawners.Num() - 1);
	ATSAISpawner* TargetSpawner = ActiveSpawners[RandomIndex];
	if (!TargetSpawner)
		return;
	
	// 스포너의 RegionKey를 이용해 스폰할 몬스터 클래스 결정
	TSubclassOf<ATSAICharacter> TargetMonsterClass = MonsterConfig->GetRandomMonsterClass(TargetSpawner->RegionKey);
	
	// 키에 맞는 몬스터 설정이 없으면 스킵 (Default 키도 없을 경우)
	if (!TargetMonsterClass)
		return;
	
	// 위치 결정
	FVector SpawnLoc = TargetSpawner->GetRandomPointInVolume();
	FRotator SpawnRot = FRotator::ZeroRotator;
	
	// 풀링 처리
	ATSAICharacter* Monster = GetMonsterFromPool(TargetMonsterClass);
	
	if (!Monster)
	{
		// 풀에 없으면 새로 생성
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		
		Monster = GetWorld()->SpawnActor<ATSAICharacter>(TargetMonsterClass, SpawnLoc, SpawnRot, Params);
	}
	else
	{
		// 풀에 있으면 재활용
		Monster->SetActorLocationAndRotation(SpawnLoc, SpawnRot);
		Monster->ResetMonster();
	}
	
	if (Monster)
	{
		ActiveMonsters.Add(Monster);
	}
}

ATSAICharacter* UMonsterSpawnSubsystem::GetMonsterFromPool(TSubclassOf<ATSAICharacter> TargetClass)
{
	if (!TargetClass)
		return nullptr;
	
	// 풀을 순회하며 조건에 맞는 몬스터 찾기
	for (int32 i = 0; i < InactiveMonsterPool.Num(); i++)
	{
		ATSAICharacter* Monster = InactiveMonsterPool[i];
		
		// 몬스터가 메모리 상에 유효한지 확인
		if (IsValid(Monster))
		{
			// 내가 원하는 클래스 타입인지 확인
			if (Monster->IsA(TargetClass))
			{
				// 찾앗으면 배열에서 제거하고 반환
				InactiveMonsterPool.RemoveAtSwap(i);
				return Monster;
			}
		}
		else
		{
			// 유효하지 않은(삭제된) 몬스터는 풀에서 정리
			InactiveMonsterPool.RemoveAtSwap(i);
			i--;	// 인덱스 보정
		}
	}
	
	return nullptr;
}

void UMonsterSpawnSubsystem::ReturnMonsterInPool(ATSAICharacter* Monster)
{
	if (Monster)
	{
		ActiveMonsters.Remove(Monster);
		InactiveMonsterPool.Add(Monster);
		
		Monster->SetActorHiddenInGame(true);
		Monster->SetActorEnableCollision(false);
		Monster->SetActorTickEnabled(false);
	}
}