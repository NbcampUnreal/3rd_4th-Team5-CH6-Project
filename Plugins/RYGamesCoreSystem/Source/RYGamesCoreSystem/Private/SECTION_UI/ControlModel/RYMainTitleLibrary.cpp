// All CopyRight From YulRyongGameStudio //


#include "SECTION_UI/ControlModel/RYMainTitleLibrary.h"
#include "SECTION_UI/ControlModel/RYMainTitleUIControlModelSystem.h"
#include "SECTION_UI/ControlModel/Struct_RYMainTitle.h"
#include "SECTION_UTIL/RYCoreLogMessageSystem.h"
#include "SECTION_UTIL/Marco/RYCoreLogMarco.h"
#include "RYLog.h"


// 메인 타이틀 초기화를 알려주는 델리게이트를 구독하는 API
void URYMainTitleLibrary::SubscribeMainTitleInitDelegate(UObject* Object, FName FunctionName)
{
	// NOTE : 오브젝트와 함수 이름 유효성 검사는 SubscribeMainTitleInitDelegate 에서 하고 있다.
	
	URYMainTitleUIControlModelSystem* MainTitleUIControlModelSystem = URYMainTitleUIControlModelSystem::Get(Object);
	if (!IsValid(MainTitleUIControlModelSystem))
	{
		MAINUICMSYSTEM_WARN(TEXT("SubscribeMainTitleInitDelegate : Fail"));
		return;
	}
	
	MAINUICMSYSTEM_LOG_RECORD(MainTitleUIControlModelSystem, TEXT("SubscribeMainTitleInitDelegate : Success"));
	MainTitleUIControlModelSystem->SubscribeMainTitleInitDelegate(Object, FunctionName);
}

// 메인 타이틀 CM의 화면 출력 종류를 알려주는 델리게이트를 구독하는 API
void URYMainTitleLibrary::SubscribeMainTitleModeDelegate(UObject* Object, FName FunctionName)
{
	// NOTE : 오브젝트와 함수 이름 유효성 검사는 SubscribeMainTitleModeDelegate 에서 하고 있다.
	
	URYMainTitleUIControlModelSystem* MainTitleUIControlModelSystem = URYMainTitleUIControlModelSystem::Get(Object);
	if (!IsValid(MainTitleUIControlModelSystem))
	{
		MAINUICMSYSTEM_WARN(TEXT("SubscribeMainTitleModeDelegate : Fail"));
		return;
	}
	
	MAINUICMSYSTEM_LOG_RECORD(MainTitleUIControlModelSystem, TEXT("SubscribeMainTitleModeDelegate : Success"));
	MainTitleUIControlModelSystem->SubscribeMainTitleModeDelegate(Object, FunctionName);
}

// 메인 타이틀 CM의 출력 화면 종류 세팅을 요청하는 API
void URYMainTitleLibrary::SetMainTitleMode(const UObject* Object, ERYMainTitleMode NewMainTitleMode)
{
	// NOTE : 오브젝트 검사는 Get 에서 하고 있다.
		
	URYMainTitleUIControlModelSystem* MainTitleUIControlModelSystem = URYMainTitleUIControlModelSystem::Get(Object);
	if (!IsValid(MainTitleUIControlModelSystem))
	{
		MAINUICMSYSTEM_WARN(TEXT("SetMainTitleMode : Fail"));
		return;
	}
	
	MAINUICMSYSTEM_LOG_RECORD(MainTitleUIControlModelSystem, TEXT("SetMainTitleMode : Success"));
	MainTitleUIControlModelSystem->SetMainTitleMode(NewMainTitleMode);
}

// 어떤 서브 위젯을 생성해야하는지 알려주는 API
void URYMainTitleLibrary::RequestSubWidgetClass(const UObject* Object, ERYMainTitleNoticeType NoticeType, APlayerController* PlayerController)
{
	// NOTE : 오브젝트 검사는 Get 에서 하고 있다.
	
	URYMainTitleUIControlModelSystem* MainTitleUIControlModelSystem = URYMainTitleUIControlModelSystem::Get(Object);
	if (!IsValid(MainTitleUIControlModelSystem))
	{
		MAINUICMSYSTEM_WARN(TEXT("RequestSubWidgetClass : Fail"));
		return;
	}
	
	MAINUICMSYSTEM_LOG_RECORD(MainTitleUIControlModelSystem, TEXT("RequestSubWidgetClass : Success"));
	MainTitleUIControlModelSystem->RequestSubWidgetClass(NoticeType, PlayerController);
}

// 서브 위젯으로부터 받은 입력을 알려주는 델리게이트를 구독하는 API 
void URYMainTitleLibrary::SubscribeMainTitleSubWidgetDelegate(UObject* Object, FName FunctionName)
{
	// NOTE : 오브젝트와 함수 이름 유효성 검사는 SubscribeMainTitleSubWidgetDelegate 에서 하고 있다.
	
	URYMainTitleUIControlModelSystem* MainTitleUIControlModelSystem = URYMainTitleUIControlModelSystem::Get(Object);
	if (!IsValid(MainTitleUIControlModelSystem))
	{
		MAINUICMSYSTEM_WARN(TEXT("SubscribeMainTitleSubWidgetDelegate : Fail"));
		return;
	}
	
	MAINUICMSYSTEM_LOG_RECORD(MainTitleUIControlModelSystem, TEXT("SubscribeMainTitleSubWidgetDelegate : Success"));
	MainTitleUIControlModelSystem->SubscribeMainTitleSubWidgetDelegate(Object, FunctionName);
}

// 서브 위젯 알림 델리게이트 
void URYMainTitleLibrary::SetMainTitleSubWidgetMessage(const UObject* Object, FRYMainTitleSubWidgetMessage NewMainTitleSubWidgetMessage)
{
	// NOTE : 오브젝트 검사는 Get 에서 하고 있다.
	
	URYMainTitleUIControlModelSystem* MainTitleUIControlModelSystem = URYMainTitleUIControlModelSystem::Get(Object);
	if (!IsValid(MainTitleUIControlModelSystem))
	{
		MAINUICMSYSTEM_WARN(TEXT("SetMainTitleSubWidgetMessage : Fail"));
		return;
	}
	
	MAINUICMSYSTEM_LOG_RECORD(MainTitleUIControlModelSystem, TEXT("SetMainTitleSubWidgetMessage : Success"));
	MainTitleUIControlModelSystem->SetMainTitleSubWidgetMessage(NewMainTitleSubWidgetMessage);
}

void URYMainTitleLibrary::DoESCLogic(const UObject* Object)
{
	// NOTE : 오브젝트 검사는 Get 에서 하고 있다.
	
	URYMainTitleUIControlModelSystem* MainTitleUIControlModelSystem = URYMainTitleUIControlModelSystem::Get(Object);
	if (!IsValid(MainTitleUIControlModelSystem))
	{
		MAINUICMSYSTEM_WARN(TEXT("DoESCLogic : Fail"));
		return;
	}
	
	MAINUICMSYSTEM_LOG_RECORD(MainTitleUIControlModelSystem, TEXT("DoESCLogic : Success"));
	MainTitleUIControlModelSystem->DoESCLogic();
}
