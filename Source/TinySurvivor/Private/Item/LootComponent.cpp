// LootComponent.cpp

#include "Item/LootComponent.h"
#include "Item/System/WorldItemPoolSubsystem.h"
#include "Kismet/GameplayStatics.h"

ULootComponent::ULootComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool ULootComponent::SpawnLoot(FTransform& SpawnTransform, const FVector& PlayerLocation)
{
	// 서버에서만 실행
	if (!GetOwner()->HasAuthority()) return false;

	UWorld* World = GetWorld();
	if (!World) return false;

	// 풀 서브시스템 가져오기
	auto* PoolSys = World->GetSubsystem<UWorldItemPoolSubsystem>();
	if (!PoolSys) 
	{
		UE_LOG(LogTemp, Error, TEXT("LootComponent: PoolSubsystem Not Found!"));
		return false;
	}

	// 드랍 테이블 순회하며 확률 계산
	for (const FLootRule& Rule : LootTable)
	{
		// ID 유효성 체크
		if (Rule.ItemID <= 0) continue;

		// 확률 체크
		if (FMath::FRand() <= Rule.DropChance)
		{
			// 개수 결정
			int32 CountToSpawn = FMath::RandRange(Rule.MinCount, Rule.MaxCount);

			for (int32 i = 0; i < CountToSpawn; i++)
			{
				// 아이템 데이터 구성
				FSlotStructMaster NewItem;
				NewItem.ItemData.StaticDataID = Rule.ItemID;
				// NewItem.Amount = 1;

				// 위치 약간 흩뿌리기
				FVector RandomOffset = FMath::VRand() * FMath::RandRange(0.0f, ScatterRadius);
				RandomOffset.Z = 50.0f; // 바닥에서 살짝 위
				
				SpawnTransform.AddToTranslation(RandomOffset);
                
				FTransform SpawnTrans(SpawnTransform);
				
				bool SpawnSuccess = PoolSys->DropItem(NewItem, SpawnTrans, PlayerLocation);
				if (!SpawnSuccess) return false;
			}
		}
	}
	return true;
}