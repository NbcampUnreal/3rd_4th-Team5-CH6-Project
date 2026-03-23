// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "A_FOR_INGAME/SECTION_UI/A_Base/TSWidgetBase.h"
#include "TSInteractUIBase.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class TINYSURVIVOR_API UTSInteractUIBase : public UTSWidgetBase
{
	GENERATED_BODY()
	
//======================================================================================================================	
#pragma region 인터렉트API
	
	
	//━━━━━━━━━━━━━━━━━━━━
	// 인터렉트API
	//━━━━━━━━━━━━━━━━━━━━	
public:

	virtual void SetInteractInfo(FText& InInteractActorName, FText& InInteractKey);
		
#pragma endregion
//======================================================================================================================	
#pragma region 파츠
	
	
	//━━━━━━━━━━━━━━━━━━━━
	// 파츠
	//━━━━━━━━━━━━━━━━━━━━
protected:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TS | Interact", meta=(BindWidget))
	TObjectPtr<UTextBlock> InteractActorNameTextBlock = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TS | Interact", meta=(BindWidget))
	TObjectPtr<UTextBlock> InteractKeyTextBlock = nullptr;
	
#pragma endregion
//======================================================================================================================	
#pragma region 데이터
	
	//━━━━━━━━━━━━━━━━━━━━
	// 데이터
	//━━━━━━━━━━━━━━━━━━━━
protected:
	
	// 표시할 상호작용 액터의 이름 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TS | Interact")
	FText InteractActorName = FText::FromString(TEXT("No Name Edit"));
	
	// 표시할 상호작용 키 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TS | Interact")
	FText InteractKey = FText::FromString(TEXT("No Key Edit"));
		
#pragma endregion
//======================================================================================================================	
};
