#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "TSErosionSubSystem.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(ErosionManager, Log, All);

// 주석 달자.

class UErosionConfigData;
class UErosionLightSourceComponent;
class AErosionStateInfo;

UENUM(BlueprintType)
enum class EErosionStage : uint8
{
	None,
	Stage30,
	Stage60,
	Stage90,
	Max
};

// 침식도 이벤트 관련 델리게이트 : 다른 매니저에서 이를 구독하여 30 / 60 / 90 / 100에 따른 이벤트 변화 수신.
// 현재는 그냥 값을 넘기는 건데 추후 확실하게 스테이지 별로 할건지, 아니면 수치에 따라서 유연하게 가져갈 건지 정해야함.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnErosionChangedDelegate, float, FinalValue);

// 빛 관련 건축물 구독용 델리게이트 : 빛 관련 건축물은 반드시 태어나면 이 델리게이트 구독해야 한다.
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnErosionLightSourceSubDelegate);

// 빛 관련 건축물 수신용 델리게이트 : 침식도 시스템이 빛 관련 건축물로 부터 데이터를 받기 위한 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnErosionLightSourceBroadcastDelegate, float, AddValue);

/*
 * 침식도 매니저 
 */
UCLASS()
class TINYSURVIVOR_API UTSErosionSubSystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//-----------------------------
	// UTSErosionSubSystem 라이프사이클
	//-----------------------------
	
public:
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	virtual void Deinitialize() override;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//-----------------------------
	// UTSErosionSubSystem 외부 API (inGame에서 호출)
	//-----------------------------

public:
	// 게터
	UFUNCTION(BlueprintCallable, Category="Erosion | API")
	static UTSErosionSubSystem* GetErosionSubSystem(const UObject* WorldContextObject);
	
	// 즉시 가감 API  : 외부 이벤트 입력 (즉시 침식도 변경 : ex몬스터 처치 시에 활용)
	UFUNCTION(BlueprintCallable, Category="Erosion | API")
	void AddOrSubtractErosion(float AddOrSubtract); // (+/-)

	// 자연 침식도 세팅 값 변경 API
	UFUNCTION(BlueprintCallable, Category="Erosion|API")
	void SetNaturalErosion(float NaturalErosionRate, float NaturalErosionSpeed);
	
	// 침식도 리셋 (예: 생존 성공 시)
	UFUNCTION(BlueprintCallable, Category="Erosion|API")
	void ResetErosion();

	// 침식도 이벤트 관련 델리게이트 
	UPROPERTY(BlueprintAssignable, Category="Erosion|Events")
	FOnErosionChangedDelegate OnErosionChangedDelegate;

	// 빛 관련 건축물이 필수적으로 구독해야하는 델리게이트 
	UPROPERTY(BlueprintAssignable, Category="Erosion|Events")
	FOnErosionLightSourceSubDelegate OnErosionLightSourceSubDelegate;

	// 빛 관련 건축물 또는 AI 몬스터나 거인이 매니저에게 브로드캐스트하는 델리게이트 (매니저만 구독하는 것! 절대 다른 곳에서 이걸 구독하지 마시오.)
	UPROPERTY(BlueprintAssignable, Category="Erosion|Events")
	FOnErosionLightSourceBroadcastDelegate OnErosionLightSourceBroadcastDelegate;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	

	//-----------------------------
	// UTSErosionSubSystem 내부 유틸 함수 
	//-----------------------------
	
protected:
	// 내부 상태 보조 함수
	bool IsServerAuth() const;

	// 로직 단위 처리 함수
	void ApplyNaturalIncrease(); // 자연 상승 

	// UI 용
	void EnsureStateInfoExists(); // UI 업데이트 액터 생성 함수 
	void BroadcastToStateInfo();  // UI 업데이트 발신 함수 
	
	// 이벤트용
	void OnErosionChangedBroadcast(); // 침식도 수준에 따라 이벤트 발신
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//-----------------------------
	// UTSErosionSubSystem 타이머
	//-----------------------------

protected:
	// 타이머 (침식도 증가 타이머)
	void UpdateErosion();  

	FTimerHandle ErosionTimerHandle;     // 타이머 핸들
	float TickInterval = 1.0f;           // 1초 주기 (모든 침식도 관련 X당 Y증감의 최소 주기이어야 한다.)
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	

	//-----------------------------
	// UTSErosionSubSystem 데이터 구성 요소 (설정 / 상태 / 이벤트)
	//-----------------------------
	
protected:
	// 설정 DataAsset (밸런스 수치)
	UPROPERTY()
	TObjectPtr<UErosionConfigData> Config;

	// 서버에서 관리되는 상태 정보 (Replicated Info Actor)
	UPROPERTY()
	TObjectPtr<AErosionStateInfo> StateInfo;

	// 마지막 침식도
	float LastErosion = 0.f;
	
	// 현재 침식도 
	float CurrentErosion = 0.f;
	
	// 자연 침식도 내부 누적 시간 (for 자연 상승)
	float NaturalElapsedTime = 0.f;
	
	// 자연 침식도 (캐싱)
	float CachedCurrentNaturalErosion = 1.f;
	
	// 현재 침식 속도 (캐싱)
	float CachedCurrentNaturalErosionSpeed = 10.f;

	// MAx 영향 on/off
	bool bMaxInfluenceActive = false;

	// 현재 침식도 스테이지 (30/60/90/100==MAX)
	EErosionStage CurrentStage = EErosionStage::None;

	// 디버깅 캐싱
	bool bShowDebug = false;

public:
	bool GetShowDebug() const { return bShowDebug; }
};
