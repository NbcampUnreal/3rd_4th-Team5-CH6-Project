// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "A_FOR_INGAME/SECTION_INTERACT/Interface/TSInteractInterface.h"
#include "A_FOR_INGAME/SECTION_ITEM/Inventory/Inventory/Interface/ForOwner/TSInventoryOwnerActionInterface.h"
#include "A_FOR_INGAME/SECTION_PLAYER/TSPlayerCompGetterInterface.h"
#include "TSPlayerCharacter.generated.h"

class UTSGiveGAGEDataAsset;
class UCameraComponent;
class USpringArmComponent;
class UTSPlayerInputActionComponent;
class UTSPlayerInteractComponent;
/**
 * 
 */
UCLASS()
class TINYSURVIVOR_API ATSPlayerCharacter : public ACharacter, public ITSInteractInterface, public ITSInventoryOwnerActionInterface, public IAbilitySystemInterface, public ITSPlayerCompGetterInterface

{
	GENERATED_BODY()
	
//======================================================================================================================	
#pragma region 게터
	
	//━━━━━━━━━━━━━━━━━━━━
	// 게터
	//━━━━━━━━━━━━━━━━━━━━	
	
public:
	
	// ITSPlayerCompGetterInterface ~ 
	FORCEINLINE virtual UCameraComponent* GetCameraComponent_Implementation() const override { return CameraComponent;}
	FORCEINLINE virtual USpringArmComponent* GetSpringArmComponent_Implementation() const override { return SpringArmComponent;}
	FORCEINLINE virtual UTSHotKeyInventoryComponent* GetHotKeyInventoryComponent_Implementation() const override { return HotKeyInventoryComponent;}
	FORCEINLINE virtual UTSHotKeyEquipVisualComponent* GetHotKeyEquipVisualComponent_Implementation() const override { return HotKeyEquipVisualComponent; }
	FORCEINLINE virtual UTSBackPackInventoryComponent* GetBackPackInventoryComponent_Implementation() const override { return BackpackInventoryComponent;}
	FORCEINLINE virtual UTSBackPackEquipVisualComponent* GetBackPackEquipVisualComponent_Implementation() const override { return BackpackEquipVisualComponent;}
	FORCEINLINE virtual UTSBodyInventoryComponent* GetBodyInventoryComponent_Implementation() const override {return EquipmentInventoryComponent; }
	FORCEINLINE virtual UTSBodyEquipVisualComponent* GetBodyEquipVisualComponent_Implementation() const override { return EquipVisualComponent; }
	// ~ ITSPlayerCompGetterInterface
	
#pragma endregion
//======================================================================================================================	
#pragma region 라이프_사이클
	
	
	//━━━━━━━━━━━━━━━━━━━━
	// 라이프 사이클
	//━━━━━━━━━━━━━━━━━━━━	

public:
	ATSPlayerCharacter();
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	
#pragma endregion
//======================================================================================================================	
#pragma region 인게임_사이클
	
	
	//━━━━━━━━━━━━━━━━━━━━
	// 인게임 사이클
	//━━━━━━━━━━━━━━━━━━━━	
	
public:
	void SubscribeInGameCycleDelegate();
	void UnSubsceceToPlayerState();
	
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
	// ITSInteractInterface ~ 
	virtual void ToggleInteractWidget_Implementation(bool InWantOn) override { return; /*아무것도 안 함*/ };
	FORCEINLINE virtual ETSPlayRole GetPlayRole_Implementation() override { return PlayRole;}
	// ~ ITSInteractInterface
	
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
