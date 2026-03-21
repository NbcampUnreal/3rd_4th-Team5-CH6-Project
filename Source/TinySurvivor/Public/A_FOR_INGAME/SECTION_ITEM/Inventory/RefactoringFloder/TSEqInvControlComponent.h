// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "A_FOR_INGAME/SECTION_ITEM/Inventory/Struct/TSInventorySlot.h"
#include "TSEqInvControlComponent.generated.h"


class UTSInventoryMasterComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TINYSURVIVOR_API UTSEqInvControlComponent : public UActorComponent
{
	GENERATED_BODY()

//======================================================================================================================
#pragma region 라이프_사이클


	//━━━━━━━━━━━━━━━━━━━━
	// 라이프 사이클
	//━━━━━━━━━━━━━━━━━━━━
	
public:
	UTSEqInvControlComponent();
	virtual void BeginPlay() override;
	
	
#pragma endregion
//======================================================================================================================	
#pragma region 부패도_관리


	//━━━━━━━━━━━━━━━━━━━━
	// 부패도 관리
	//━━━━━━━━━━━━━━━━━━━━

protected:
	
	// 부패도 시스템 바인딩 함수
	void BindDecayManagerDelegate_internal();
	
	// 부패도 업데이트 내부 총괄 함수 
	UFUNCTION() void OnDecayTick_internal();
	
	// 부패도 함수 (인벤토리 마다)
	void ConvertToDecayedItem_internal(EInventoryType InventoryType);
	
	
#pragma endregion
//======================================================================================================================
#pragma region 관리할_인벤토리


	//━━━━━━━━━━━━━━━━━━━━
	// 관리할_인벤토리
	//━━━━━━━━━━━━━━━━━━━━
	
public:
	
	// 관리할 인벤토리 마스터 컴포넌트
	UPROPERTY()
	UTSInventoryMasterComponent* InventoryMasterComp = nullptr;
	
	
#pragma endregion
//======================================================================================================================
	
};
