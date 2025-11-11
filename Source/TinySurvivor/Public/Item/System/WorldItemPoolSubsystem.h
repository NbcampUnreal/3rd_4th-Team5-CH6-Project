// WorldItemPoolSubsystem.h

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Inventory/Struct/TSInventorySlot.h"
#include "WorldItemPoolSubsystem.generated.h"

class AActorPool;
class AWorldItem;
class UWorldItemInstanceSubsystem;
class ATSItemPoolActor;

UCLASS(Config = Game)
class TINYSURVIVOR_API UWorldItemPoolSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual TStatId GetStatId() const override;
	virtual void Tick(float DeltaTime) override;

	// 인벤토리 등 다른 시스템이 호출
	
	// 인벤토리에서 아이템을 버릴 때 호출
	UFUNCTION(BlueprintCallable, Category = "Item Pool")
	bool DropItem(const FSlotStructMaster& ItemData, const FTransform& Transform, FVector PlayerLocation);
	// AWorldItem이 주워졌을 때, 자신을 풀에 반납하기 위해 호출
	UFUNCTION(BlueprintCallable, Category = "Item Pool")
	void ReleaseItemActor(AWorldItem* ActorToRelease);
	// Actor가 생성되었을 때, 스스로를 등록하도록 호출
	UFUNCTION(BlueprintCallable, Category = "Item Pool")
	void RegisterPoolActor(ATSItemPoolActor* PoolActor);
protected:
	// Actor Pooling/rdInst 처리 함수

	// 풀에서 AWorldItem 액터를 꺼내 스폰
	AWorldItem* SpawnItemActor(const FSlotStructMaster& ItemData, const FTransform& Transform);
	// rdInst 서브시스템에 가짜 메시 인스턴스 스폰을 요청
	void SpawnItemInstance(const FSlotStructMaster& ItemData, const FTransform& Transform);

	// Actor Pooling 콜백

	// 비동기 풀 생성이 완료되었을 때 호출될 콜백
	void OnPoolInitialized();
	// 풀이 비동기 초기화를 완료했는지 여부
	bool IsPoolReady() const;

	// Actor Pooling/rdInst 스왑 로직

	// 매 틱마다 플레이어 주변의 액터와 인스턴스를 검사하여 스왑을 실행
	void UpdateItemSwapping();
	// 감지된 가까운 rdInst 인스턴스를 진짜 AWorldItem 액터로 교체
	void SwapInstanceToActor(int32 InstanceIndex);
	// 감지된 멀어진 AWorldItem 액터를 가짜 rdInst 인스턴스로 교체
	void SwapActorToInstance(AWorldItem* ItemaActor);

	// EAP가 스폰할 AWorldItem의 블루프린트 클래스를 지정
	UPROPERTY(Config, EditDefaultsOnly, Category = "Item Pool Config")
	TSubclassOf<AWorldItem> WorldItemClass;
	// Actor Pooling 플러그인의 액터 풀 객체
	UPROPERTY()
	TObjectPtr<AActorPool> WorldItemPool;
	// rdInst 관리 서브시스템의 캐시된 포인터
	UPROPERTY()
	TObjectPtr<UWorldItemInstanceSubsystem> InstanceSubsystem;
	// 활성 액터 추적 Set
	UPROPERTY()
	TSet<TObjectPtr<AWorldItem>> ActiveWorldItems;
	// 이 값보다 가까우면 Actor Pooling 액터로 스폰/스왑
	UPROPERTY(Config, EditDefaultsOnly, Category = "Item Pool Config")
	float SwapToActorDistance = 500.0f; // 5m
	// 이 값보다 멀면 rdInst 인스턴스로 스폰/스왑
	UPROPERTY(Config, EditDefaultsOnly, Category = "Item Pool Config")
	float SwapToInstanceDistance = 600.0f; // 6m
	
	// 스왑 로직 성능 최적화를 위한 틱 조절
	float LastSwapCheckTime = 0.0f;
	UPROPERTY(Config, EditDefaultsOnly, Category = "Item Pool Config")
	float SwapCheckInterval = 0.5f;
};
