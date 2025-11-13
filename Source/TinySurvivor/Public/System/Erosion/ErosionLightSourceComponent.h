// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ErosionLightSourceComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TINYSURVIVOR_API UErosionLightSourceComponent : public UActorComponent
{
	GENERATED_BODY()
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// ===============================
	// UErosionLightSourceComponent 라이프 사이클
	// ===============================
	
protected:
	UErosionLightSourceComponent();
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// ===============================
	// 설정값 (건물 데이터에서 가져와야 함!!!!!!!)
	// ===============================

protected:
	// 침식도 감소량 주기 (초)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Erosion | Config")
	float MaintenanceEffectTime = 10.0f;

	// 침식도 감소량 (음수 -> 감소)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Erosion | Config")
	float MaintenanceEffectValue = -2.0f;

	// 누적 시간 (침식도 감소량 주기에 따른 스택 변수)
	float AccumulatedTime = 0;
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// ===============================
	// 침식도 관련 
	// ===============================
	
public:
	// 나중에 건물 데이터로부터 값 초기화할 함수 : 몇 초마다 몇을 감소할 거냐?
	UFUNCTION(BlueprintCallable)
	void InitializeFromBuildingData();

	// 상태 업데이트 (아이템 데이터로부터 유지비가 가능한 상태인지 아닌지 받아옴)
	UFUNCTION(BlueprintCallable)
	bool UpdateActiveFlag();

	// 매 1초마다 침식도 매니저에서 수신되면 호출될 함수
	UFUNCTION()
	void ReceiveErosionCheckTime();

	// 침식도 감소 실행 : 스택이 쌓이면 이 함수를 호출
	void OnIntervalTriggered();

	// 상태 변수 (테스트용 : 추후 삭제 예정)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Erosion | Debug")
	bool bIsActiveLightForDebug = true;

	// 디버깅
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Erosion | Debug")
	bool bShowDebug = false;
};
