// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "A_FOR_INGAME/SECTION_ITEM/Inventory/Inventory/Comp/A_Master/TSInventoryMasterComponent.h"
#include "TSHotKeyInventoryComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TINYSURVIVOR_API UTSHotKeyInventoryComponent : public UTSInventoryMasterComponent
{
	GENERATED_BODY()
	
//======================================================================================================================	
#pragma region 라이프_사이클
	
	
	//━━━━━━━━━━━━━━━━━━━━
	// 라이프 사이클
	//━━━━━━━━━━━━━━━━━━━━	
public:
	UTSHotKeyInventoryComponent();
	
#pragma endregion	
//======================================================================================================================	
#pragma region 핫키
	
	//━━━━━━━━━━━━━━━━━━━━
	// 핫키 
	//━━━━━━━━━━━━━━━━━━━━	

public:
	// 핫키가 활성화되어 있음? 
	FORCEINLINE bool IsAnyHotKeyActive() { return ActiveHotKeyIndex != -1 ? true : false; };
	
	// 활성화 중인 핫키 슬롯 인덱스를 반환
	FORCEINLINE int32 GetActiveHotKeyIndex() { return ActiveHotKeyIndex; };

protected:	
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TS | Hotkey")
	int32 ActiveHotKeyIndex = -1;
	
#pragma endregion 
//======================================================================================================================	

};
