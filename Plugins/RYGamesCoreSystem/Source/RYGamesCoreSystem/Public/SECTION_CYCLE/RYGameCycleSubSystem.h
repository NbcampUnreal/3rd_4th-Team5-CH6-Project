// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "Enum_RYGameCycle.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "RYGameCycleSubSystem.generated.h"

// 게임 사이클 알림 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGameCycleDelegate, ERYGameCycleMode, NewGameCycleMode);

/**
 * 게임 사이클 서브 시스템 
 * : 메인 역할 : 
 * 1. 현재 게임이 어떤 상태인지 알려준다. (브로드 캐스트)
 * 2. 현재 게임이 어떤 상태인지 설정한다. (호출 후 수동 세팅 -> 브로드 캐스트)
 */
UCLASS()
class RYGAMESCORESYSTEM_API URYGameCycleSubSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
//======================================================================================================================
#pragma region Getter
	//--------------------
	// Getter
	//--------------------

public:
	static URYGameCycleSubSystem* Get(const UObject* Object);
	
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
	// 게임 사이클 매니저의 델리게이트를 구독하는 API
	UFUNCTION(BlueprintCallable, Category = "RY | Core | GameCycle")
	void SubscribeGameCycleDelegate(UObject* Object, FName FunctionName);
	
	// 게임 사이클 매니저에게 특정 상황임을 알리는 API
	UFUNCTION(BlueprintCallable, Category = "RY | Core | GameCycle")
	FORCEINLINE void SetGameCycleMode(const ERYGameCycleMode& NewGameCycleMode) { CurrentGameCycleMode = NewGameCycleMode; OnGameCycleChangedDelegate.Broadcast(CurrentGameCycleMode);};
	
	// 게임 사이클 매니저로부터 현재 어떤 상황임을 확인받는 API
	UFUNCTION(BlueprintCallable, Category = "RY | Core | GameCycle")
	FORCEINLINE ERYGameCycleMode GetGameCycleMode() const { return CurrentGameCycleMode; };
	
protected:
	// 게임 사이클 알림 델리게이트
	UPROPERTY(BlueprintAssignable, Category = "RY | Core | GameCycle")
	FGameCycleDelegate OnGameCycleChangedDelegate;
	
	// 현재 게임 상태 모드
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RY | Core | GameCycle")
	ERYGameCycleMode CurrentGameCycleMode = ERYGameCycleMode::None;
	
#pragma endregion
//======================================================================================================================
	
};


