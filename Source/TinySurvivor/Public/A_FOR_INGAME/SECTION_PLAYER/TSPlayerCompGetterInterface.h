// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TSPlayerCompGetterInterface.generated.h"

class UTSBodyEquipVisualComponent;
class UTSBodyInventoryComponent;
class UTSBackPackEquipVisualComponent;
class UTSBackPackInventoryComponent;
class UTSHotKeyEquipVisualComponent;
class UTSHotKeyInventoryComponent;
class USpringArmComponent;
class UCameraComponent;
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

public:
	// 카메라 게터 
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "TS | Player")
	UCameraComponent* GetCameraComponent() const;
	
	// 스프링 암 게터 
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "TS | Player")
	USpringArmComponent* GetSpringArmComponent() const;
	
	// 핫 키 컴포넌트 게터 
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "TS | Player")
	UTSHotKeyInventoryComponent* GetHotKeyInventoryComponent() const;
	
	// 핫키 비쥬얼 컴포넌트 게터 
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "TS | Player")
	UTSHotKeyEquipVisualComponent* GetHotKeyEquipVisualComponent() const;
	
	// 가방 컴포넌트 게터 
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "TS | Player")
	UTSBackPackInventoryComponent* GetBackPackInventoryComponent() const;
	
	// 가방 비쥬얼 컴포넌트 게터 
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "TS | Player")
	UTSBackPackEquipVisualComponent* GetBackPackEquipVisualComponent() const;
	
	// 장비 컴포넌트 게터 
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "TS | Player")
	UTSBodyInventoryComponent* GetBodyInventoryComponent() const;
	
	// 장비 컴포넌트 게터 
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "TS | Player")
	UTSBodyEquipVisualComponent* GetBodyEquipVisualComponent() const;
	
};
