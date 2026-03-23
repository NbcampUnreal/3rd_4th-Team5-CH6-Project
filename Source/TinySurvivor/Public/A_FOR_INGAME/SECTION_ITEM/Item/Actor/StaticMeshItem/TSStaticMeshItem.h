// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "A_FOR_INGAME/SECTION_ITEM/Item/Actor/Base/TSItemActorBase.h"
#include "TSStaticMeshItem.generated.h"

UCLASS()
class TINYSURVIVOR_API ATSStaticMeshItem : public ATSItemActorBase
{
	GENERATED_BODY()
	
//======================================================================================================================	
#pragma region 라이프_사이클
	
	
	//━━━━━━━━━━━━━━━━━━━━
	// 라이프 사이클
	//━━━━━━━━━━━━━━━━━━━━	
public:
	
	ATSStaticMeshItem();
	virtual void BeginPlay() override;
	
#pragma endregion
//======================================================================================================================	
#pragma region 컴포넌트
	
	
	//━━━━━━━━━━━━━━━━━━━━
	// 컴포넌트 
	//━━━━━━━━━━━━━━━━━━━━	
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TS | Item")
	TObjectPtr<UStaticMeshComponent> ItemStaticMeshComp = nullptr;
	
	
#pragma endregion
//======================================================================================================================	
};
