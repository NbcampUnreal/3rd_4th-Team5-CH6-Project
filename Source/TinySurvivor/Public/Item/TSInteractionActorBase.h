// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/IInteraction.h"
#include "Runtime/ItemInstance.h"
#include "TSInteractionActorBase.generated.h"

class UWidgetComponent;

UCLASS()
class TINYSURVIVOR_API ATSInteractionActorBase : public AActor, public IIInteraction
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATSInteractionActorBase();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

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
	
	virtual void InitializeFromItemData();
	
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
