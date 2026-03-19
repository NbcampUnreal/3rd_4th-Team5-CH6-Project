// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "Struct_RYMainTitle.h"
#include "SECTION_CYCLE/Enum_RYGameCycle.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "RYMainTitleUIControlModelSystem.generated.h"

// 메인 화면 초기화 요청 델리게이트 
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMainTitleMainWidgetInitDelegate);

// 메인 타이틀 화면 출력 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMainTitleMainWidgetDelegate, ERYMainTitleMode, NewMainTitleMode);

// 서브 위젯 입력 알림 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMainTitleSubWidgetDelegate, FRYMainTitleSubWidgetMessage, NewMainTitleSubWidgetMessage);

/**
 *  메인 타이틀 UI 컨트롤 모델 시스템 
 * : 메인 역할 : 
 * 1. 현재 메인 화면이 어떤 화면을 출력해야 하는지 알려준다. (브로드 캐스트)
 * 2. 어떤 서브 위젯을 생성해야하는지 알려준다 (위젯 생성)
 * 3. 여러 델리게이트를 통해 메인 타이틀 화면에서 입력 받은 것을 알려준다 (보로드 캐스트 + 구조체)
 */
UCLASS()
class RYGAMESCORESYSTEM_API URYMainTitleUIControlModelSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
//======================================================================================================================
#pragma region GetSystem
	//--------------------
	// Get System
	//--------------------

public:
	static URYMainTitleUIControlModelSystem* Get(const UObject* Object);
	
#pragma endregion
//======================================================================================================================
#pragma region LifeCycle
	//--------------------
	// Life Cycle
	//--------------------

public:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	
#pragma endregion
//======================================================================================================================
#pragma region GameCycle
	//--------------------
	// Game Cycle
	//--------------------
	
public:
	// 게임 사이클 메인 바인딩 함수
	UFUNCTION()
	void CallOnGameCycleMode(ERYGameCycleMode NewGameCycleMode);
	
	// 메인 화면 초기화
	void CallOnInit();
	
	// 메인 화면일 때
	void CallOnMainTitleMode();
	
#pragma endregion
//======================================================================================================================
#pragma region MaintitleUI_API
	//--------------------
	// Main Title UI_API
	//--------------------
	
public:
	// 메인 타이틀 초기화를 알려주는 델리게이트를 구독하는 API
	void SubscribeMainTitleInitDelegate(UObject* Object, FName FunctionName);
	
	// 메인 타이틀 CM의 화면 출력 종류를 알려주는 델리게이트를 구독하는 API
	void SubscribeMainTitleModeDelegate(UObject* Object, FName FunctionName);

	// 메인 타이틀 CM의 출력 화면 종류 세팅을 요청하는 API
	void SetMainTitleMode(const ERYMainTitleMode& NewMainTitleMode);

	// 어떤 서브 위젯을 생성해야하는지 알려주는 API
	void RequestSubWidgetClass(const ERYMainTitleNoticeType& NoticeType, APlayerController* PlayerController);
	
	// 서브 위젯으로부터 받은 입력을 알려주는 델리게이트를 구독하는 API 
	void SubscribeMainTitleSubWidgetDelegate(UObject* Object, FName FunctionName);

	// 서브 위젯 알림 델리게이트 
	void SetMainTitleSubWidgetMessage(const FRYMainTitleSubWidgetMessage& NewMainTitleSubWidgetMessage);
	
	// ESC 작동 함수
	void DoESCLogic();
	
#pragma endregion
//======================================================================================================================
#pragma region MaintitleUI_델리게이트
	
	//--------------------
	// Main Title UI_델리게이트
	//--------------------
		
protected:
	// 메인 화면 초기화 요청 델리게이트 
	UPROPERTY(BlueprintAssignable)
	FMainTitleMainWidgetInitDelegate OnMainTitleInitDelegate;
	
	// 메인 타이틀 화면 출력 델리게이트
	UPROPERTY(BlueprintAssignable)
	FMainTitleMainWidgetDelegate OnMainTitleModeDelegate;
	
	// 서브 위젯 입력 델리게이트
	UPROPERTY(BlueprintAssignable)
	FMainTitleSubWidgetDelegate OnMainTitleSubWidgetDelegate;
	
#pragma endregion
//======================================================================================================================
#pragma region MaintitleUI_데이터
	
	//--------------------
	// Main Title UI_델리게이트
	//--------------------	
	
protected:
	
	// 서브 위젯 생성 조회 데이터 
	TMap<ERYMainTitleNoticeType, TSubclassOf<UUserWidget>> SubWidgetClassMap;

	// 서브 위젯 인스턴스 
	UPROPERTY() 
	TObjectPtr<UUserWidget> SubWidgetInstance = nullptr;
	
	// 현재 UI 출력 화면 
	ERYMainTitleMode CurrentMainTitleMode = ERYMainTitleMode::None;
	
#pragma endregion
//======================================================================================================================
	
};

