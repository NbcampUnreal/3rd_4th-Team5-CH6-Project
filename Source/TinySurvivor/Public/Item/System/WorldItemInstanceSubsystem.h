// WorldItemInstanceSubsystem.h

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Inventory/Struct/TSInventorySlot.h"
#include "WorldItemInstanceSubsystem.generated.h"

class ArdActor;

UCLASS()
class TINYSURVIVOR_API UWorldItemInstanceSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	// 서버에서만 생성
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	// 레벨에 배치된 ArdActor를 찾아 캐시
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	// 레벨에 배치된 ArdActor가 이 함수를 호출해서 자신을 등록
	UFUNCTION(BlueprintCallable, Category = "Item Instance")
	void RegisterInstanceActor(ArdActor* InstanceActor);
	
	// rdInst 인스턴스 추가
	int32 AddInstance(const FSlotStructMaster& ItemData, const FTransform& Transform);
	// rdInst 인스터스 제거 및 데이터/트랜스폼 반환
	bool RemoveInstance(int32 InstanceIndex, FSlotStructMaster& OutItemData, FTransform& OutTransform);
	// 특정 위치 주변의 인스턴스 인덱스 검색
	void FindInstanceNear(const TArray<FVector>& Locations, float Radius, TArray<int32>& OutInstanceIndices);

protected:
	// 레벨에 배치된 rdInst의 메인 액터 (본체)
	UPROPERTY()
	TObjectPtr<ArdActor> ItemInstanceManager;
	// rdInst aoq 인덱스(key)/아이템 데이터(Value)
	UPROPERTY()
	TMap<int32, FSlotStructMaster> InstanceDataMap;

private:
	// ID로 메시를 동기 로드하는 내부 헬퍼 함수
	UStaticMesh* GetMeshFromID(UWorld* World, int32 ItemID);
};
