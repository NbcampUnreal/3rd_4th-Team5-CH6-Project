// WorldItemInstanceSubsystem.h

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Inventory/Struct/TSInventorySlot.h"
#include "WorldItemInstanceSubsystem.generated.h"

class ATSItemPoolActor;

UCLASS()
class TINYSURVIVOR_API UWorldItemInstanceSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    UFUNCTION(BlueprintCallable, Category = "Item Instance")
    void RegisterPoolActor(ATSItemPoolActor* PoolActor);
    
    // 인스턴스 추가
    UFUNCTION(BlueprintCallable, Category = "Item Instance")
    int32 AddInstance(const FSlotStructMaster& ItemData, const FTransform& Transform);

    // 인스턴스를 삭제하지 않고 숨김 처리 (Actor로 스왑될 때)
    bool HideInstanceForSwap(int32 InstanceIndex, FSlotStructMaster& OutItemData, FTransform& OutTransform);

    // 숨겨진 인스턴스를 다시 보임 처리 (Actor가 멀어져서 돌아올 때)
    void ShowInstanceFromSwap(int32 InstanceIndex, const FSlotStructMaster& ItemData, const FTransform& Transform);

    // 아이템 획득 시 영구적 제거 (실제로는 숨기고 데이터 완전 삭제)
    void RemoveInstancePermanent(int32 InstanceIndex);

    // Client RPC 수신용 가시성 제어
    void SetInstanceVisible_Direct(int32 InstanceIndex, int32 ItemID, bool bVisible, const FTransform& Transform);

    void FindInstanceNear(const TArray<FVector>& Locations, float Radius, TArray<int32>& OutInstanceIndices);
    void GetAllInstsnceData(TArray<FSlotStructMaster>& OutItemData, TArray<FTransform>& OutTransforms);
    void HandleInitialChunkData(const TArray<FSlotStructMaster>& ChunkData, const TArray<FTransform>& ChunkTransforms);
    
    bool GetItemDataByInstanceIndex(int32 InstanceIndex, FSlotStructMaster& OutData) const;

protected:
    UPROPERTY()
    TObjectPtr<ATSItemPoolActor> MyPoolActor;
    
    // 현재 인스턴스 상태로 존재하는 아이템 데이터
    UPROPERTY()
    TMap<int32, FSlotStructMaster> InstanceDataMap;

    // 인스턴스의 원래 위치를 기억하는 맵 (숨겨졌을 때 GetTransform 하면 지하 좌표가 나오므로 필요)
    UPROPERTY()
    TMap<int32, FTransform> CachedTransforms;

private:
    UStaticMesh* GetMeshFromID(UWorld* World, int32 ItemID);
};