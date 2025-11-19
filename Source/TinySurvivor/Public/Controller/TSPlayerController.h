// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TSPlayerController.generated.h"

/**
 * ATSPlayerController
 *
 * TinySurvivor 프로젝트의 플레이어 컨트롤러
 * - HUD 및 UI 위젯 관리
 * - 인벤토리/컨테이너 상호작용 처리
 * - 건설 모드 및 설정 UI 제어
 * - 컨테이너 거리 체크 및 자동 닫기
 */

enum class EInventoryType : uint8;
class ATSCharacter;

UCLASS()
class TINYSURVIVOR_API ATSPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	//~=============================================================================
	// Network RPC
	//~=============================================================================

	/** 컨테이너와 플레이어 간 아이템 전송 (서버 권한) */
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Inventory")
	void ServerTransferItem(
		AActor* SourceActor,
		AActor* TargetActor,
		EInventoryType FromInventoryType,
		int32 FromSlotIndex,
		EInventoryType ToInventoryType,
		int32 ToSlotIndex,
		bool bIsFullStack = true);

	//~=============================================================================
	// UI Management
	//~=============================================================================

	/** HUD 위젯 초기화 */
	UFUNCTION(BlueprintCallable, Category = "UI")
	void InitializePlayerHUD();

	/** 캐릭터 데이터로 HUD 업데이트 */
	UFUNCTION(BlueprintCallable, Category = "UI")
	void UpdateHUDWithCharacter(ATSCharacter* TSCharacter);

	/** 인벤토리 UI 토글 */
	UFUNCTION(BlueprintCallable, Category = "UI")
	void ToggleInventory();

	/** 컨테이너 UI 토글 */
	UFUNCTION(BlueprintCallable, Category = "UI")
	void ToggleContainer(AActor* ContainerActor);

	/** 건설 모드 UI 토글 */
	UFUNCTION(BlueprintCallable, Category = "UI")
	void ToggleBuildingMode();

	/** ESC 키 입력 처리 */
	UFUNCTION(BlueprintCallable, Category = "UI")
	void HandleEscapeKey();

	/** 설정 UI 열기 */
	UFUNCTION(BlueprintCallable, Category = "UI")
	void OpenSettings();

	/** 설정 UI 닫기 */
	UFUNCTION(BlueprintCallable, Category = "UI")
	void CloseSettings();

	/** 현재 열린 컨테이너 닫기 */
	UFUNCTION(BlueprintCallable, Category = "UI")
	void CloseCurrentContainer();

protected:
	//~=============================================================================
	// Lifecycle Overrides
	//~=============================================================================

	virtual void AcknowledgePossession(class APawn* P) override;
	virtual void OnUnPossess() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	//~=============================================================================
	// UI Components
	//~=============================================================================

	/** HUD 위젯 클래스 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> HUDWidgetClass;

	/** 생성된 HUD 위젯 인스턴스 */
	UPROPERTY(BlueprintReadOnly, Category = "UI")
	UUserWidget* HUDWidget;

	//~=============================================================================
	// Container Management
	//~=============================================================================

	/** 현재 열린 컨테이너 */
	UPROPERTY(BlueprintReadOnly, Category = "Container")
	AActor* CurrentContainer;

	/** 컨테이너 거리 체크 */
	UFUNCTION()
	void CheckContainerDistance();

	/** HUD 업데이트 플래그 */
	bool IsUpdated = false;

private:
	//~=============================================================================
	// Widget Index Enums
	//~=============================================================================

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

	//~=============================================================================
	// Helper Functions
	//~=============================================================================

	/** 모든 게임 UI 닫기 */
	void CloseAllGameUI();

	/** 입력 모드 업데이트 (UI/게임 모드 전환) */
	void UpdateInputMode();

	/** UI가 하나라도 열려있는지 확인 */
	bool IsAnyUIOpen() const;

	//~=============================================================================
	// Container Configuration
	//~=============================================================================

	/** 컨테이너 거리 체크 타이머 */
	FTimerHandle ContainerDistanceCheckTimer;

	/** 컨테이너 상호작용 최대 거리 */
	UPROPERTY(EditDefaultsOnly, Category = "Container")
	float MaxContainerInteractionDistance = 300.0f;

	/** 거리 체크 간격 (초) */
	UPROPERTY(EditDefaultsOnly, Category = "Container")
	float DistanceCheckInterval = 0.5f;

	//~=============================================================================
	// UI State
	//~=============================================================================

	/** 설정 UI 열림 상태 */
	bool bIsSettingsOpen = false;
};
