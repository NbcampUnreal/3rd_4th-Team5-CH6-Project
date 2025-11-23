// WorldItemInstanceSubsystem.cpp

#include "Item/System/WorldItemInstanceSubsystem.h"
#include "Engine/OverlapResult.h"
#include "Kismet/GameplayStatics.h"
#include "Item/System/TSItemPoolActor.h"
#include "Item/System/ItemDataSubsystem.h"
#include "Item/Data/ItemData.h"

bool UWorldItemInstanceSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
    if (!Super::ShouldCreateSubsystem(Outer)) return false;
    UWorld* World = Outer->GetWorld();
    return World && World->IsGameWorld();
}

void UWorldItemInstanceSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
}

void UWorldItemInstanceSubsystem::RegisterPoolActor(ATSItemPoolActor* PoolActor)
{
    if (MyPoolActor) return;
    MyPoolActor = PoolActor;
}

UStaticMesh* UWorldItemInstanceSubsystem::GetMeshFromID(UWorld* World, int32 ItemID)
{
    if (!World || ItemID <= 0) return nullptr;
    UGameInstance* GI = World->GetGameInstance();
    if (!GI) return nullptr;
    UItemDataSubsystem* DataSys = GI->GetSubsystem<UItemDataSubsystem>();
    if (!DataSys || !DataSys->IsInitialized()) return nullptr;

    FItemData Data;
    if (DataSys->GetItemDataSafe(ItemID, Data) && Data.IsWorldMeshValid())
    {
        return Data.WorldMesh.LoadSynchronous(); 
    }
    return nullptr;
}

int32 UWorldItemInstanceSubsystem::AddInstance(const FSlotStructMaster& ItemData, const FTransform& Transform)
{
    if (!MyPoolActor)
    {
        AActor* FoundActor = UGameplayStatics::GetActorOfClass(GetWorld(), ATSItemPoolActor::StaticClass());
        if (FoundActor)
        {
            MyPoolActor = Cast<ATSItemPoolActor>(FoundActor);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("[Fail] CRITICAL ERROR: BP_ItemPoolManager is NOT in the level!"));
            return -1;
        }
    }

    if (ItemData.ItemData.StaticDataID <= 0) 
    {
        UE_LOG(LogTemp, Error, TEXT("[Fail] Invalid Item ID: %d"), ItemData.ItemData.StaticDataID);
        return -1;
    }

    UStaticMesh* Mesh = GetMeshFromID(GetWorld(), ItemData.ItemData.StaticDataID);
    if (!Mesh) 
    {
        UE_LOG(LogTemp, Error, TEXT("[Fail] Mesh Not Found for ID: %d. Check ItemDataTable!"), ItemData.ItemData.StaticDataID);
        return -1;
    }
    
    int32 NewIndex = MyPoolActor->AddInstanceNative(Mesh, Transform);

    if (NewIndex != -1)
    {
        InstanceDataMap.Add(NewIndex, ItemData);
        CachedTransforms.Add(NewIndex, Transform); // 캐싱
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("[Fail] AddInstanceNative returned -1"));
    }
    
    return NewIndex;
}

// Actor로 변환하기 위해 인스턴스를 숨김
bool UWorldItemInstanceSubsystem::HideInstanceForSwap(int32 InstanceIndex, FSlotStructMaster& OutItemData, FTransform& OutTransform)
{
    if (!MyPoolActor) return false;

    FSlotStructMaster* FoundData = InstanceDataMap.Find(InstanceIndex);
    if (!FoundData) return false;

    OutItemData = *FoundData;

    // 캐시된 위치가 있으면 그걸 사용, 없으면 Native 호출
    // 근데 숨겨져 있으면 엉뚱한 값일 수 있음
    if (FTransform* Cached = CachedTransforms.Find(InstanceIndex))
    {
        OutTransform = *Cached;
    }
    else
    {
        // 예외 처리
        UStaticMesh* Mesh = GetMeshFromID(GetWorld(), OutItemData.ItemData.StaticDataID);
        MyPoolActor->GetInstanceTransformNative(Mesh, InstanceIndex, OutTransform);
    }

    UStaticMesh* Mesh = GetMeshFromID(GetWorld(), OutItemData.ItemData.StaticDataID);

    // 가시성 false (지하로 보냄)
    MyPoolActor->SetInstanceVisibleNative(Mesh, InstanceIndex, false, OutTransform);

    // Map에서 제거하여 FindInstanceNear 검색에 걸리지 않게 함.
    // 하지만 HISM 인덱스는 유지됨.
    InstanceDataMap.Remove(InstanceIndex);

    return true;
}

