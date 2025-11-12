// DecayManager.h
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "DecayManager.generated.h"

class UTimeTickManager;

// 부패도 업데이트 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDecayTick);

/*
	부패도 매니저
	
	역할:
	- TimeTickManager의 1초 신호를 구독하여 부패도 관련 신호 중계
	- 부패물 아이템 ID 제공 (고정 상수)
	- 부패도 관련 공통 로직 처리 계층
	
	네트워크 동작:
	- 서버에서만 생성 및 실행
	- 구독자: 아이템 액터, 인벤토리 컴포넌트
	- 구독자들은 서버에서만 이 매니저를 구독 (BeginPlay에서 HasAuthority 체크)
	
	설계 의도:
	- TimeTickManager와 구독자들 사이의 중간 계층 (Mediator 패턴)
	- 타이머 의존성 제거: 구독자들은 DecayManager만 알면 됨
	- 확장성: 향후 부패도 관련 공통 로직을 여기서 추가 가능
	
	부패 처리 흐름:
		1. TimeTickManager → 1초 신호 → DecayManager::OnSecondTick()
		2. DecayManager → OnDecayTick.Broadcast()
		3. 구독자들(인벤토리/액터) → 부패도 계산 및 처리 (서버)
		4. 부패물 전환 시 → 리플리케이션 → 클라이언트 UI 업데이트
		
	사용 예시:
		// TSInventoryMasterComponent.cpp
		void UTSInventoryMasterComponent::BeginPlay()
		{
			if (GetOwner()->HasAuthority())  // 서버에서만
			{
				UDecayManager* DecayMgr = GetWorld()->GetSubsystem<UDecayManager>();
				DecayMgr->OnDecayTick.AddDynamic(this, &UTSInventoryMasterComponent::OnDecayTick);
			}
		}
		...
		void UTSInventoryMasterComponent::OnDecayTick()
		{
			// 서버에서만 실행됨
			// 부패도 계산...
			if (부패 완료)
			{
				int32 RottenID = DecayMgr->GetDecayItemID(); // 307
				Slot.StaticDataID = RottenID;
			}
		}
*/
UCLASS()
class TINYSURVIVOR_API UDecayManager : public UWorldSubsystem
{
	GENERATED_BODY()
	
#pragma region Lifecycle
public:
	//========================================
	// Lifecycle
	//========================================
	
	/*
		서버 전용 생성 제약
		- TimeTickManager와 동일한 조건 (서버만)
	*/
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	
	/*
		Subsystem 초기화
		- TimeTickManager 의존성 명시
		- OnSecondTick 구독
	*/
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	/*
		Subsystem 정리
		- TimeTickManager 구독 해제
	*/
	virtual void Deinitialize() override;
#pragma endregion
	
#pragma region Delegate
public:
	//========================================
	// Delegate
	//========================================
	
	/*
		부패도 업데이트 이벤트
		
		브로드캐스트 타이밍:
		- TimeTickManager의 1초 신호를 받을 때마다 발생
		- 서버에서만 발생
		
		주의사항:
		- 구독은 반드시 서버에서만 수행 (BeginPlay에서 HasAuthority 체크)
		- 콜백 함수도 서버에서만 실행됨
	*/
	UPROPERTY(BlueprintAssignable, Category="Decay")
	FOnDecayTick OnDecayTick;
#pragma endregion
	
#pragma region DecayItemIDGetter
public:
	//========================================
	// Decay Item ID Getter
	//========================================
	
	/*
		고정 부패물 아이템 ID 반환
		
		용도:
		- 부패 완료 시 아이템을 부패물로 전환할 때 사용
		- 예: Slot.StaticDataID = DecayMgr->GetDecayItemID();
		
		기획 협의:
		- 부패물 ID는 고정 상수 (307)
		- 모든 부패 가능 아이템은 동일한 부패물로 전환
		
		네트워크:
		- 서버/클라이언트 구분 없이 호출 가능
		- 실제로는 서버에서만 사용됨 (부패 처리가 서버 전용이므로)
		
		@return 부패물 아이템 ID (현재: 307)
	*/
	UFUNCTION(BlueprintCallable, Category="Decay")
	int32 GetDecayItemID() const { return DecayItemID; }
#pragma endregion
	
#pragma region Internal
private:
	//========================================
	// Internal
	//========================================
	
	// TimeTickManager 캐시
	UPROPERTY()
	TObjectPtr<UTimeTickManager> TimeTickManager;
	
	/*
		TimeTickManager의 1초 신호를 받는 콜백
		- 구독자들에게 부패도 업데이트 신호 중계
	*/
	UFUNCTION()
	void OnSecondTick();
#pragma endregion
	
#pragma region Constants
private:
	//========================================
	// Constants
	//========================================
	
	/*
		고정 부패물 아이템 ID
		
		주의:
		- constexpr: 컴파일 타임 상수
		- 변경 금지 (기획 협의 완료)
	*/
	static constexpr int32 DecayItemID = 307;
#pragma endregion
};
