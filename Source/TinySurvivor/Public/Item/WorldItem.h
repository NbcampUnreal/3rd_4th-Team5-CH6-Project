// WorldItem.h

#pragma once

#include "CoreMinimal.h"
#include "PoolableActorBase.h"
#include "Inventory/Struct/TSInventorySlot.h"
#include "Engine/StreamableManager.h"
#include "WorldItem.generated.h"

class UStaticMeshComponent;
class USphereComponent;

UCLASS()
class TINYSURVIVOR_API AWorldItem : public APoolableActorBase
{
	GENERATED_BODY()

public:
	AWorldItem();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* InteractionCollision;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_ItemData, Category = "Item")
	FSlotStructMaster ItemData;

	// 비동기 메시 로딩을 위한 핸들
	TSharedPtr<FStreamableHandle> MeshLoadHandle;
	
	UFUNCTION()
	void OnRep_ItemData();
	UFUNCTION()
	void OnInteractionOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnInteractionEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	void UpdateAppearance();

public:
	// 풀에서 액터 꺼낼 때
	virtual void OnAcquire_Implementation(const int32& IntParam, const FString& StringParam, const UObject* ObjectParam) override;
	// 풀로 액터를 반납할 때
	virtual void OnRelease_Implementation() override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	void SetItemData(const FSlotStructMaster& NewItemData);
	const FSlotStructMaster& GetItemData() const { return ItemData; }
};
