// LootComponent.cpp

#include "Item/LootComponent.h"
#include "Item/System/WorldItemPoolSubsystem.h"
#include "Kismet/GameplayStatics.h"

ULootComponent::ULootComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool ULootComponent::SpawnLoot(FTransform& SpawnTransform, const FVector& PlayerLocation, int32& LootNumForResource)
{
	// 서버에서만 실행
	if (!GetOwner()->HasAuthority()) 
		return false;

	UWorld* World = GetWorld();
	if (!World) 
		return false;

	// 풀 서브시스템 가져오기
	auto* PoolSys = World->GetSubsystem<UWorldItemPoolSubsystem>();
	if (!PoolSys) 
	{
		UE_LOG(LogTemp, Error, TEXT("LootComponent: PoolSubsystem Not Found!"));
		return false;
	}
	
	// 기준 위치
	FVector OriginLocation = GetOwner()->GetActorLocation();
	// 가장 가까운 플레이어 찾기
	FVector TargetPlayerLoc = GetClosestPlayerLocation(OriginLocation);
	
	// 드랍 테이블 순회하며 확률 계산
	for (const FLootRule& Rule : LootTable)
	{
		// ID 유효성 체크
		if (Rule.ItemID <= 0) 
			continue;

		// 확률 체크
		if (FMath::FRand() <= Rule.DropChance)
		{
			// 개수 결정
			int32 CountToSpawn = FMath::RandRange(Rule.MinCount, Rule.MaxCount);
			
			// 자원 원천인 경우 뱉어내는 아이템 수 만큼 보유 수량을 깎아주기
			LootNumForResource -= CountToSpawn;

			for (int32 i = 0; i < CountToSpawn; i++)
			{
				// 아이템 데이터 구성
				FSlotStructMaster NewItem;
				NewItem.ItemData.StaticDataID = Rule.ItemID;

				// 위치 약간 흩뿌리기
				FVector RandomOffset = FMath::VRand() * FMath::RandRange(0.0f, ScatterRadius);
				RandomOffset.Z = 50.0f; // 바닥에서 살짝 위
				
				SpawnTransform.AddToTranslation(RandomOffset);
                
				FTransform SpawnTrans(SpawnTransform);
				
				bool SpawnSuccess = PoolSys->DropItem(NewItem, SpawnTrans, TargetPlayerLoc);
				if (!SpawnSuccess) 
					return false;
			}
		}
	}
	return true;
}

bool ULootComponent::SpawnSpecificLoot(int32 ItemID, int32 Count)
{
	if (!GetOwner()->HasAuthority() || Count <= 0 || ItemID <= 0) 
		return false;
	
	UWorld* World = GetWorld();
	auto* PoolSys = World ? World->GetSubsystem<UWorldItemPoolSubsystem>() : nullptr;
	if (!PoolSys)
		return false;
	
	// 기준 위치
	FVector OriginLocation = GetOwner()->GetActorLocation();
	// 가장 가까운 플레이어 찾기
	FVector TargetPlayerLoc = GetClosestPlayerLocation(OriginLocation);
	
	for (int32 i = 0; i < Count; i++)
	{
		FSlotStructMaster NewItem;
		NewItem.ItemData.StaticDataID = ItemID;
		
		// 위치 약간 흩뿌리기
		FVector RandomOffset = FMath::VRand() * FMath::RandRange(0.0f, ScatterRadius);
		RandomOffset.Z = 50.0f; // 바닥에서 살짝 위
		
		PoolSys->DropItem(NewItem, FTransform(OriginLocation + RandomOffset), TargetPlayerLoc);
	}
	
	return true;
}

// 가장 가까운 플레이어 찾기 (헬퍼 함수)
FVector ULootComponent::GetClosestPlayerLocation(const FVector& OriginOriginLocation) const
{
	UWorld* World = GetWorld();
	if (!World)
		// 월드 없으면 기준 위치 반환
		return OriginOriginLocation;
	
	FVector ClosestLoc = OriginOriginLocation;
	float MinDistSq = FLT_MAX;
	
	for (FConstPlayerControllerIterator Iterator = World->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* PC = Iterator->Get();
		if (PC && PC->GetPawn())
		{
			FVector PlayerPos = PC->GetPawn()->GetActorLocation();
			float DistSq = FVector::DistSquared(OriginOriginLocation, PlayerPos);
			
			if (DistSq < MinDistSq)
			{
				MinDistSq = DistSq;
				ClosestLoc = PlayerPos;
			}
		}
	}
	
	return ClosestLoc;
}