// GC_ElectricShock_Spark.h
#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Actor.h"
#include "GC_ElectricShock_Spark.generated.h"

class UNiagaraComponent;
class UNiagaraSystem;

/*
	전기 감전 상태 나이아가라 효과 GameplayCue
	- 감전 상태인 플레이어 주위에 찌릿찌릿 스파크 적용
	- Replicated 환경에서 모든 클라이언트에서 실행됨
*/
UCLASS()
class TINYSURVIVOR_API AGC_ElectricShock_Spark : public AGameplayCueNotify_Actor
{
	GENERATED_BODY()
	
public:
	AGC_ElectricShock_Spark();
	
	virtual bool OnActive_Implementation(
		AActor* MyTarget,
		const FGameplayCueParameters& Parameters) override;
	
	virtual bool OnRemove_Implementation(
		AActor* MyTarget,
		const FGameplayCueParameters& Parameters) override;
	
protected:
	// 나이아가라 시스템
	UPROPERTY(EditDefaultsOnly, Category = "Effect Settings")
	class UNiagaraSystem* SparkEffect;
	
	// 생성된 나이아가라 컴포넌트
	UPROPERTY()
	class UNiagaraComponent* SparkComponent;
	
	// 효과 부착 소켓 이름
	UPROPERTY(EditDefaultsOnly, Category = "Effect Settings")
	FName AttachSocketName = "pelvis";
	
	// 효과 위치 오프셋
	UPROPERTY(EditDefaultsOnly, Category = "Effect Settings")
	FVector LocationOffset = FVector(0.0f, 0.0f, 0.0f);
	
	// 효과 크기
	UPROPERTY(EditDefaultsOnly, Category = "Effect Settings")
	float SparkScale = 1.0f;
};
