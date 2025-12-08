// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/IInteraction.h"
#include "Runtime/ItemInstance.h"
#include "TSInteractionActorBase.generated.h"

struct FBuildingData;
class UAbilitySystemComponent;
class UItemDataSubsystem;
class UWidgetComponent;

UCLASS()
class TINYSURVIVOR_API ATSInteractionActorBase : public AActor, public IIInteraction
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATSInteractionActorBase();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// 빌딩 데이터로 멤버 변수 업데이트
	virtual void InitializeFromBuildingData(const FBuildingData& BuildingInfo, const int32 StaticDataID);

	// 액터 내구도 업데이트
	void DamageDurability(UAbilitySystemComponent* ASC, int32 DamageAmount);

#pragma region IInteraction
	virtual void ShowInteractionWidget(ATSCharacter* InstigatorCharacter) override;
	virtual void HideInteractionWidget() override;
	virtual void SetInteractionText(FText WidgetText) override;
	virtual bool CanInteract(ATSCharacter* InstigatorCharacter) override;
	virtual void Interact(ATSCharacter* InstigatorCharacter) override;
	virtual bool RunOnServer() override;
#pragma endregion

	UPROPERTY(ReplicatedUsing = OnRep_ItemInstance, EditAnywhere, BlueprintReadWrite, Category = "Item")
	FItemInstance ItemInstance;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visual")
	UStaticMeshComponent* MeshComponent;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;
	UFUNCTION()
	void OnRep_ItemInstance();
	// 메시 초기화
	virtual void InitializeMesh(const FBuildingData& BuildingInfo);

	// 플레이어 아이템 내구도 업데이트 이벤트 발송
	void SendItemDurabilityEvent(UAbilitySystemComponent* ASC);

	TObjectPtr<UItemDataSubsystem> CachedIDS;
	int32 LastStaticDataID = 0;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction|Widget")
	TObjectPtr<UWidgetComponent> InteractionWidget;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction|Widget")
	TSubclassOf<UUserWidget> InteractionWidgetClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction|Widget")
	FText InteractionText = FText::FromString(TEXT("상호작용[E]"));
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction|Widget")
	FText DisabledText = FText::FromString(TEXT("상호작용 불가"));
};
