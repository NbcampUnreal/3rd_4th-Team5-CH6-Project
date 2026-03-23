// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "TSInteractUIBase.h"
#include "TSItemInteractUI.generated.h"

/**
 * 
 */
UCLASS()
class TINYSURVIVOR_API UTSItemInteractUI : public UTSInteractUIBase
{
	GENERATED_BODY()
	
//======================================================================================================================	
#pragma region 인터렉트API
	
	
	//━━━━━━━━━━━━━━━━━━━━
	// 인터렉트API
	//━━━━━━━━━━━━━━━━━━━━	
public:
	
	virtual void SetItemRemainStackInfo(int32 InRemainStack);

protected:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = " TS | Item", meta=(BindWidget))
	TObjectPtr<UTextBlock> ItemRemainStackInfoTextBlock = nullptr;
	
#pragma endregion
//======================================================================================================================	
};
