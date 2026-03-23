// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "TSInteractUIBase.h"
#include "TSResourceInteractUI.generated.h"

/**
 * 
 */
UCLASS()
class TINYSURVIVOR_API UTSResourceInteractUI : public UTSInteractUIBase
{
	GENERATED_BODY()
	
//======================================================================================================================	
#pragma region 인터렉트API
	
	
	//━━━━━━━━━━━━━━━━━━━━
	// 인터렉트API
	//━━━━━━━━━━━━━━━━━━━━	
public:
	
	virtual void SetResourceRemainStackInfo(int32 InRemainStack, int32 InMaxStack);

protected:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = " TS | Resource", meta=(BindWidget))
	TObjectPtr<UTextBlock> ResourceRemainStackInfoTextBlock = nullptr;
	
#pragma endregion
//======================================================================================================================	
};
