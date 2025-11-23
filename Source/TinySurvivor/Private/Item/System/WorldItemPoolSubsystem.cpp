// WorldItemPoolSubsystem.cpp
/*
	===============================================================================
	[ FILE MODIFICATION NOTICE - DECAY SYSTEM INTEGRATION ]
	작성자: 양한아
	날짜: 2025/11/21
	
	본 파일에는 '부패(Decay) 시스템'을 통합하기 위한 변경이 포함되어 있습니다.
	해당 변경들은 모두 아래 표기된 주석 블록 내부에 위치합니다:
	
		// ■ Decay
		//[S]=====================================================================================
			(Decay 관련 통합 코드)
		//[E]=====================================================================================
		
	위 영역 외의 기존 풀링/스폰/인스턴싱 로직은 변경하지 않았습니다.
	Decay 시스템만 연동한 최소 변경입니다.
	후속 작업 시 해당 블록을 참고해주세요.
	===============================================================================
*/

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

	FSlotStructMaster NewItemData = ItemData;
	
	if (NewItemData.ItemData.CreationServerTime <= 0.1f)
	{
		NewItemData.ItemData.CreationServerTime = GetWorld()->GetTimeSeconds();	
	}
	
	float DistanceSq = FVector::DistSquared(PlayerLocation, Transform.GetLocation());

	if (DistanceSq < FMath::Square(SwapToActorDistance))
	{
		// 거리가 가까우면 액터 스폰
		SpawnItemActor(NewItemData, Transform);
	}
	else
	{
		// 거리가 멀면 인스턴스 스폰
		SpawnItemInstance(NewItemData, Transform);
	}
	
	return true;
}

// AWorldItem이 풀에 반납될 때 호출
void UWorldItemPoolSubsystem::ReleaseItemActor(AWorldItem* ActorToRelease)
{
	if (WorldItemPool && ActorToRelease)
	{
		// 풀이 꽉 차서 OnRelease를 호출 안 해줄 수도 있으므로,
		// 여기서 강제로 먼저 안 보이게 만들고 충돌을 끔
		ActorToRelease->SetActorHiddenInGame(true);
		ActorToRelease->SetActorEnableCollision(false);
		ActorToRelease->SourceInstanceIndex = -1;
       
		// 물리 시뮬레이션 등이 켜져 있었다면 끄기
		if (UPrimitiveComponent* Root = Cast<UPrimitiveComponent>(ActorToRelease->GetRootComponent()))
		{
			Root->SetSimulatePhysics(false);
		}

		// 그 다음 풀에 반납 시도 (풀이 받아주면 OnRelease가 또 호출되겠지만, 중복 호출되어도 상관없음)
		WorldItemPool->ReleaseActor(ActorToRelease);
       
		// 관리 목록에서 제거
		ActiveWorldItems.Remove(ActorToRelease); 
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
		//WorldItem->SetItemData(ItemData);
		// ■ Decay
		//[S]=====================================================================================
		// ItemData의 CreationServerTime이 없으면 현재 시각으로 설정
		FSlotStructMaster ItemDataCopy = ItemData;
		if (ItemDataCopy.ItemData.CreationServerTime <= 0)
		{
			ItemDataCopy.ItemData.CreationServerTime = GetWorld()->GetTimeSeconds();
		}
		WorldItem->SetItemData(ItemDataCopy);
		//[E]=====================================================================================
		
		ActiveWorldItems.Add(WorldItem);			// 활성 목록에 추가
	}

	return WorldItem;
}

// 내부 rdInst
void UWorldItemPoolSubsystem::SpawnItemInstance(const FSlotStructMaster& ItemData, const FTransform& Transform)
{
	if (WorldItemPool)
	{
		if (ATSItemPoolActor* TSPool = Cast<ATSItemPoolActor>(WorldItemPool))
		{
			TSPool->Multicast_AddInstance(ItemData, Transform);
		}
	}
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
	if (!InstanceSubsystem) return;

	FSlotStructMaster ItemData;
	FTransform InstanceTransform;

	// 인스턴스 숨기기 & 데이터 가져오기
	if (InstanceSubsystem->HideInstanceForSwap(InstanceIndex, ItemData, InstanceTransform))
	{
		// 액터 스폰
		AWorldItem* SpawnedActor = SpawnItemActor(ItemData, InstanceTransform);
		
		if (SpawnedActor)
		{
			SpawnedActor->SourceInstanceIndex = InstanceIndex; 
		}
       
		// RPC로 클라에게도 숨기라고 명령
		if (WorldItemPool)
		{
			if (ATSItemPoolActor* TSPool = Cast<ATSItemPoolActor>(WorldItemPool))
			{
				TSPool->Multicast_SetInstanceVisible(InstanceIndex, ItemData.ItemData.StaticDataID, false, InstanceTransform);
			}
		}
	}
}

void UWorldItemPoolSubsystem::SwapActorToInstance(AWorldItem* ItemaActor)
{
	if (!ItemaActor) return;

	FSlotStructMaster ItemData = ItemaActor->GetItemData();
	FTransform Transform = ItemaActor->GetActorTransform();
	int32 SrcIndex = ItemaActor->SourceInstanceIndex; // 출처 인덱스 가져오기
	
	if (ItemData.ItemData.StaticDataID <= 0)
	{
		ReleaseItemActor(ItemaActor);
		return;
	}
    
	// 원래 인스턴스에서 온 놈이면 복구
	if (SrcIndex != -1 && InstanceSubsystem)
	{
		// Add가 아니라 Show (복구)
		InstanceSubsystem->ShowInstanceFromSwap(SrcIndex, ItemData, Transform);
        
		// RPC로 클라이언트들에게도 가시성 복구 명령
		if (ATSItemPoolActor* TSPool = Cast<ATSItemPoolActor>(WorldItemPool))
		{
			TSPool->Multicast_SetInstanceVisible(SrcIndex, ItemData.ItemData.StaticDataID, true, Transform);
		}
	}
	else
	{
		// 인스턴스 출신이 아니면(그냥 드랍된거면) 새로 생성
		SpawnItemInstance(ItemData, Transform);
	}

	ReleaseItemActor(ItemaActor);
}