#pragma region DetailedDescription
// 상세 설명 
/*
 * < 개요 > 
 * 게임 사이클 시스템은 모든 매니저 (하위 서브 시스템 또는 컴포넌트 방식의 매니저들을 말함) 들이 구독해야 하는 최상위 시스템입니다.
 * 
 * < 시스템의 역할 >
 * 게임 사이클 시스템은 모든 최상위 시스템들에게 델리게이트를 통한 이벤트 전달 방식으로 게임의 거시적인 상황에 있어서 어떠한 일을 해야하는지를
 * 알려주는 역할을 합니다. 예를 들어 게임이 진행 중에 "자동 저장" 을 해야 하는 상황이라고 가정하겠습니다. 
 * 
 * - Ex - 
 * "세이브 로드 매니저" 가 5분 마다 자동 저장을 호출하는 상황입니다. 이때 게임 사이클 시스템이 없을 때 이야기해보겠습니다.
 * 
 * -When No GameCycleSystem-
 * 게임 세이브 매니저는 저장이 되어야 하는 모든 컴포넌트, 액터, 서브 시스템들에게 저장을 요청해야 합니다. 즉, 게임 세이브 매니저가 임시적으로
 * 게임 사이클 시스템 역할을 하게 되는 것입니다. 그런데 이 경우에 문제가 생깁니다. 
 * 
 * 첫째, 세이브 매니저가 God Class 가 될 여지가 있습니다. 이는 게임의 모듈화를 헤치고, 확장 시 세이브 매니저가 여러 역할을 겸하게 될 수 있습니다.
 * 예를 들자면 게임이 자동 저장이 끝났음을 알리거나 자동 저장이 시작할 때 특정 이벤트가 필요하다면 이 역시 세이브 매니저가 역할을 겸해야 합니다.
 * 이렇게 될 경우 세이브 매니저가 God Class 가 되는 일은 점점 커지게 됩니다.   
 * 
 * 둘째, 게임 사이클 전체 통제가 힘들어집니다.
 * 언리얼에는 서버에만 존재하는 게임 모드가 있습니다. 이 게임 모드는 게임 스테이트와 달리 게임이 어떻게 진행되는지를 담습니다.
 * 하지만 문제가 있습니다. 게임 모드는 오직 월드 한 곳 (== 하나의 레벨)에서만 존재할 수 있다는 것입니다. 
 * 즉, 만약에 레벨을 옮린다면 게임 인스턴스 및 게임 인스턴스 서브 시스템을 제외한 모든 게 사실상 파괴가 됩니다. 
 * 그러나 게임은 여전히 켜져 있습니다. 그리고 어느 순간 월드가 아니라 게임 자체를 관리해야할 필요성이 느껴지지만 게임 모드는 그게 불가능합니다. 
 * 하지만 게임 인스턴스 서브 시스템은 게임이 시작할 때부터 종료할 때까지 존재하므로 게임 자체를 컨트롤할 수 있습니다.
 * 
 * 
 * 
 * 
 * - 구체적 설명 -
 * - 구체적 설명 -
 * 
 * [1]번 역할의 구체적인 설명 : 1. 현재 게임이 어떤 상태인지 알려준다. (브로드 캐스트)
 * 
 * 현재 게임 사이클 시스템은 총 6가지 열거형이 있습니다.
 * MainTitle : 게임이 맨 처음 켜질 시 나타나는 메인 타이틀 화면임을 알리는 열거형입니다.
 * GameStart : 인 게임 플레이가 시작되었음을 알리는 얼겨형입니다.
 * GamePause : 인 게임 플레이 중 게임이 멈춤 상태가 되었음을 알리는 열거형입니다.
 * GameEnd   : 인 게임 플레이가 종료되었음을 알리는 열거형입니다.
 * ManualSave : 수동 세이브 요청을 받았을 때 열거형입니다.
 * AutoSave   : 자동 세이브 요청을 받았을 때 열거형입니다.
 * 
 * 게임 사이클 시스템은 위의 열거형을 델리게이트를 구독한 모든 하위 매니저들에게 알려줍니다.
 * 하위 매니저들은 switch 문을 통하여 열거형 상태에 따라 자신의 로직을 실행합니다.
 * 
 * 
 * [2]번 역할의 구체적인 설명 : 2. 현재 게임이 어떤 상태인지 설정한다. (호출 후 수동 세팅 -> 브로드 캐스트)
 * 
 * 앞서 언급한 상태는 오직 게임 사이클 매니저만이 바꿀 수 있습니다.
 * 즉, 모든 하위 매니저는 외부 API 호출을 통하여 게임 사이클 시스템에 접근하여 특정 열거형으로 바꿔달라는 요청을 해야합니다.
 * 허나 이 요청은 게임 사이클 매니저를 직접 Get 하여 호출하지 않습니다.
 * Blueprint function Library 클래스에 있는 API를 이용하여 일관적으로 접근합니다.
 * 
 * 
 * ##[주의]##
 * 이전에 언급한 것처럼 모든 하위 매니저는 이 시스템을 구독해야 한다는 필요성이 있다면 반드시 구독해야 합니다.
 * 그러나 바로 위의 글에 적혀 있듯이 매니저 시스템에 바로 접근하지 않습니다. 모든 것은 라이브러리 함수를 통해서 한 단계를 거쳐서 접근합니다.
 * 이렇게 하는 이유는 각 매니저들마다 Get 처리, 유효성 검사, 방어 코드 등등을 전부 라이브러리 한 곳에 담기 위함입니다.
 * 그리 하여 매니저들은 전부 하나의 코드만 존재하게 하여 유지 보수성을 높일 수 있기 때문입니다.
 * 
 * Last Update 2026.02.24
 * 
 */
#pragma endregion
