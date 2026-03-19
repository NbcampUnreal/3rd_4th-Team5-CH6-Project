// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NoticeUIBase.generated.h"

class UButton;
/**
 * 
 */
UCLASS()
class RYGAMESCORESYSTEM_API UNoticeUIBase : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RYGameCore | NoticeUI")
	FText NoticeText;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RYGameCore | NoticeUI")
	TObjectPtr<UButton> ConfirmButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RYGameCore | NoticeUI")
	TObjectPtr<UButton> CancelButton;
	
};
