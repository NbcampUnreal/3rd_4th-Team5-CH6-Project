// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "RYClipboardLibrary.generated.h"

/**
 * 
 */
UCLASS()
class RYGAMESCORESYSTEM_API URYClipboardLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	
	// 인게임의 스트링 문자열을 복사해서 윈도우 운영 체제 넘겨주는 API (ex 인게임 방 이름 복사해서 메모장에 붙여넣기 가능)
	UFUNCTION(BlueprintCallable, Category = "RY | Core | Lobby")
	static void CopyToClipboard(const FString& Text);
};
