// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ErosionLightSourceInterface.h"
#include "GameFramework/Actor.h"
#include "ErosionLightSourceSubActor.generated.h"

class UErosionLightSourceComponent;
class UPointLightComponent;

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
class TINYSURVIVOR_API AErosionLightSourceSubActor : public AActor, public IErosionLightSourceInterface
{
	GENERATED_BODY()

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

	// 포인트 라이트 사용 변수
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ErosionLightSource")
	bool bUsePointLight = false;
	
	// 포인트 라이트 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ErosionLightSource")
	TObjectPtr<UPointLightComponent> PointLightComponent;

	// 밝기 조절 단계 (포인트 라이트)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ErosionLightSource")
	FLightScaleStep LightScaleStep;
	
	// 스태틱 메쉬(라이트 미사용 시)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ErosionLightSource")
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent;
	
	// 밝기 조절 단계 (스태틱 메쉬 머티리얼)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ErosionLightSource")
	FLightScaleStep EmissiveScaleStep;
	
	// 머리티리얼 인스턴스
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ErosionLightSource")
	UMaterialInstanceDynamic* LightMID = nullptr;
};
