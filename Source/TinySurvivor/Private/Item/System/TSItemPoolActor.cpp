// TSItemPoolActor.cpp

#include "Item/System/TSItemPoolActor.h"
#include "Item/System/WorldItemInstanceSubsystem.h"
#include "Item/System/WorldItemPoolSubsystem.h"

ATSItemPoolActor::ATSItemPoolActor()
{
    bReplicates = true;
    bAlwaysRelevant = true;
}

UHierarchicalInstancedStaticMeshComponent* ATSItemPoolActor::GetOrCreateHISM(UStaticMesh* Mesh)
{
    if (!Mesh) return nullptr;

    if (HISMComponents.Contains(Mesh))
    {
        return HISMComponents[Mesh];
    }

    FName CompName = MakeUniqueObjectName(this, UHierarchicalInstancedStaticMeshComponent::StaticClass(), Mesh->GetFName());
    UHierarchicalInstancedStaticMeshComponent* HISM = NewObject<UHierarchicalInstancedStaticMeshComponent>(this, CompName);
    
    HISM->SetStaticMesh(Mesh);
    HISM->SetCollisionProfileName(FName("BlockAllDynamic")); 
    // HISM->SetCullDistances(0, 0); // 필요 시 활성화
    HISM->RegisterComponent();
    HISM->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);

    HISMComponents.Add(Mesh, HISM);
    return HISM;
}

int32 ATSItemPoolActor::AddInstanceNative(UStaticMesh* Mesh, const FTransform& Transform)
{
    UHierarchicalInstancedStaticMeshComponent* HISM = GetOrCreateHISM(Mesh);
    if (HISM)
    {
        int32 Index = HISM->AddInstanceWorldSpace(Transform);
        HISM->MarkRenderStateDirty();
        return Index;
    }
    return -1;
}

// 인스턴스를 삭제하지 않고 트랜스폼만 조작하여 숨김/보임 처리
bool ATSItemPoolActor::SetInstanceVisibleNative(UStaticMesh* Mesh, int32 InstanceIndex, bool bVisible, const FTransform& NewTransform)
{
    if (HISMComponents.Contains(Mesh))
    {
        auto* HISM = HISMComponents[Mesh];
        if (HISM && HISM->IsValidInstance(InstanceIndex))
        {
            FTransform TargetTransform;

            if (bVisible)
            {
                // 보이게 할 때는 원래 위치로 복구
                TargetTransform = NewTransform;
                
                // 혹시 스케일이 0으로 들어오면 1로 강제 복구
                if (TargetTransform.GetScale3D().IsZero())
                {
                    TargetTransform.SetScale3D(FVector::OneVector);
                }
            }
            else
            {
                // 숨길 때는 지하 깊숙이, 크기 0
                TargetTransform = FTransform(FRotator::ZeroRotator, FVector(0, 0, -50000.0f), FVector::ZeroVector);
            }

            // 트랜스폼 업데이트
            HISM->UpdateInstanceTransform(InstanceIndex, TargetTransform, true, true, true);

            // 다시 보이게 할 때는 바운드(Bounds)를 강제로 다시 계산해야 함
            if (bVisible)
            {
                HISM->UpdateBounds();
                HISM->MarkRenderStateDirty(); // 렌더링 상태 강제 갱신
            }
            
            return true;
        }
    }
    return false;
}

bool ATSItemPoolActor::GetInstanceTransformNative(UStaticMesh* Mesh, int32 InstanceIndex, FTransform& OutTransform)
{
    if (HISMComponents.Contains(Mesh))
    {
        auto* HISM = HISMComponents[Mesh];
        if (HISM && HISM->IsValidInstance(InstanceIndex))
        {
            return HISM->GetInstanceTransform(InstanceIndex, OutTransform, true);
        }
    }
    return false;
}

void ATSItemPoolActor::Multicast_AddInstance_Implementation(const FSlotStructMaster& ItemData, const FTransform& Transform)
{
    // 클라이언트는 Subsystem을 통해 데이터와 인스턴스를 동기화
    if (auto* Sys = GetWorld()->GetSubsystem<UWorldItemInstanceSubsystem>())
    {
        Sys->AddInstance(ItemData, Transform);
    }
}

void ATSItemPoolActor::Multicast_SetInstanceVisible_Implementation(int32 InstanceIndex, int32 StaticItemID, bool bVisible, const FTransform& NewTransform)
{
    if (auto* Sys = GetWorld()->GetSubsystem<UWorldItemInstanceSubsystem>())
    {
        // 클라이언트 가시성 동기화 호출
        Sys->SetInstanceVisible_Direct(InstanceIndex, StaticItemID, bVisible, NewTransform);
    }
}

void ATSItemPoolActor::Client_SyncAllInstances_Implementation(const TArray<FSlotStructMaster>& DataList, const TArray<FTransform>& TransformList)
{
    if (auto* InstanceSys = GetWorld()->GetSubsystem<UWorldItemInstanceSubsystem>())
    {
        for (int32 i=0; i<DataList.Num(); ++i)
        {
            InstanceSys->AddInstance(DataList[i], TransformList[i]);
        }
    }
}

void ATSItemPoolActor::BeginPlay()
{
    Super::BeginPlay();

    // 서버에서만 실행
    if (HasAuthority())
    {
        UWorld* World = GetWorld();
        if (World)
        {
            // 인스턴스 서브시스템에 등록 (HISM 렌더링 담당)
            auto* InstanceSys = World->GetSubsystem<UWorldItemInstanceSubsystem>();
            if (InstanceSys)
            {
                InstanceSys->RegisterPoolActor(this);
            }

            // 풀 서브시스템에도 등록 (액터 풀링 담당)
            auto* PoolSys = World->GetSubsystem<UWorldItemPoolSubsystem>();
            if (PoolSys)
            {
                PoolSys->RegisterPoolActor(this);
            }
        }
    }
}