#pragma region DetailedDescription
// 상세 설명 
/*
 * < 개요 > 
 * 메인 타이틀 UI 컨트롤러 모델 시스템은 메인 타이틀 UI에게 실제로 무엇을 출력해야 하는 알려주는 백엔드 시스템입니다.
 *
 * < 시스템의 역할 >
 * 메인 타이틀 화면에서 플레이어의 입력과 설정으로 인해 어떤 UI가 켜지거나 어떤 UI가 꺼지거나 할 수 있습니다.
 * 그런데 이때 백엔드 시스템이 없이 UI가 프론트와 백엔드를 전부 맡아버리는 상황이 온다면 이것은 God Class 문제입니다.
 * 따라서 그러한 문제를 해결하기 위해 VMCM 모델 패턴에 따른 백엔드 시스템이 개발되었습니다.
 *
 * - Ex - 
 * (현재 예시 설명 없음)
 * 
 * 
 * - 구체적 설명 -
 * - 구체적 설명 -
 * 
 * [1]번 역할의 구체적인 설명 : 1. 현재 메인 화면이 어떤 화면을 출력해야 하는지 알려준다. (브로드 캐스트)
 * 
 * 예를 들어 화면에서 옵션 버튼을 누른다고 가정하겠습니다. 그러면 메인 타이틀 화면에서 받은 입력은 옵션 화면 켜야 한다는 신호입니다.
 * 메인 타이틀 UI 컨트롤 모델 시스템 (이하 CM 시스템이라고 명명) 은 델리게이트를 통하여 현재 옵션 화면을 켜야 한다고 알려줍니다. 
 * 메인 타이틀 UI 는 (이하 VM이라 명명) 이 델리게이트를 구독하여 자신이 켜고 꺼야 하는 모든 화면을 알게 됩니다.
 * 
 * 
 * [2]번 역할의 구체적인 설명 : 2. 여러 델리게이트를 통해 메인 타이틀 화면에서 입력 받은 것을 알려준다 (위젯 생성)
 * 
 * 메인 타이틀 화면에서 예를 들어 다시 옵션 버튼을 누른다고 가정하겠습니다. 그러면 1번에 의하여 옵션 화면이 뜨게 됩니다.
 * 여기서 플레이어가 특정 옵션을 선택하고 설정 적용을 눌렀다고 가정하겠습니다. 그리고 확인창이 나타난다고 가정하겠습니다.
 * 아키텍처 설계가 없는 보통의 경우 UI가 UI를 탄생시키곤 합니다. 이렇게할 경우 UI가 UI를 탄생시키는 문제가 발생합니다.
 * 물론 많은 경우에는 이것은 문제가 되지 않습니다. 하지만 좋은 아키텍처 설계 방식은 아닙니다. 
 * 따라서 이 시스템은 그러한 알림 유형의 UI의 생성을 책임지어 일관적, 단일적인 파이프라인을 보장합니다. 
 * 이것은 게임의 시스템 확장 시 보다 수월하고 유연한 확장이 가능하도록 돕습니다.
 * 
 * 
 * [3]번 역할의 구체적인 설명 :  3. 여러 델리게이트를 통해 메인 타이틀 화면에서 입력 받은 것을 알려준다 (보로드 캐스트)
 * 
 * 메인 타이틀 화면에서 2번에 따라 옵션을 설정하고 확인창이 떴다고 가정하겠습니다. 여기서 우리는 어떻게 지금 유저가 선택한 옵션을 저장해야 한다고
 * 다른 시스템에게 알려줄 수 있을까요. 그것은 라이리아의 게임 플레이 메시지 서브 시스템을 참고한 구조체 기반 구독 발행 패턴을 적용하면 됩니다. 
 * 우리는 다음과 같은 정보를 구조체에 담아서 델리게이트를 쏘면 됩니다.
 * 1) 어떤 위젯 타입이 지금 이걸 확인을 요청했는지 ->  만약 옵션 확인창이라면 옵션 타입이 되겠습니다.
 * 2) 어떤 행동을 취해야 하는지 -> 확인? 취소? 초기화? 같은 타입이 되겠습니다.
 * 
 * 이 두 가지 구조체를 정보에 담아서 알려주게 되면 델리게이트를 구독했지만 오직 옵션 관련 시스템만 이 델리게이트에 반응하게 됩니다.
 * 그리고 어떤 행동을 취해야 하는지는 두번째 정보를 통하여 자신이 할 일을 하게 됩니다.
 * 
 * ##[주의]##
 * 프로젝트 세팅에 있는 메인 타이틀 UI 관련 세팅이 반드시 필요합니다.
 * 게임 사이클 시스템과 마찬가지로 구독과 세팅 관련 API는 전부 라이브러리화합니다.
 * 따라서 블루프린트 펑션 라이브러리를 이용해야 합니다.
 * 
 * Last Update 2026.02.25
 * 
 */
#pragma endregion