// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item/Interface/IInteraction.h"
#include "Item/Runtime/ItemInstance.h"
#include "TSBuildingActorBase.generated.h"

class UNiagaraSystem;
struct FBuildingData;
class UAbilitySystemComponent;
class UItemDataSubsystem;
class UWidgetComponent;

UCLASS()
class TINYSURVIVOR_API ATSBuildingActorBase : public AActor, public IIInteraction
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATSBuildingActorBase();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// 빌딩 데이터로 멤버 변수 업데이트
	virtual void InitializeFromBuildingData(const FBuildingData& BuildingInfo, const int32 StaticDataID);

	// 액터 내구도 업데이트
	void DamageDurability(UAbilitySystemComponent* ASC, float DamageAmount);

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
	// 태그 동기화
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_AddBlockBuildingTag();

	// 플레이어 아이템 내구도 업데이트 이벤트 발송
	void SendItemDurabilityEvent(UAbilitySystemComponent* ASC);
	
	// 태그 추가용 변수
	UPROPERTY(ReplicatedUsing = OnRep_IsBlockBuilding)
	bool bIsBlockBuilding = false;
	UFUNCTION()
	void OnRep_IsBlockBuilding();
	
	// 스폰 사운드 재생
	UFUNCTION(NetMulticast, Reliable)
	virtual void Multicast_PlaySpawnEffect() const;
	
	// 파괴 이펙트/사운드 재생
	UFUNCTION(NetMulticast, Reliable)
	virtual void Multicast_PlayDestroyEffect() const;
	
	TObjectPtr<UItemDataSubsystem> CachedIDS;
	int32 LastStaticDataID = 0;

	// 스폰 이펙트/사운드
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect|Spawn")
	TObjectPtr<UNiagaraSystem> SpawnEffect;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect|Spawn")
	TObjectPtr<USoundBase> SpawnSound;
	
	// 파괴 이펙트/사운드
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect|Destroy")
	TObjectPtr<UNiagaraSystem> DestroyEffect;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect|Destroy")
	TObjectPtr<USoundBase> DestroySound;
	
	// 상호작용 위젯
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction|Widget")
	TObjectPtr<UWidgetComponent> InteractionWidget;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction|Widget")
	TSubclassOf<UUserWidget> InteractionWidgetClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction|Widget")
	FText InteractionText = FText::FromString(TEXT("상호작용[E]"));
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction|Widget")
	FText DisabledText = FText::FromString(TEXT("상호작용 불가"));
};
