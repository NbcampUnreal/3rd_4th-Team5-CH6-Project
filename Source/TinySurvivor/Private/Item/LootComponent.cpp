// LootComponent.cpp

#include "Item/LootComponent.h"
#include "Item/System/WorldItemPoolSubsystem.h"
#include "Kismet/GameplayStatics.h"

ULootComponent::ULootComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void ULootComponent::SpawnLoot()
{
	// 서버에서만 실행
	if (!GetOwner()->HasAuthority()) return;

	UWorld* World = GetWorld();
	if (!World) return;

	// 풀 서브시스템 가져오기
	auto* PoolSys = World->GetSubsystem<UWorldItemPoolSubsystem>();
	if (!PoolSys) 
	{
		UE_LOG(LogTemp, Error, TEXT("LootComponent: PoolSubsystem Not Found!"));
		return;
	}

	FVector CenterLoc = GetOwner()->GetActorLocation();

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
                
				FTransform SpawnTrans(CenterLoc + RandomOffset);
				
				PoolSys->DropItem(NewItem, SpawnTrans, CenterLoc);
			}
		}
	}
}