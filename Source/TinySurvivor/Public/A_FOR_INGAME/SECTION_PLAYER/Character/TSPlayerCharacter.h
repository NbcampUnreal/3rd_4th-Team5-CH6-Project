// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "A_FOR_INGAME/SECTION_INTERACT/Interface/TSCommonInteractInterface.h"
#include "A_FOR_INGAME/SECTION_ITEM/Inventory/Inventory/Interface/ForOwner/TSInventoryOwnerActionInterface.h"
#include "A_FOR_INGAME/SECTION_PLAYER/Interface/TSPlayerCompGetterInterface.h"
#include "A_FOR_INGAME/SECTION_SAVELOAD/Data/Struct/TSSaveMasterData.h"
#include "TSPlayerCharacter.generated.h"

enum class ETSInGameCycleMode : uint8;
class UTSGiveGAGEDataAsset;
class UCameraComponent;
class USpringArmComponent;
class UTSPlayerInputActionComponent;
class UTSPlayerInteractComponent;
/**
 * 
 */
UCLASS()
class TINYSURVIVOR_API ATSPlayerCharacter : public ACharacter, public ITSCommonInteractInterface, public ITSInventoryOwnerActionInterface, public IAbilitySystemInterface, public ITSPlayerCompGetterInterface

{
	GENERATED_BODY()
	
//======================================================================================================================	
#pragma region 게터
	
	//━━━━━━━━━━━━━━━━━━━━
	// 게터
	//━━━━━━━━━━━━━━━━━━━━	
	
public:
	
	// ITSPlayerCompGetterInterface ~ 
	
	// 카메라, 스프링암, 캡슐, 스켈레탈, CMC
	FORCEINLINE virtual UCameraComponent* GetCameraComponent_Implementation()  override { return CameraComponent;}
	FORCEINLINE virtual USpringArmComponent* GetSpringArmComponent_Implementation()  override { return SpringArmComponent;}
	FORCEINLINE virtual UCapsuleComponent* GetPlayerCapsuleComponent_Implementation()  override { return GetCapsuleComponent(); }
	FORCEINLINE virtual USkeletalMeshComponent* GetSkeletalMeshComponent_Implementation() override { return GetMesh();}
	FORCEINLINE virtual UCharacterMovementComponent* GetCharacterMovementComponent_Implementation() override { return GetCharacterMovement(); }
	
	// GAS
	FORCEINLINE virtual UAbilitySystemComponent* GetAbilitySystemComponent_Implementation() override { return GetAbilitySystemComponent() ;}
	
	// 입력
	FORCEINLINE virtual UTSPlayerInputActionComponent* GetPlayerInputActionComponent_Implementation()  override { return PlayerInputHandleComponent;}
	
	// 인터렉트
	FORCEINLINE virtual UTSPlayerInteractComponent* GetPlayerInteractComponent_Implementation()  override { return PlayerInteractComponent;}

	// 인벤토리 및 비쥬얼
	FORCEINLINE virtual UTSHotKeyInventoryComponent* GetHotKeyInventoryComponent_Implementation()  override { return HotKeyInventoryComponent;}
	FORCEINLINE virtual UTSHotKeyEquipVisualComponent* GetHotKeyEquipVisualComponent_Implementation()  override { return HotKeyEquipVisualComponent; }
	FORCEINLINE virtual UTSBackPackInventoryComponent* GetBackPackInventoryComponent_Implementation()  override { return BackpackInventoryComponent;}
	FORCEINLINE virtual UTSBackPackEquipVisualComponent* GetBackPackEquipVisualComponent_Implementation()  override { return BackpackEquipVisualComponent;}
	FORCEINLINE virtual UTSBodyInventoryComponent* GetBodyInventoryComponent_Implementation()  override {return EquipmentInventoryComponent; }
	FORCEINLINE virtual UTSBodyEquipVisualComponent* GetBodyEquipVisualComponent_Implementation()  override { return EquipVisualComponent; }
	
	// ~ ITSPlayerCompGetterInterface
	
#pragma endregion
//======================================================================================================================	
#pragma region 라이프_사이클
	
	
	//━━━━━━━━━━━━━━━━━━━━
	// 라이프 사이클
	//━━━━━━━━━━━━━━━━━━━━	

public:
	ATSPlayerCharacter();
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
#pragma endregion
//======================================================================================================================	
#pragma region 인게임_사이클
	
	
	//━━━━━━━━━━━━━━━━━━━━
	// 인게임 사이클
	//━━━━━━━━━━━━━━━━━━━━	
	
public:
	void SubscribeInGameCycleDelegate();
	void UnSubscribeToPlayerState();
	
	// 인 게임 사이클 델리게이트 바인딩 함수 
	UFUNCTION()
	void OnReceivedInGameCycleDelegate_internal(ETSInGameCycleMode InGameCycleMode, FTSSaveMasterData& InData);
	
