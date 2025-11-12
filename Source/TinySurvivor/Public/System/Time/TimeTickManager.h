// TimeTickManager.h
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "TimeTickManager.generated.h"

// 1초 타이머 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSecondTick);

/*
	시간 틱 매니저
	
	역할:
	- 타이머를 사용하여 1초마다 FOnSecondTick 델리게이트 브로드캐스트
	- 범용 시간 기반 시스템들이 이 신호를 구독하여 자체 로직 실행
	- 예: DecayManager(부패도)
	
	네트워크 동작:
	- 서버에서만 생성 및 실행 (ShouldCreateSubsystem 제약)
	- 클라이언트는 생성되지 않음 (불필요한 타이머 중복 방지)
	
	설계 의도:
	- 성능 최적화: 단일 타이머로 여러 시스템 구동
	- 관심사 분리: 각 시스템은 독립적인 매니저로 분리
	- 확장 가능: 새로운 시간 기반 시스템 추가 시 이 매니저만 구독하면 됨
	
	사용 예시:
		// DecayManager.cpp
		void UDecayManager::Initialize(FSubsystemCollectionBase& Collection)
		{
			UTimeTickManager* TimeTick = GetWorld()->GetSubsystem<UTimeTickManager>();
			TimeTick->OnSecondTick.AddDynamic(this, &UDecayManager::OnSecondTick);
		}
*/
UCLASS()
class TINYSURVIVOR_API UTimeTickManager : public UWorldSubsystem
{
	GENERATED_BODY()
	
#pragma region Lifecycle
public:
	//========================================
	// Lifecycle
	//========================================
	
	/*
		Subsystem 생성 여부를 결정하는 가상 함수
		
		@param Outer WorldSubsystem의 경우 UWorld가 전달됨
		@return true면 Subsystem 생성, false면 생성 안 함
		
		목적:
		- 서버 전용 생성 제약 (클라이언트 제외)
		- 게임 월드에서만 생성 (에디터/PIE 제외)
	*/
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	
	/*
		Subsystem 초기화
		- 1초 반복 타이머 설정
		- 서버에서만 호출됨 (ShouldCreateSubsystem에서 필터링)
	*/
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	/*
		Subsystem 정리
		- 타이머 해제
		- 레벨 전환 또는 게임 종료 시 호출
	*/
	virtual void Deinitialize() override;
#pragma endregion
	
#pragma region Delegate
public:
	//========================================
	// Delegate
	//========================================
	
	/*
		1초 타이머 이벤트
		
		브로드캐스트 타이밍:
		- 정확히 1초마다 호출 (타이머 기반)
		- 서버에서만 발생 (클라이언트는 이 매니저 자체가 없음)
		
		구독자:
		- 시간 기반 시스템들 (DecayManager 등)
	*/
	UPROPERTY(BlueprintAssignable, Category="Time")
	FOnSecondTick OnSecondTick;
#pragma endregion
	
#pragma region Internal
private:
	//========================================
	// Internal
	//========================================
	
	// 타이머 핸들 (중복 등록 방지 및 해제 시 사용)
	FTimerHandle SecondTimerHandle;
	
	/*
		타이머 콜백 함수 (1초마다 호출)
		- OnSecondTick 델리게이트 브로드캐스트
	*/
	void BroadcastSecondTick();
#pragma endregion
};