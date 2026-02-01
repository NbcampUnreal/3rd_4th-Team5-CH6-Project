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
    
    // 초기 풀 생성 시 만들 액터의 총 개수
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pool Setting", meta = (ClampMin = "1", DisplayName = "초기 풀 크기"))
    int32 InitialPoolsize = 50;
    // 풀(Pool)의 아이템이 부족할 때 어떻게 처리할지 결정하는 전략 (CreateNew: 새로 생성, RecycleOldest: 오래된 것 재사용)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pool Setting", meta = (DisplayName = "풀 전략"))
    EPoolStrategy PoolStrategy = EPoolStrategy::CreateNew;
    // 비동기 초기화 시, 한 번에(한 프레임에) 생성할 액터의 개수 (렉 방지용)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pool Setting", meta = (ClampMin = "1", DisplayName = "배치 크기 (Batch Size)"))
    int32 BatchSize = 5;
    // 비동기 초기화 시, 몇 초마다 생성 작업을 수행할지 설정
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pool Setting", meta = (ClampMin = "0.01", DisplayName = "배치 간격 (Batch Interval)"))
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