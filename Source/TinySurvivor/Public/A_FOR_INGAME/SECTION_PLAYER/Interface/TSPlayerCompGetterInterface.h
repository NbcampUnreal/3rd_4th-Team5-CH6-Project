// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TSPlayerCompGetterInterface.generated.h"

class UTSPlayerInputActionComponent;
class UTSPlayerInteractComponent;
class UAbilitySystemComponent;
class UCharacterMovementComponent;
class UTSBodyEquipVisualComponent;
class UTSBodyInventoryComponent;
class UTSBackPackEquipVisualComponent;
class UTSBackPackInventoryComponent;
class UTSHotKeyEquipVisualComponent;
class UTSHotKeyInventoryComponent;
class USpringArmComponent;
class UCameraComponent;
class UCapsuleComponent;
class USkeletalMeshComponent;

// This class does not need to be modified.
UINTERFACE()
class UTSPlayerCompGetterInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class TINYSURVIVOR_API ITSPlayerCompGetterInterface
{
	GENERATED_BODY()
	
//----------------------------------------------------------------------------------------------------------------------

public:
	// 카메라 게터 
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "TS | Player")
	UCameraComponent* GetCameraComponent();
	
	// 스프링 암 게터 
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "TS | Player")
	USpringArmComponent* GetSpringArmComponent();

	// 캡슐 컴포넌트 
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "TS | Player")
	UCapsuleComponent* GetPlayerCapsuleComponent();
	
	// 스켈레타 메쉬 컴포넌트 
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "TS | Player")
	USkeletalMeshComponent* GetSkeletalMeshComponent();
	
	// 캐릭터 무브먼트 컴포넌트
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "TS | Player")
	UCharacterMovementComponent* GetCharacterMovementComponent();
	
//----------------------------------------------------------------------------------------------------------------------

	// GAS 컴포넌트 
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "TS | Player")
	UAbilitySystemComponent* GetAbilitySystemComponent();
	
//----------------------------------------------------------------------------------------------------------------------

	// 입력 컴포넌트 
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "TS | Player")
	UTSPlayerInputActionComponent* GetPlayerInputActionComponent();
	
//----------------------------------------------------------------------------------------------------------------------

	// 인터렉트 컴포넌트 
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "TS | Player")
	UTSPlayerInteractComponent* GetPlayerInteractComponent();
	
//----------------------------------------------------------------------------------------------------------------------
	
	// 핫 키 컴포넌트 게터 
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "TS | Player")
	UTSHotKeyInventoryComponent* GetHotKeyInventoryComponent();
	
	// 핫키 비쥬얼 컴포넌트 게터 
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "TS | Player")
	UTSHotKeyEquipVisualComponent* GetHotKeyEquipVisualComponent();
	
	// 가방 컴포넌트 게터 
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "TS | Player")
	UTSBackPackInventoryComponent* GetBackPackInventoryComponent();
	
	// 가방 비쥬얼 컴포넌트 게터 
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "TS | Player")
	UTSBackPackEquipVisualComponent* GetBackPackEquipVisualComponent();
	
	// 장비 컴포넌트 게터 
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "TS | Player")
	UTSBodyInventoryComponent* GetBodyInventoryComponent();
	
	// 장비 비쥬얼 컴포넌트 게터 
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "TS | Player")
	UTSBodyEquipVisualComponent* GetBodyEquipVisualComponent();
	
//----------------------------------------------------------------------------------------------------------------------
	
};
