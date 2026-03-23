// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "A_FOR_INGAME/SECTION_INTERACT/Interface/TSInteractInterface.h"
#include "A_FOR_INGAME/SECTION_ITEM/Inventory/Inventory/Interface/ForOwner/TSInventoryOwnerActionInterface.h"
#include "TSPlayerCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UTSPlayerInputActionComponent;
class UTSPlayerInteractComponent;
/**
 * 
 */
UCLASS()
class TINYSURVIVOR_API ATSPlayerCharacter : public ACharacter, public ITSInteractInterface, public ITSInventoryOwnerActionInterface

{
	GENERATED_BODY()
	
//======================================================================================================================	
#pragma region 라이프_사이클
	
	
	//━━━━━━━━━━━━━━━━━━━━
	// 라이프 사이클
	//━━━━━━━━━━━━━━━━━━━━	

public:
	ATSPlayerCharacter();
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	
#pragma endregion
//======================================================================================================================	
#pragma region 인벤토리_API_및_컴포넌트
	
	
	//━━━━━━━━━━━━━━━━━━━━
	// 인벤토리 API 및 컴포넌트
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
#pragma region 인터렉트API_컴포넌트_데이터
	
	
	//━━━━━━━━━━━━━━━━━━━━
	// 인터렉트API_컴포넌트_데이터
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
#pragma region 입력_컴포넌트들
	
	
	//━━━━━━━━━━━━━━━━━━━━
	// 입력_컴포넌트들
	//━━━━━━━━━━━━━━━━━━━━	
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TS | Player")
	TObjectPtr<UTSPlayerInputActionComponent> PlayerInputHandleComponent = nullptr;
	
#pragma endregion
//======================================================================================================================	
#pragma region 카메라와_스프링암
	
	
	//━━━━━━━━━━━━━━━━━━━━
	// 입력_컴포넌트들
	//━━━━━━━━━━━━━━━━━━━━	
	
protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TS | Player")
	TObjectPtr<USpringArmComponent> SpringArmComponent = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TS | Player")
	TObjectPtr<UCameraComponent> CameraComponent = nullptr;
	
#pragma endregion
//======================================================================================================================		
};
