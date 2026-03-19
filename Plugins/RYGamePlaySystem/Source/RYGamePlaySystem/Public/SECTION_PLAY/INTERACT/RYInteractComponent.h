// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RYInteractComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class RYGAMEPLAYSYSTEM_API URYInteractComponent : public UActorComponent
{
	GENERATED_BODY()

//======================================================================================================================
#pragma region Life_Cycle
	
	//--------------------
	// 라이프 사이클 
	//--------------------
	
public:
	URYInteractComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,FActorComponentTickFunction* ThisTickFunction) override;
	
#pragma endregion
//======================================================================================================================
};
