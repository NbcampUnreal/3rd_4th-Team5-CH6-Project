// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "Enum_RYMainTitle.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "RYMainTitleLibrary.generated.h"

struct FRYMainTitleSubWidgetMessage;
/**
 * 
 */
UCLASS()
class RYGAMESCORESYSTEM_API URYMainTitleLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	// 메인 타이틀 초기화를 알려주는 델리게이트를 구독하는 API
	UFUNCTION(BlueprintCallable, Category = "RY | Core | Main")
	static void SubscribeMainTitleInitDelegate(UObject* Object, FName FunctionName);
	
	// 메인 타이틀 CM의 화면 출력 종류를 알려주는 델리게이트를 구독하는 API
	UFUNCTION(BlueprintCallable, Category = "RY | Core | Main")
	static void SubscribeMainTitleModeDelegate(UObject* Object, FName FunctionName);
	
	// 메인 타이틀 CM의 출력 화면 종류 세팅을 요청하는 API
	UFUNCTION(BlueprintCallable, Category = "RY | Core | Main")
	static void SetMainTitleMode(const UObject* Object, ERYMainTitleMode NewMainTitleMode);
	
	// 어떤 서브 위젯을 생성해야하는지 알려주는 API
	UFUNCTION(BlueprintCallable, Category = "RY | Core | Main")
	static void RequestSubWidgetClass(const UObject* Object, ERYMainTitleNoticeType NoticeType, APlayerController* PlayerController);
	
	// 서브 위젯으로부터 받은 입력을 알려주는 API 
	UFUNCTION(BlueprintCallable, Category = "RY | Core | Main")
	static void SubscribeMainTitleSubWidgetDelegate(UObject* Object, FName FunctionName);
	
	// 서브 위젯 알림 델리게이트 
	UFUNCTION(BlueprintCallable, Category = "RY | Core | Main")
	static void SetMainTitleSubWidgetMessage(const UObject* Object, FRYMainTitleSubWidgetMessage NewMainTitleSubWidgetMessage);
	
	// ESC 작동 함수
	UFUNCTION(BlueprintCallable, Category = "RY | Core | Main")
	static void DoESCLogic(const UObject* Object);
	
};
