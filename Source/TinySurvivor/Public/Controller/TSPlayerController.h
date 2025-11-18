#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TSPlayerController.generated.h"

enum class EInventoryType : uint8;
class ATSCharacter;

UCLASS()
class TINYSURVIVOR_API ATSPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	// 컨테이너 상호작용용 RPC
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Inventory")
	void ServerTransferItem(
	AActor* SourceActor,      // 출발 액터 (플레이어 또는 컨테이너)
	AActor* TargetActor,
		EInventoryType FromInventoryType,
		int32 FromSlotIndex,
		EInventoryType ToInventoryType,
		int32 ToSlotIndex,
		bool bIsFullStack = true);
	UFUNCTION(BlueprintCallable)
	void InitializePlayerHUD();

	UFUNCTION(BlueprintCallable)
	void UpdateHUDWithCharacter(ATSCharacter* TSCharacter);
	
	UFUNCTION(BlueprintCallable)
	void ToggleInventory();
	UFUNCTION(BlueprintCallable)
	void ToggleContainer(AActor* ContainerActor);
	UFUNCTION(BlueprintCallable)
	void ToggleBuildingMode();
	UFUNCTION(BlueprintCallable)
	void HandleEscapeKey();
	
	UFUNCTION(BlueprintCallable, Category = "UI")
	void OpenSettings();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void CloseSettings();
	
protected:

	virtual void AcknowledgePossession(class APawn* P) override;
	virtual void OnUnPossess() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	UPROPERTY(BlueprintReadOnly, Category = "Container")
	AActor* CurrentContainer;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> HUDWidgetClass;
	
	UPROPERTY(BlueprintReadOnly, Category = "UI")
	UUserWidget* HUDWidget;
	
	bool IsUpdated = false;
	
	UFUNCTION()
	void CheckContainerDistance();


	
private:
	FTimerHandle ContainerDistanceCheckTimer;
	// 컨테이너 상호작용 최대 거리
	UPROPERTY(EditDefaultsOnly, Category = "Container")
	float MaxContainerInteractionDistance = 300.0f;

	// 거리 체크 간격 (초)
	UPROPERTY(EditDefaultsOnly, Category = "Container")
	float DistanceCheckInterval = 0.5f;
	
	bool bIsSettingsOpen = false;
	
	enum EBackpackWidgetIndex
	{
		Empty_Backpack = 0,
		PlayerInventory = 1
	};

	enum EContentWidgetIndex
	{
		Empty_Content = 0,
		Container = 1,
		BuildingMode = 2,
		CraftingMode = 3,
		Settings = 4
	};
	
	void CloseCurrentContainer();
	void CloseAllGameUI();
	void UpdateInputMode();
	bool IsAnyUIOpen() const;
	
};