	// new 호출 시
	void CallWhenNewModeIsCalled_internal();
	
	// load 호출 시
	void CallWhenLoadModeIsCalled_internal(FTSSaveMasterData& InData);
	
	// play 호출 시 
	void CallWhenPlayModeIsCalled_internal();
	
#pragma endregion
//======================================================================================================================	
#pragma region GAS_섹션
	//━━━━━━━━━━━━━━━━━━━━
	// GAS
	//━━━━━━━━━━━━━━━━━━━━

protected:
	// IAbilitySystemInterface ~ 
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;;
	// ~ IAbilitySystemInterface
	
	// GAS 관련 초기화 -> GA 부여, GE 부여 -> 초기화 완료 알림
	virtual void InitGAAndSendInitComplete_internal(UAbilitySystemComponent* InASC, APlayerController* InPC);
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = " TS | Player")
	TObjectPtr<UTSGiveGAGEDataAsset> BaseGAGEData = nullptr;
	
#pragma endregion
//======================================================================================================================	
#pragma region 인벤토리_섹션
	
	
	//━━━━━━━━━━━━━━━━━━━━
	// 인벤토리_섹션
	//━━━━━━━━━━━━━━━━━━━━
	
public:
	
	// ITSNewInventoryOwnerActionInterface ~
	FORCEINLINE virtual UTSHotKeyInventoryComponent* GetHotKeyComponent_Implementation() override { return HotKeyInventoryComponent;}
	FORCEINLINE virtual UTSBackPackInventoryComponent* GetBackpackComponent_Implementation() override { return BackpackInventoryComponent;}
	FORCEINLINE virtual UTSBodyInventoryComponent* GetEquipmentComponent_Implementation() override {return EquipmentInventoryComponent;}
	FORCEINLINE virtual UTSFuelProductInventoryComponent* GetFuelComponent_Implementation() override {return nullptr;}	// 안씀
	FORCEINLINE virtual UTSStorageInventoryComponent* GetStorageComponent_Implementation() override {return nullptr;}	// 안씀
	// ~ ITSNewInventoryOwnerActionInterface
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TS | Player")
	TObjectPtr<UTSHotKeyInventoryComponent> HotKeyInventoryComponent = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TS | Player")
	TObjectPtr<UTSBackPackInventoryComponent> BackpackInventoryComponent = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TS | Player")
	TObjectPtr<UTSBodyInventoryComponent> EquipmentInventoryComponent = nullptr;
	
#pragma endregion
//======================================================================================================================
#pragma region 인벤토리_비쥬얼_섹션	
	
	//━━━━━━━━━━━━━━━━━━━━
	// 인벤토리_비쥬얼_섹션
	//━━━━━━━━━━━━━━━━━━━━	
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TS | Player")
	TObjectPtr<UTSHotKeyEquipVisualComponent> HotKeyEquipVisualComponent = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TS | Player")
	TObjectPtr<UTSBackPackEquipVisualComponent> BackpackEquipVisualComponent = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TS | Player")
	TObjectPtr<UTSBodyEquipVisualComponent> EquipVisualComponent = nullptr;
	
#pragma endregion
//======================================================================================================================	
#pragma region 인터렉트_섹션
	
	
	//━━━━━━━━━━━━━━━━━━━━
	// 인터렉트_섹션
	//━━━━━━━━━━━━━━━━━━━━
		
public:
	// ITSCommonInteractInterface ~ 
	virtual void ToggleInteractWidget_Implementation(bool InWantOn) override { return; /*아무것도 안 함*/ };
	FORCEINLINE virtual ETSPlayRole GetPlayRole_Implementation() override { return PlayRole;}
	// ~ ITSCommonInteractInterface
	
protected:
	// 컴포넌트
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TS | Player")
	TObjectPtr<UTSPlayerInteractComponent> PlayerInteractComponent;
	
	// 롤 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TS | Player")
	ETSPlayRole PlayRole = ETSPlayRole::Player;

#pragma endregion
//======================================================================================================================	
#pragma region 입력_섹션
	
	
	//━━━━━━━━━━━━━━━━━━━━
	// 입력_섹션
	//━━━━━━━━━━━━━━━━━━━━	
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TS | Player")
	TObjectPtr<UTSPlayerInputActionComponent> PlayerInputHandleComponent = nullptr;
	
#pragma endregion
//======================================================================================================================	
#pragma region 카메라와_스프링암
	
	
	//━━━━━━━━━━━━━━━━━━━━
	// 카메라와_스프링암
	//━━━━━━━━━━━━━━━━━━━━	
	
protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TS | Player")
	TObjectPtr<USpringArmComponent> SpringArmComponent = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TS | Player")
	TObjectPtr<UCameraComponent> CameraComponent = nullptr;
	
#pragma endregion
//======================================================================================================================	
	
	
	
	
	
	
};
