// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "A_FOR_INGAME/SECTION_INTERACT/Interface/TSInteractInterface.h"
#include "GameFramework/Actor.h"
#include "TSItemActorBase.generated.h"

UCLASS(NotBlueprintable, Abstract)
class TINYSURVIVOR_API ATSItemActorBase : public AActor, public ITSInteractInterface
{
	GENERATED_BODY()

//======================================================================================================================	
#pragma region 라이프_사이클
	
	
	//━━━━━━━━━━━━━━━━━━━━
	// 라이프 사이클
	//━━━━━━━━━━━━━━━━━━━━	
public:
	ATSItemActorBase();
	
#pragma endregion
//======================================================================================================================	
#pragma region 인터렉트_API
	
	
	//━━━━━━━━━━━━━━━━━━━━
	// 라이프 사이클
	//━━━━━━━━━━━━━━━━━━━━	
	
public:
	// ITSInteractInterface ~ 
	virtual void ToggleInteractWidget_Implementation(bool InWantOn) override;
	// ~ ITSInteractInterface
	
#pragma endregion
//======================================================================================================================
#pragma region 아이템_API
	
	
	//━━━━━━━━━━━━━━━━━━━━
	// 라이프 사이클
	//━━━━━━━━━━━━━━━━━━━━	
	
public:
	
#pragma endregion
//======================================================================================================================
};
