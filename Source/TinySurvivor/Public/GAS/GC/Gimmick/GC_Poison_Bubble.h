// GC_Poison_Bubble.h
#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Actor.h"
#include "GC_Poison_Bubble.generated.h"

/*
	독 상태 나이아가라 효과 GameplayCue
	- 독 상태인 플레이어 주위에 초록빛 거품 효과 적용
	- Replicated 환경에서 모든 클라이언트에서 실행됨
*/
UCLASS()
class TINYSURVIVOR_API AGC_Poison_Bubble : public AGameplayCueNotify_Actor
{
	GENERATED_BODY()
	
public:
	AGC_Poison_Bubble();
	
	virtual bool OnActive_Implementation(
		AActor* MyTarget,
		const FGameplayCueParameters& Parameters) override;
	
	virtual bool OnRemove_Implementation(
		AActor* MyTarget,
		const FGameplayCueParameters& Parameters) override;

protected:
	// 나이아가라 시스템
	UPROPERTY(EditDefaultsOnly, Category = "Effect Settings")
	class UNiagaraSystem* BubbleEffect;
	
	// 생성된 나이아가라 컴포넌트
	UPROPERTY()
	class UNiagaraComponent* BubbleComponent;
	
	// 효과 위치 오프셋 (캐릭터 발 위치 등)
	UPROPERTY(EditDefaultsOnly, Category = "Effect Settings")
	FVector LocationOffset = FVector(0.0f, 0.0f, 0.0f);
	
	// 효과 크기
	UPROPERTY(EditDefaultsOnly, Category = "Effect Settings")
	float BubbleScale = 1.0f;
};
