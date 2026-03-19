// All CopyRight From YulRyongGameStudio //


#include "SECTION_UI/ControlModel/RYMainTitleUIControlModelSystem.h"

#include "Kismet/GameplayStatics.h"
#include "SECTION_CYCLE/RYGameCycleLibrary.h"
#include "SECTION_CYCLE/RYGameCycleSubSystem.h"
#include "SECTION_UI/ControlModel/RYMainTitleSettings.h"

//======================================================================================================================
#pragma region GetSystem
	//--------------------
	// Get System
	//--------------------

URYMainTitleUIControlModelSystem* URYMainTitleUIControlModelSystem::Get(const UObject* Object)
{
	if (!IsValid(Object))
	{
		// 로그
		return nullptr;
	}
	
	// 월드 컨텍스트로부터 월드를 가져옵니다.
	const UWorld* World = Object->GetWorld();
	if (!IsValid(World))
	{
		// 로그
		return nullptr;
	}
	
	// 월드로부터 게임 인스턴스를 가져옵니다.
	const UGameInstance* GameInstance = World->GetGameInstance();
	if (!IsValid(GameInstance))
	{
		// 로그
		return nullptr;
	}
	
	// 로그
	
	// 게임 인스턴스로부터 게임 인스턴스 서브 시스템을 가져옵니다.
	return GameInstance->GetSubsystem<URYMainTitleUIControlModelSystem>();
}

#pragma endregion
//======================================================================================================================
#pragma region LifeCycle
	//--------------------
	// Life Cycle
	//--------------------

bool URYMainTitleUIControlModelSystem::ShouldCreateSubsystem(UObject* Outer) const
{
	return true;
}

void URYMainTitleUIControlModelSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	// 반드시 게임 사이클 시스템 이후에 초기화 
	Collection.InitializeDependency(URYGameCycleSubSystem::StaticClass());
	
	// 게임 사이클 델리게이크 구독
	URYGameCycleLibrary::SubscribeGameCycleDelegate(this, TEXT("CallOnGameCycleMode"));
	
	// 세팅 불러오기
	const URYMainTitleSettings* Settings = GetDefault<URYMainTitleSettings>();
	if (!IsValid(Settings))
	{
		// 로그
		return;
	}
	
	// 조회용 TMAP에 넣기.
	SubWidgetClassMap.Add(ERYMainTitleNoticeType::NewGame, Settings->NewGameWidgetClass);
	SubWidgetClassMap.Add(ERYMainTitleNoticeType::LoadGame, Settings->LoadGameWidgetClass);
	SubWidgetClassMap.Add(ERYMainTitleNoticeType::DeleteGame, Settings->DeleteSaveWidgetClass);
	
	SubWidgetClassMap.Add(ERYMainTitleNoticeType::OptionSet, Settings->OptionsWidgetClass);
	
	SubWidgetClassMap.Add(ERYMainTitleNoticeType::ResetAll, Settings->ResetALLOptionsWidgetClass);
	SubWidgetClassMap.Add(ERYMainTitleNoticeType::ResetGraphic, Settings->ResetGraphicWidgetClass);
	SubWidgetClassMap.Add(ERYMainTitleNoticeType::ResetSound, Settings->ResetSoundWidgetClass);
	SubWidgetClassMap.Add(ERYMainTitleNoticeType::ResetControl, Settings->ResetControlWidgetClass);
	SubWidgetClassMap.Add(ERYMainTitleNoticeType::ResetGameplay, Settings->ResetGameplayWidgetClass);
	
	SubWidgetClassMap.Add(ERYMainTitleNoticeType::QuitGame, Settings->QuitGameWidgetClass);
	
	SubWidgetClassMap.Add(ERYMainTitleNoticeType::CreateSession, Settings->CreateSessionWidgetClass);
	SubWidgetClassMap.Add(ERYMainTitleNoticeType::CreateSessionFail, Settings->CreateSessionFailWidgetClass);
	
	SubWidgetClassMap.Add(ERYMainTitleNoticeType::JoinSession, Settings->JoinSessionWidgetClass);
	SubWidgetClassMap.Add(ERYMainTitleNoticeType::JoinSessionFail, Settings->JoinSessionFailWidgetClass);
	
	SubWidgetClassMap.Add(ERYMainTitleNoticeType::Streaming, Settings->StreamerModeWidgetClass);
	
	// 로그
}

void URYMainTitleUIControlModelSystem::Deinitialize()
{
	// 모든 델리게이트 바인딩 해제
	OnMainTitleInitDelegate.Clear();
	OnMainTitleModeDelegate.Clear();
	OnMainTitleSubWidgetDelegate.Clear();
	
	
	Super::Deinitialize();
	
	// 로그
}
	
#pragma endregion
//======================================================================================================================
#pragma region GameCycle
	//--------------------
	// Game Cycle
	//--------------------
	
