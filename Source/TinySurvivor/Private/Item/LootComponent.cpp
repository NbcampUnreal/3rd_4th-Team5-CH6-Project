// LootComponent.cpp

#include "Item/LootComponent.h"
#include "Item/System/WorldItemPoolSubsystem.h"
#include "Kismet/GameplayStatics.h"

ULootComponent::ULootComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


bool ULootComponent::SpawnLoot(FVector TargetLocation)
{
	if (!GetOwner()->HasAuthority())
		return false;
	
	UWorld* World = GetWorld();
	auto* PoolSys = World ? World->GetSubsystem<UWorldItemPoolSubsystem>() : nullptr;
	if (!PoolSys)
	{
		UE_LOG(LogTemp, Error, TEXT("LootComponent : PoolSubsystem not found"));
		return false;
	}
	
	FVector OriginLocation = GetOwner()->GetActorLocation();
	
	// 타겟 위치 결정
	FVector FinalTargetloc = TargetLocation;
	if (FinalTargetloc.IsZero())
	{
		FinalTargetloc = GetClosestPlayerLocation(OriginLocation);
	}
	
	for (const FLootRule& Rule : LootTable)
	{
		if (Rule.ItemID <= 0)
			continue;
		
		if (FMath::FRand() <= Rule.DropChance)
		{
			int32 CountToSpawn = FMath::RandRange(Rule.MinCount, Rule.MaxCount);
			
			for (int32 i = 0; i < CountToSpawn; i++)
			{
				FSlotStructMaster NewItem;
				NewItem.ItemData.StaticDataID = Rule.ItemID;
				
				// AddToTranslation 대신 매번 원점에서 새로 계산
				FVector RandomOffset = FMath::VRand() * FMath::RandRange(0.0f, ScatterRadius);
				RandomOffset.Z = 50.0f;
				
				PoolSys->DropItem(NewItem, FTransform(OriginLocation + RandomOffset), TargetLocation);
			}
		}
	}
	
	return true;
}

bool ULootComponent::SpawnHarvestLoot(FVector TargetLocation, FVector SpawnOriginLocation)
{
	if (!GetOwner()->HasAuthority())
		return false;
	
	UWorld* World = GetWorld();
	auto* PoolSys = World ? World->GetSubsystem<UWorldItemPoolSubsystem>() : nullptr;
	if (!PoolSys)
		return false;
	
	FVector OriginLocation = SpawnOriginLocation;
	if (OriginLocation.IsZero())
	{
		OriginLocation = GetOwner()->GetActorLocation();
	}
	
	FVector FinalTargetLoc = TargetLocation;
	if (FinalTargetLoc.IsZero())
		FinalTargetLoc = GetClosestPlayerLocation(OriginLocation);
	
	// LootTable 순회 (Main[0], Sub[1])
	for (const FLootRule& Rule : LootTable)
	{
		if (Rule.ItemID <= 0)
			continue;
		
		// 확률 체크 (Main은 1.0이라 무조건 통과, Sub는 확률에 따라)
		if (FMath::FRand() <= Rule.DropChance)
		{
			// 개수 결정 (Min ~ Max)
			int32 CountToSpawn = FMath::RandRange(Rule.MinCount, Rule.MaxCount);
			
			for (int32 i = 0; i < CountToSpawn; i++)
			{
				FSlotStructMaster NewItem;
				NewItem.ItemData.StaticDataID = Rule.ItemID;
				
				FVector Direction = (FinalTargetLoc - OriginLocation).GetSafeNormal();
				FVector RandomDir = (Direction + FMath::VRand()).GetSafeNormal();
				FVector RandomOffset = RandomDir * FMath::RandRange(50.0f, ScatterRadius);
				RandomOffset.Z = 50.0f;
				
				PoolSys->DropItem(NewItem, FTransform(OriginLocation + RandomOffset), FinalTargetLoc);
			}
		}
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