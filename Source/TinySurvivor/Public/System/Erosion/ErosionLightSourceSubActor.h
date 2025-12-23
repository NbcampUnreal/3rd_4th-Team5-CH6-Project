// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ErosionLightSourceInterface.h"
#include "GameFramework/Actor.h"
#include "Building/Actor/TSBuildingActorBase.h"
#include "ErosionLightSourceSubActor.generated.h"

class UErosionLightSourceComponent;

USTRUCT(BlueprintType)
struct FLightScaleStep
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ErosionLightSource")
	float StepLess30 = 10.f	;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ErosionLightSource")
	float StepMore30Less60 = 8.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ErosionLightSource")
	float StepMore60Less90 = 6.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ErosionLightSource")
	float StepMore90LessMax = 4.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ErosionLightSource")
	float StepMax = 2.f;
};

UCLASS()
class TINYSURVIVOR_API AErosionLightSourceSubActor : public ATSBuildingActorBase, public IErosionLightSourceInterface
{
	GENERATED_BODY()
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// ===============================
	// ATSInteractionActorBase 함수 오버라이드 
	// 빌딩 액터 멤버 변수
	// ===============================
	
public:
	virtual void InitializeFromBuildingData(const FBuildingData& BuildingInfo, const int32 StaticDataID) override;
	float GetLightRadius() const { return LightRadius; }
protected:
	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "ErosionLightSource")
	float LightRadius = 0.f;
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// ===============================
	// UErosionLightSourceComponent 라이프 사이클
	// ===============================
	
public:
	AErosionLightSourceSubActor();
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	float GetLightscale(){ return LightScale; }
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// ===============================
	// UErosionLightSourceComponent 침식도 구독 컴포넌트 
	// ===============================

public:
	UFUNCTION(BlueprintCallable, Category = "ErosionLightSource")
	UErosionLightSourceComponent* GetErosionLightSourceComponent() { return ErosionLightSourceComponent;}

protected:
	// 침식도 구독 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ErosionLightSource")
	TObjectPtr<UErosionLightSourceComponent> ErosionLightSourceComponent;
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// ===============================
	// UErosionLightSourceComponent 라이트 컴포넌트 
	// ===============================

	// IErosionLightSourceInterface ~
	virtual void SetErosionLightSource_Implementation(bool bEnable) override;
	// ~ IErosionLightSourceInterface
	
	// 침식도 이벤트 구독 함수 
	UFUNCTION()
	void ChangeLightScaleByErosion(float CurrentErosionScale);

	// 밝기 조절 함수 
	void SetLightScale(float scale);

	// 클라이언트 밝기 동기화 
	UFUNCTION()
	void OnRep_LightScale();

	// 밝기 변수 
	UPROPERTY(ReplicatedUsing = OnRep_LightScale)
	float LightScale = 1.0f;

	// 침식도에 따라 빛 조절 여부
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ErosionLightSource")
	bool bSetLightScaleByErosion = true;
	
	// 밝기 조절 단계 (스태틱 메쉬 머티리얼)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ErosionLightSource")
	FLightScaleStep EmissiveScaleStep;
	
	// 머리티리얼 인스턴스
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ErosionLightSource")
	UMaterialInstanceDynamic* LightMID = nullptr;
	
	// 디버깅
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Erosion | Debug")
	bool bShowDebug = false;
};
