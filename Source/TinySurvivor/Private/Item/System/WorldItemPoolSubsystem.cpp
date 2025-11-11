// WorldItemPoolSubsystem.cpp

#include "Item/System/WorldItemPoolSubsystem.h"
#include "Item/WorldItem.h"
#include "Item/System/WorldItemInstanceSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "ActorPool.h"
#include "PoolableActorBase.h"
#include "Item/System/TSItemPoolActor.h"

bool UWorldItemPoolSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	if (!Super::ShouldCreateSubsystem(Outer))
		return false;

	UWorld* World = Outer->GetWorld();
	if (!World)
		return false;

	if (!World->IsGameWorld())
		return false;

	// 서버에서만 생성
	return World->GetNetMode() != NM_Client;
}

void UWorldItemPoolSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// rdInst 서브시스템 캐시
	InstanceSubsystem = GetWorld()->GetSubsystem<UWorldItemInstanceSubsystem>();
}

// 생성된 액터가 자신을 Pool에 등록. ATSItemPoolActor가 BeginPlay에서 이 함수를 호출
void UWorldItemPoolSubsystem::RegisterPoolActor(ATSItemPoolActor* PoolActor)
{
	// 이미 다른 풀이 등록되었는지 확인
	if (WorldItemPool != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("UWorldItemPoolSubsystem : PoolActor is already registered"));
		return;
	}

	if (!PoolActor)
	{
		UE_LOG(LogTemp, Error, TEXT("UWorldItemPoolSubsystem : RegisterPoolActor called with null PoolActor"));
		return;
	}

	// EAP 풀을 찾았으므로, 여기서 풀을 캐시하고 초기화를 시작
	WorldItemPool = PoolActor;
	ATSItemPoolActor* TSPoolActor = PoolActor;

	if (TSPoolActor)
	{
		if (WorldItemClass == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("UWorldItemPoolSubsystem : WorldItemClass is not set in Config! Use the AWorldItem C++ class."));
			WorldItemClass = AWorldItem::StaticClass();
		}

		// BP에서 설정한 변수들을 읽어와서 풀 초기화 시작
		TSPoolActor->StartInitializePoolAsync(
			WorldItemClass,
			TSPoolActor->InitialPoolsize,
			TSPoolActor->PoolStrategy,
			TSPoolActor->BatchSize,
			TSPoolActor->BatchInterval
		);

		UE_LOG(LogTemp, Warning, TEXT("UWroldItemPoolSubsystem : ATSItemPoolActor is successfully registered and begins initialization."));
	}
}

TStatId UWorldItemPoolSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UWorldItemPoolSubsystem, STATGROUP_Tickables);
}

// EAP가 완료되었는지 확인
bool UWorldItemPoolSubsystem::IsPoolReady() const
{
	return WorldItemPool && WorldItemPool->IsInitializationComplete();
}

void UWorldItemPoolSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!IsPoolReady() || !InstanceSubsystem)
		return;

	// SwapCheckInterval마다 한 번씩만 스왑 로직 실행 (성능 최적화)
	LastSwapCheckTime += DeltaTime;
	if (LastSwapCheckTime >= SwapCheckInterval)
	{
		LastSwapCheckTime = 0.0f;
		UpdateItemSwapping();
	}
}

// 인벤토리에서 호출
bool UWorldItemPoolSubsystem::DropItem(const FSlotStructMaster& ItemData, const FTransform& Transform,
	FVector PlayerLocation)
{
	if (!IsPoolReady() || !InstanceSubsystem || ItemData.ItemData.StaticDataID <= 0)
		return false;

	float DistanceSq = FVector::DistSquared(PlayerLocation, Transform.GetLocation());

	if (DistanceSq < FMath::Square(SwapToActorDistance))
		// 거리가 가까우면 액터 스폰
		SpawnItemActor(ItemData, Transform);
	else
		// 거리가 멀면 인스턴스 스폰
		SpawnItemInstance(ItemData, Transform);

	return true;
}

// AWorldItem이 풀에 반납될 때 호출
void UWorldItemPoolSubsystem::ReleaseItemActor(AWorldItem* ActorToRelease)
{
	if (WorldItemPool && ActorToRelease)
	{
		WorldItemPool->ReleaseActor(ActorToRelease);
		ActiveWorldItems.Remove(ActorToRelease);		// 활성 목록에서 제거
	}
}