void URYMainTitleUIControlModelSystem::CallOnGameCycleMode(ERYGameCycleMode NewGameCycleMode)
{
	switch (NewGameCycleMode)
	{
	case ERYGameCycleMode::None:
		break;
		
	case ERYGameCycleMode::Init:
		CallOnInit();
		break;
		
	case ERYGameCycleMode::MainTitle:
		CallOnMainTitleMode();
		break;
		
	case ERYGameCycleMode::GameStart:
		break;
		
	case ERYGameCycleMode::GamePause:
		break;
		
	case ERYGameCycleMode::GameEnd:
		break;
		
	case ERYGameCycleMode::ManualSave:
		break;
		
	case ERYGameCycleMode::AutoSave:
		break;
		
	case ERYGameCycleMode::ERROR:
		break;
		
	case ERYGameCycleMode::SAVELOG:
		break;
	}
}

void URYMainTitleUIControlModelSystem::CallOnInit()
{
}

void URYMainTitleUIControlModelSystem::CallOnMainTitleMode()
{
}

#pragma endregion
//======================================================================================================================
#pragma region MaintitleUI
	//--------------------
	// Main Title UI
	//--------------------


void URYMainTitleUIControlModelSystem::SubscribeMainTitleInitDelegate(UObject* Object, FName FunctionName)
{
	if (!IsValid(Object))
	{
		// 로그
		return;
	}
	
	if (FunctionName.IsNone())
	{
		// 로그
		return;
	}
	
	// 스크립트 델리게이트 생성 및 설정
	FScriptDelegate Delegate;
	Delegate.BindUFunction(Object, FunctionName);
	
	// 등록 (중복 방지 적용)
	OnMainTitleInitDelegate.AddUnique(Delegate);
	
	// 로그
}


void URYMainTitleUIControlModelSystem::SubscribeMainTitleModeDelegate(UObject* Object, FName FunctionName)
{
	if (!IsValid(Object))
	{
		// 로그
		return;
	}
	
	if (FunctionName.IsNone())
	{
		// 로그
		return;
	}
	
	// 스크립트 델리게이트 생성 및 설정
	FScriptDelegate Delegate;
	Delegate.BindUFunction(Object, FunctionName);
	
	// 등록 (중복 방지 적용)
	OnMainTitleModeDelegate.AddUnique(Delegate);
	
	// 로그
}

void URYMainTitleUIControlModelSystem::SetMainTitleMode(const ERYMainTitleMode& NewMainTitleMode)
{
	if (IsValid(SubWidgetInstance))
	{
		SubWidgetInstance->RemoveFromParent();
	}
	
	CurrentMainTitleMode = NewMainTitleMode;
	OnMainTitleModeDelegate.Broadcast(CurrentMainTitleMode);
}

void URYMainTitleUIControlModelSystem::RequestSubWidgetClass(const ERYMainTitleNoticeType& NoticeType, APlayerController* PlayerController)
{
	if (!IsValid(GetWorld()))
	{
		// 로그
		return;
	}
	
	if (!SubWidgetClassMap.Contains(NoticeType))
	{
		// 로그
		return;
	}
	
	TSubclassOf<UUserWidget> Value = SubWidgetClassMap[NoticeType];
	if (!IsValid(Value))
	{
		// 로그
		return;
	}
	
	if (!IsValid(PlayerController))
	{
		// 로그
		return;
	}
	
	UUserWidget* CreatedWidget = CreateWidget(PlayerController, SubWidgetClassMap[NoticeType]);
	if (!IsValid(CreatedWidget))
	{
		// 로그
		return;
	}
	
	if (IsValid(SubWidgetInstance))
	{
		SubWidgetInstance->RemoveFromParent();
		SubWidgetInstance = nullptr;
	}
	
	CreatedWidget->AddToViewport();
	SubWidgetInstance = CreatedWidget;
}

void URYMainTitleUIControlModelSystem::SubscribeMainTitleSubWidgetDelegate(UObject* Object, FName FunctionName)
{
	if (!IsValid(Object))
	{
		// 로그
		return;
	}
	
	if (FunctionName.IsNone())
	{
		// 로그
		return;
	}
	
	// 스크립트 델리게이트 생성 및 설정
	FScriptDelegate Delegate;
	Delegate.BindUFunction(Object, FunctionName);
	
	// 등록 (중복 방지 적용)
	OnMainTitleSubWidgetDelegate.AddUnique(Delegate);
	
	// 로그
}

void URYMainTitleUIControlModelSystem::SetMainTitleSubWidgetMessage(const FRYMainTitleSubWidgetMessage& NewMainTitleSubWidgetMessage)
{
	OnMainTitleSubWidgetDelegate.Broadcast(NewMainTitleSubWidgetMessage);
	
	if (IsValid(SubWidgetInstance))
	{
		SubWidgetInstance->RemoveFromParent();
		SubWidgetInstance = nullptr;
	}
}

void URYMainTitleUIControlModelSystem::DoESCLogic()
{
	if (IsValid(SubWidgetInstance))
	{
		SubWidgetInstance->RemoveFromParent();
		SubWidgetInstance = nullptr;
		return;
	}
	
	if (CurrentMainTitleMode != ERYMainTitleMode::Default)
	{
		SetMainTitleMode(ERYMainTitleMode::Default);
		return;
	}
	
	if (CurrentMainTitleMode == ERYMainTitleMode::Default)
	{
		RequestSubWidgetClass(ERYMainTitleNoticeType::QuitGame, UGameplayStatics::GetPlayerController(GetWorld(), 0));
		return;
	}
}

#pragma endregion
//======================================================================================================================
