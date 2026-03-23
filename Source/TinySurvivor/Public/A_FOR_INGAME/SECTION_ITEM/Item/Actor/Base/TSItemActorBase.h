// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "A_FOR_INGAME/SECTION_INTERACT/Interface/TSInteractInterface.h"
#include "A_FOR_INGAME/SECTION_ITEM/Item/Data/Struct/TSItemRuntimeData.h"
#include "A_FOR_INGAME/SECTION_ITEM/Item/Interface/TSItemInterface.h"
#include "TSItemActorBase.generated.h"

class UWidgetComponent;

UCLASS(NotBlueprintable, Abstract)
class TINYSURVIVOR_API ATSItemActorBase : public AActor, public ITSInteractInterface, public ITSItemInterface
{
	GENERATED_BODY()

//======================================================================================================================	
#pragma region REP_API
	
	//━━━━━━━━━━━━━━━━━━━━
	// REP API
	//━━━━━━━━━━━━━━━━━━━━
	
public:
	UFUNCTION()
	void OnRep_ItemData();
	
#pragma endregion
//======================================================================================================================	
#pragma region 라이프_사이클
	
	
	//━━━━━━━━━━━━━━━━━━━━
	// 라이프 사이클
	//━━━━━━━━━━━━━━━━━━━━	
public:
	ATSItemActorBase();
	virtual void PostInitializeComponents() override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
#pragma endregion
//======================================================================================================================	
#pragma region 인터렉트API_컴포넌트_데이터
	
	
	//━━━━━━━━━━━━━━━━━━━━
	// 인터렉트API_컴포넌트_데이터
	//━━━━━━━━━━━━━━━━━━━━	
	
public:
	// ITSInteractInterface ~ 
	virtual void ToggleInteractWidget_Implementation(bool InWantOn) override;
	FORCEINLINE virtual ETSPlayRole GetPlayRole_Implementation() override { return PlayRole;}
	// ~ ITSInteractInterface
	
protected:
	// 위젯 컴포넌트 선언
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TS | Item")
	TObjectPtr<UWidgetComponent> InteractWidgetComp = nullptr;
	
	// 롤 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TS | Item")
	ETSPlayRole PlayRole = ETSPlayRole::Item;
	
#pragma endregion
//======================================================================================================================
#pragma region 아이템_API_및_데이터
	
	
	//━━━━━━━━━━━━━━━━━━━━
	// 아이템_API_및_데이터
	//━━━━━━━━━━━━━━━━━━━━	
	
public:
	
	// ITSItemInterface ~ 
	FORCEINLINE virtual FTSItemRuntimeData& GetItemRuntimeDataPtr() override { return ItemData;};
	virtual void SetItemRuntimeData(FTSItemRuntimeData& InItemRuntimeData) override { ItemData = InItemRuntimeData;};
	/// ~ ITSItemInterface
	
protected:
	
	// 아이템 데이터 
	UPROPERTY(ReplicatedUsing = OnRep_ItemData,EditAnywhere, BlueprintReadWrite, Category = "TS | Item")
	FTSItemRuntimeData ItemData;
	
#pragma endregion
//======================================================================================================================
};
