// TSItemPoolActor.h

#pragma once

#include "CoreMinimal.h"
#include "ActorPool.h"
#include "Inventory/Struct/TSInventorySlot.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "TSItemPoolActor.generated.h"

class UTSInventorySlot;

UCLASS()
class TINYSURVIVOR_API ATSItemPoolActor : public AActorPool
{
    GENERATED_BODY()

public:
    ATSItemPoolActor();
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pool Setting", meta = (ClampMin = "1"))
    int32 InitialPoolsize = 50;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pool Setting")
    EPoolStrategy PoolStrategy = EPoolStrategy::CreateNew;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pool Setting", meta = (ClampMin = "1"))
    int32 BatchSize = 5;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pool Setting", meta = (ClampMin = "0.01"))
    float BatchInterval = 0.1f;
    
    UPROPERTY()
    TMap<UStaticMesh*, UHierarchicalInstancedStaticMeshComponent*> HISMComponents;
    
    // 인스턴스 추가 (인덱스 반환)
    int32 AddInstanceNative(UStaticMesh* Mesh, const FTransform& Transform);
    
    // 인스턴스 가시성 토글 (삭제 대신 사용)
    bool SetInstanceVisibleNative(UStaticMesh* Mesh, int32 InstanceIndex, bool bVisible, const FTransform& NewTransform);

    // 트랜스폼 가져오기
    bool GetInstanceTransformNative(UStaticMesh* Mesh, int32 InstanceIndex, FTransform& OutTransform);
    
    // 추가
    UFUNCTION(NetMulticast, Reliable)
    void Multicast_AddInstance(const FSlotStructMaster& ItemData, const FTransform& Transform);

    // 가시성 변경
    UFUNCTION(NetMulticast, Reliable)
    void Multicast_SetInstanceVisible(int32 InstanceIndex, int32 StaticItemID, bool bVisible, const FTransform& NewTransform);

    // 전체 동기화
    UFUNCTION(Client, Reliable)
    void Client_SyncAllInstances(const TArray<FSlotStructMaster>& DataList, const TArray<FTransform>& TransformList);

private:
    UHierarchicalInstancedStaticMeshComponent* GetOrCreateHISM(UStaticMesh* Mesh);
    
protected:
    virtual void BeginPlay() override;
};