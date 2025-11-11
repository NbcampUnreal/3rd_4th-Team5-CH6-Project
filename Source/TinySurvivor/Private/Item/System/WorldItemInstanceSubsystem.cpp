// WorldItemInstanceSubsystem.cpp


#include "Item/System/WorldItemInstanceSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "rdInst/Public/rdActor.h"
#include "Item/System/ItemDataSubsystem.h"
#include "Item/Data/ItemData.h"

// 서버에서만 서브 시스템 실행
bool UWorldItemInstanceSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	if (!Super::ShouldCreateSubsystem(Outer))
		return false;

	UWorld* World = Outer->GetWorld();
	if (!World || !World->IsGameWorld())
		return false;

	// 서버에서만 생성
	return World->GetNetMode() != NM_Client;
}

void UWorldItemInstanceSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UWorldItemInstanceSubsystem::RegisterInstanceActor(ArdActor* InstanceActor)
{
	if (ItemInstanceManager != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("UWorldItemInstanceSubsystem : InstanceActor is already registered"));
		return;
	}

	if (!InstanceActor)
	{
		UE_LOG(LogTemp, Error, TEXT("UWorldItemInstanceSubsystem : RegisterInstanceActor was called with a Null InstanceActor"));
		return;
	}

	// ArdActor를 찾았으므로, 여기서 변수에 캐시
	ItemInstanceManager = InstanceActor;
}

// ID로 메시를 동기식 로드
UStaticMesh* UWorldItemInstanceSubsystem::GetMeshFromID(UWorld* World, int32 ItemID)
{
	if (!World || ItemID <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("GetMeshFromID : World or ItemID is invalid"));
		return nullptr;	
	}

	UGameInstance* GameInstance = World->GetGameInstance();
	if (!GameInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("GetMeshFromID : GameInstance is not valid"));
		return nullptr;	
	}

	UItemDataSubsystem* ItemDataSubsystem = GameInstance->GetSubsystem<UItemDataSubsystem>();
	if (!ItemDataSubsystem || !ItemDataSubsystem->IsInitialized())
	{
		UE_LOG(LogTemp, Warning, TEXT("GetMeshFromID : ItemDataSubsystem is not valid or not initialized"));
		return nullptr;	
	}

	FItemData StaticItemData;
	if (ItemDataSubsystem->GetItemDataSafe(ItemID, StaticItemData) && StaticItemData.IsWorldMeshValid())
	{
		return StaticItemData.WorldMesh.LoadSynchronous();	
	}

	UE_LOG(LogTemp, Warning, TEXT("GetMeshFromID : Can't find mesh for ID '%d'"), ItemID);
	return nullptr;
}

// rdAddInstance를 호출
int32 UWorldItemInstanceSubsystem::AddInstance(const FSlotStructMaster& ItemData, const FTransform& Transform)
{
	if (!ItemInstanceManager || ItemData.StaticDataID <= 0)
		return -1;

	// ID로 UStaticMesh를 가져옵니다
	UStaticMesh* Mesh = GetMeshFromID(GetWorld(), ItemData.StaticDataID);
	if (!Mesh)
	{
		UE_LOG(LogTemp, Warning, TEXT("AddInstance : Failed to load mesh for ID '%d'"), ItemData.StaticDataID);
		return -1;
	}

	// rdAddInstance를 호출
	int32 NewIndex = ItemInstanceManager->rdAddInstance(Mesh, Transform);

	if (NewIndex != -1)
		// 성공 시 인스턴스 맵에 등록
		InstanceDataMap.Add(NewIndex, ItemData);

	return NewIndex;
}

// rdRemoveInstance를 호출
bool UWorldItemInstanceSubsystem::RemoveInstance(int32 InstanceIndex, FSlotStructMaster& OutItemData, FTransform& OutTransform)
{
	if (!ItemInstanceManager)
		return false;

	FSlotStructMaster* FoundData = InstanceDataMap.Find(InstanceIndex);
	if (!FoundData)
		return false;

	UStaticMesh* Mesh = GetMeshFromID(GetWorld(), FoundData->StaticDataID);
	if (!Mesh)
	{
		UE_LOG(LogTemp, Warning, TEXT("RemoveInstance : Mesh not found for ID '%d'(index '%d'. Removing from map"), FoundData->StaticDataID, InstanceIndex);
		InstanceDataMap.Remove(InstanceIndex);
		// ItemInstanceManager->rdRemoveInstanceX(const FName sid, int32 index);
		return false;
	}

	// ArdActor의 rdGetInstanceTransform을 호출하여 Transform을 가져옵니다
	bool bSuccess = ItemInstanceManager->rdGetInstanceTransform(Mesh, InstanceIndex, OutTransform);

	if (!bSuccess)
		UE_LOG(LogTemp, Warning, TEXT("RemoveInstance : rdGetInstanceTransform failed for index '%d'"), InstanceIndex);

	// 맵에서 데이터를 제거
	OutItemData = *FoundData;
	InstanceDataMap.Remove(InstanceIndex);
	
	// rdInst에서 데이터 제거(실패했을 경우에도 제거)
	ItemInstanceManager->rdRemoveInstance(Mesh, InstanceIndex);

	return bSuccess;
}

// Actor 근처의 인스턴스를 찾는 기능(엔진의 공간 쿼리(Sphere Sweep) 사용)
void UWorldItemInstanceSubsystem::FindInstanceNear(const TArray<FVector>& Locations, float Radius, TArray<int32>& OutInstanceIndices)
{
	// OutInstanceIndices.Empty();
	
	UWorld* World = GetWorld();
	if(!World || Locations.Num() == 0)
		return;

	TArray<FHitResult> Hits;
	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(FindRdInstItemss), false, nullptr);

	const ECollisionChannel CollisionChannel = ECC_Visibility;

	// 모든 플레이어 위치에 대해 스피어 스윕(Sphere Sweep)을 실행
	for (const FVector& Loc : Locations)
	{
		World->SweepMultiByChannel(
			Hits,
			Loc,	// 시작 위치
			Loc,	// 끝 위치
			FQuat::Identity,
			CollisionChannel,
			FCollisionShape::MakeSphere(Radius),	// 검색 반경
			QueryParams
		);
	}

	if (Hits.Num() > 0)
	{
		// 중복된 인덱스 제거를 위해 TSet을 사용
		TSet<int32> FoundIndices;

		for (const FHitResult& Hit : Hits)
		{
			// 찾은 것이 Instanced Static Mesh(ISM)가 맞는지 확인
			UInstancedStaticMeshComponent* HitISM = Cast<UInstancedStaticMeshComponent>(Hit.GetComponent());

			// ISM이 'rdInst' 플러그인이 관리하는 ISM인지 확인 
			if (HitISM)
			{
				// 부딪힌 인스턴스의 인덱스를 가져옴
				int32 InstanceIndex = Hit.Item;

				// 이 인덱스가 맵에 있는지 확인
				if (InstanceDataMap.Contains(InstanceIndex))
					FoundIndices.Add(InstanceIndex);
			}
		}
		// TSet의 결과를 OutInstanceIndices에 복사
		OutInstanceIndices = FoundIndices.Array();
	}
}