// Actor가 사라지고 다시 인스턴스로 복귀
void UWorldItemInstanceSubsystem::ShowInstanceFromSwap(int32 InstanceIndex, const FSlotStructMaster& ItemData, const FTransform& Transform)
{
    if (!MyPoolActor) return;

    UStaticMesh* Mesh = GetMeshFromID(GetWorld(), ItemData.ItemData.StaticDataID);
    
    // 가시성 true (원래 위치로 복구)
    if (MyPoolActor->SetInstanceVisibleNative(Mesh, InstanceIndex, true, Transform))
    {
        InstanceDataMap.Add(InstanceIndex, ItemData);
        CachedTransforms.Add(InstanceIndex, Transform);
    }
}

// 아이템 획득 시 (영구 삭제)
void UWorldItemInstanceSubsystem::RemoveInstancePermanent(int32 InstanceIndex)
{
    // 데이터 맵에서 삭제하여 로직상 없는 존재로 만듦
    if (InstanceDataMap.Contains(InstanceIndex))
    {
        FSlotStructMaster Data = InstanceDataMap[InstanceIndex];
        InstanceDataMap.Remove(InstanceIndex);
        CachedTransforms.Remove(InstanceIndex);
        
        // 시각적으로 숨김 (영구)
        UStaticMesh* Mesh = GetMeshFromID(GetWorld(), Data.ItemData.StaticDataID);
        if(MyPoolActor)
        {
            MyPoolActor->SetInstanceVisibleNative(Mesh, InstanceIndex, false, FTransform::Identity);
        }
    }
}

// Client RPC Receiver
void UWorldItemInstanceSubsystem::SetInstanceVisible_Direct(int32 InstanceIndex, int32 ItemID, bool bVisible, const FTransform& Transform)
{
    if (!MyPoolActor) return;
    UStaticMesh* Mesh = GetMeshFromID(GetWorld(), ItemID);

    MyPoolActor->SetInstanceVisibleNative(Mesh, InstanceIndex, bVisible, Transform);

    // 클라이언트도 데이터 맵 싱크를 맞춤
    if (!bVisible)
    {
        InstanceDataMap.Remove(InstanceIndex);
    }
}

void UWorldItemInstanceSubsystem::FindInstanceNear(const TArray<FVector>& Locations, float Radius, TArray<int32>& OutInstanceIndices)
{
    UWorld* World = GetWorld();
    if(!World || Locations.Num() == 0) return;
    
    TArray<FOverlapResult> Overlaps;
    FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(FindRdInstItems), false, nullptr);
    const ECollisionChannel CollisionChannel = ECC_Visibility;
    FCollisionShape SphereShape = FCollisionShape::MakeSphere(Radius);
    
    for (const FVector& Loc : Locations)
    {
       // DrawDebugSphere(World, Loc, Radius, 16, FColor::Green, false, 0.1f);
       World->OverlapMultiByChannel(Overlaps, Loc, FQuat::Identity, CollisionChannel, SphereShape, QueryParams);
    }
    
    if (Overlaps.Num() > 0)
    {
       TSet<int32> FoundIndices;
       for (const FOverlapResult& Result : Overlaps)
       {
          if (UInstancedStaticMeshComponent* HitISM = Cast<UInstancedStaticMeshComponent>(Result.GetComponent()))
          {
             int32 InstanceIndex = Result.ItemIndex;
             // 숨겨진 아이템(Actor상태)은 InstanceDataMap에 없으므로 여기서 걸러짐
             if (InstanceDataMap.Contains(InstanceIndex))
             {
                FoundIndices.Add(InstanceIndex);
             }
          }
       }
       OutInstanceIndices = FoundIndices.Array();
    }
}

void UWorldItemInstanceSubsystem::GetAllInstsnceData(TArray<FSlotStructMaster>& OutItemData, TArray<FTransform>& OutTransforms)
{
    OutItemData.Empty();
    OutTransforms.Empty();
    
    if (!MyPoolActor) return;
    
    for (const auto& Pair : InstanceDataMap)
    {
       int32 InstanceIndex = Pair.Key;
       const FSlotStructMaster& Data = Pair.Value;
       
       // CachedTransforms에서 가져오는 것이 안전함
       if (CachedTransforms.Contains(InstanceIndex))
       {
           OutItemData.Add(Data);
           OutTransforms.Add(CachedTransforms[InstanceIndex]);
       }
    }
}

void UWorldItemInstanceSubsystem::HandleInitialChunkData(const TArray<FSlotStructMaster>& ChunkData, const TArray<FTransform>& ChunkTransforms)
{
    if (ChunkData.Num() != ChunkTransforms.Num()) return;
    for (int32 i = 0; i < ChunkData.Num(); i++)
    {
       AddInstance(ChunkData[i], ChunkTransforms[i]);
    }
}

bool UWorldItemInstanceSubsystem::GetItemDataByInstanceIndex(int32 InstanceIndex, FSlotStructMaster& OutData) const
{
    if (const FSlotStructMaster* Found = InstanceDataMap.Find(InstanceIndex))
    {
       OutData = *Found;
       return true;
    }
    return false;
}