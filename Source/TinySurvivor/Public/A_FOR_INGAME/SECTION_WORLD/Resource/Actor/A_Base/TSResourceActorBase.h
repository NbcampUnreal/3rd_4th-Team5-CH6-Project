// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "A_FOR_INGAME/SECTION_INTERACT/Interface/TSInteractInterface.h"
#include "A_FOR_INGAME/SECTION_WORLD/Resource/Interface/TSResourceInterface.h"
#include "TSResourceActorBase.generated.h"

class UTSLootHandleComponent;
class UWidgetComponent;

UCLASS()
class TINYSURVIVOR_API ATSResourceActorBase : public AActor, public ITSResourceInterface, public ITSInteractInterface
{
	GENERATED_BODY()

//======================================================================================================================	
#pragma region REP_API
	
	//━━━━━━━━━━━━━━━━━━━━
	// REP API
	//━━━━━━━━━━━━━━━━━━━━
	
public:
	UFUNCTION() void OnRep_ResourceData();
	
#pragma endregion
//======================================================================================================================	
#pragma region 라이프_사이클
	
	
	//━━━━━━━━━━━━━━━━━━━━
	// 라이프 사이클
	//━━━━━━━━━━━━━━━━━━━━	
	
public:
	ATSResourceActorBase();
	virtual void BeginPlay() override;
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
	virtual void InitInteractUI(FTSResourceRuntimeData& ItemRuntimeData);
	
	// 위젯 컴포넌트 선언
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TS | Item")
	TObjectPtr<UWidgetComponent> InteractWidgetComp = nullptr;
	
	// 롤 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TS | Item")
	ETSPlayRole PlayRole = ETSPlayRole::Resource;
	
	
#pragma endregion
//======================================================================================================================
#pragma region 자원_API_및_데이터
	
	//━━━━━━━━━━━━━━━━━━━━
	// 자원_API_및_데이터
	//━━━━━━━━━━━━━━━━━━━━	
	
public:
	// ITSResourceInterface ~
	FORCEINLINE virtual FTSResourceRuntimeData& GetResourceRuntimeData() override { return ResourceData; }
	virtual void SetResourceRuntimeData(FTSResourceRuntimeData& InResourceRuntimeData) override { ResourceData = InResourceRuntimeData; }
	virtual bool TryInteractLogicOnResource_Implementation(float InAttackDamage, FGameplayTag InInteractType, FVector InHitImpactPoint) override;
	// ~ ITSResourceInterface
	
	// 루팅 컴포넌트 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TS | Resource")
	TObjectPtr<UTSLootHandleComponent> LootHandleComponent = nullptr;
	
	UPROPERTY(ReplicatedUsing = OnRep_ResourceData, EditAnywhere, BlueprintReadWrite, Category = "TS | Resource")
	FTSResourceRuntimeData ResourceData;	
	
#pragma endregion
//======================================================================================================================	
	
	
	
};