// 내부 EAP
AWorldItem* UWorldItemPoolSubsystem::SpawnItemActor(const FSlotStructMaster& ItemData, const FTransform& Transform)
{
	if (!WorldItemPool)
		return nullptr;

	
	APoolableActorBase* PooledActor = WorldItemPool->AcquireActor(0, TEXT(""), nullptr);
	AWorldItem* WorldItem = Cast<AWorldItem>(PooledActor);

	if (WorldItem)
	{
		// 위치 설정
		WorldItem->SetActorTransform(Transform, false, nullptr, ETeleportType::TeleportPhysics);
		// 데이터 주입(AWorldItem::SetItemData가 내부적으로 UpdateAppearance 호출)
		WorldItem->SetItemData(ItemData);

		ActiveWorldItems.Add(WorldItem);			// 활성 목록에 추가
	}

	return WorldItem;
}

// 내부 rdInst
void UWorldItemPoolSubsystem::SpawnItemInstance(const FSlotStructMaster& ItemData, const FTransform& Transform)
{
	if (InstanceSubsystem)
		InstanceSubsystem->AddInstance(ItemData, Transform);
}

void UWorldItemPoolSubsystem::UpdateItemSwapping()
{
	UWorld* World = GetWorld();
	if (!World || !InstanceSubsystem)
		return;
	
	// 모든 플레이어 위치를 수집
	TArray<FVector> PlayerLocations;
	for (FConstPlayerControllerIterator It = World->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PC = It->Get();
		if (PC && PC->GetPawnOrSpectator())
			PlayerLocations.Add(PC->GetPawnOrSpectator()->GetActorLocation());
	}

	// 서버에 플레이어가 한 명도 없으면 틱 중지
	if (PlayerLocations.Num() == 0)
		return;

	const float InstanceDistSq = FMath::Square(SwapToInstanceDistance);

	// Actor -> Instance 멀어짐
	TArray<AWorldItem*> ActorsToSwap;
	for (AWorldItem* ItemActor : ActiveWorldItems)	// TSet 순회
	{
		if (!ItemActor)
			continue;

		bool bIsFarFromAllPlayer = true;
		FVector ActorLocation = ItemActor->GetActorLocation();

		// 액터가 모든 플레이어와 멀리 있는지 검사
		for (const FVector& PlayerLoc : PlayerLocations)
		{
			if (FVector::DistSquared(ActorLocation, PlayerLoc) <= InstanceDistSq)
			{
				// 한 명이라도 가까이 있으면 스왑(제거)하면 안됨
				bIsFarFromAllPlayer = false;
				break;
			}
		}

		if (bIsFarFromAllPlayer)
			ActorsToSwap.Add(ItemActor);
	}

	// 수집된 목록을 스왑
	for (AWorldItem* ItemActor : ActorsToSwap)
		SwapActorToInstance(ItemActor);

	// Instance -> Actor 가까워짐
	TArray<int32> NearbyIndices;
	InstanceSubsystem->FindInstanceNear(PlayerLocations, SwapToActorDistance, NearbyIndices);

	for (int32 Index : NearbyIndices)
	{
		SwapInstanceToActor(Index);
	}
}

void UWorldItemPoolSubsystem::SwapInstanceToActor(int32 InstanceIndex)
{
	if (!InstanceSubsystem)
		return;

	FSlotStructMaster ItemData;
	FTransform InstanceTransform;

	if (InstanceSubsystem->RemoveInstance(InstanceIndex, ItemData, InstanceTransform))
		SpawnItemActor(ItemData, InstanceTransform);	// 액터 스폰
}

void UWorldItemPoolSubsystem::SwapActorToInstance(AWorldItem* ItemaActor)
{
	if (!ItemaActor || !InstanceSubsystem)
		return;

	FSlotStructMaster ItemData = ItemaActor->GetItemData();
	if (ItemData.ItemData.StaticDataID <= 0)
	{
		ReleaseItemActor(ItemaActor);	// 데이터가 없는 액터는 그냥 풀에 반납
		return;
	}

	FTransform Transform = ItemaActor->GetActorTransform();
	SpawnItemInstance(ItemData, Transform);		// 인스턴스 스폰
	ReleaseItemActor(ItemaActor);				// 액터 반납
}

