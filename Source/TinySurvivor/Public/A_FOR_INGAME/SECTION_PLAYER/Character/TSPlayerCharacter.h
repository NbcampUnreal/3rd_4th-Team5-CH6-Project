// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "A_FOR_INGAME/SECTION_ITEM/Inventory/Inventory/Interface/ForOwner/TSInventoryOwnerActionInterface.h"
#include "TSPlayerCharacter.generated.h"

/**
 * 
 */
UCLASS()
//-------------------------------------------------------------
class TINYSURVIVOR_API ATSPlayerCharacter : public ACharacter,

// 인벤토리 관련 인터페이스
public ITSInventoryOwnerActionInterface
//--------------------------------------------------------------
{
	GENERATED_BODY()
	
//======================================================================================================================	
#pragma region 라이프_사이클
	
	
	//━━━━━━━━━━━━━━━━━━━━
	// 라이프 사이클
	//━━━━━━━━━━━━━━━━━━━━	

public:
	ATSPlayerCharacter();
	
	
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
};
