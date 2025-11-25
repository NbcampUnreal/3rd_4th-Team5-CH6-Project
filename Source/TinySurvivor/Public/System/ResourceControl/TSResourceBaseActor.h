#pragma once

#include "CoreMinimal.h"
#include "TSResourceItemInterface.h"
#include "GameFramework/Actor.h"
#include "Inventory/Struct/TSInventorySlot.h"
#include "Item/Data/ItemData.h"
#include "Item/Data/ResourceData.h"
#include "Item/Interface/IInteraction.h"
#include "TSResourceBaseActor.generated.h"

class ATSResourcePoint;
class ULootComponent;

UCLASS()
class TINYSURVIVOR_API ATSResourceBaseActor : public AActor, public IIInteraction, public ITSResourceItemInterface
{
	GENERATED_BODY()

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	

	//========================
	// ATSResourceBaseActor 라이프사이클
	//========================
	
public:
	ATSResourceBaseActor();
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	

	//========================
	// ATSResourceBaseActor 아이템 스폰  
	//========================
	
	void SetSpawnPoint(ATSResourcePoint* Point);
	void InitFromResourceData(const FResourceData& Data);
	void SetMeshComp(UStaticMesh* MeshComp);

	// 어빌리티로 트레이스 날려서 맞춘 놈이 이 액터인지 확인하고 이 API로 요청하면 됨.
	UFUNCTION(BlueprintCallable)
	virtual void GetItemFromResource(int32 RequiredToolID, FVector HitPoint, FVector HitNormal, FVector PlayerLocation, FVector ForwardVector) override;

protected:
	void SpawnItem_Internal(const FItemData& ItemDataForMesh, const FSlotStructMaster& ItemData, const FTransform& SpawnTransform);
	
	// 껍데기
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UStaticMeshComponent> ResourceStaticMeshComp = nullptr;
	
	// 이 자원의 고유 ID
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ThisResourceID = 0;
	
	// 런타임 데이터	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FResourceData ResourceRuntimeData = {};

	// 현재 가진 아이템 수량
	int32 CurrentItemCount = 0;
	
	// 아이템 스폰용 root comp
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<ULootComponent> LootComponent;
	
	// 스폰 포인트 (자신이 어디에 있는지 알기 위함)
	UPROPERTY()
	TWeakObjectPtr<ATSResourcePoint> OwningPoint;
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	

	//========================
	// ATSResourceBaseActor 인터렉션 
	//========================
	
	// IIInteraction ~
	virtual void ShowInteractionWidget(ATSCharacter* InstigatorCharacter) override;
	virtual void HideInteractionWidget() override;
	virtual void SetInteractionText(FText InteractionText) override;
	virtual bool CanInteract(ATSCharacter* InstigatorCharacter) override;
	virtual void Interact(ATSCharacter* InstigatorCharacter) override;
	virtual bool RunOnServer() override;
	// ~ IIInteraction
};
