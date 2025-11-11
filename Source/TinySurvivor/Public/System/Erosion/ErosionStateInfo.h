// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ErosionStateInfo.generated.h"

// UI 업데이트용 델리게이트 
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnErosionChangedUIDelegate, float, FinalValue);

/*
 * 침식도 UI info 클래스
 * 호스트 / 클라이언트 침식도 통합 업데이트용 클래스 
 */
UCLASS()
class TINYSURVIVOR_API AErosionStateInfo : public AActor
{
	GENERATED_BODY()
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//-----------------------------
	// AErosionStateInfo 라이프사이클
	//-----------------------------
	
public:
	AErosionStateInfo();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//-----------------------------
	// AErosionStateInfo 변수 업데이트
	//-----------------------------

public:
	// 서버 전용 set (Subsystem에서 호출)
	void ServerSetErosion(float NewValue);

protected:
	// 현재 침식도 값 (서버에서만 수정, 클라로 복제)
	UPROPERTY(ReplicatedUsing=OnRep_CurrentErosion)
	float CurrentErosion = 0.f;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//-----------------------------
	// AErosionStateInfo UI 업데이트 (델리게이트, OnRep
	//-----------------------------

public:
	UPROPERTY(BlueprintAssignable)
	FOnErosionChangedUIDelegate OnErosionChangedUIDelegate;

protected:
	UFUNCTION()
	void OnRep_CurrentErosion();

};
