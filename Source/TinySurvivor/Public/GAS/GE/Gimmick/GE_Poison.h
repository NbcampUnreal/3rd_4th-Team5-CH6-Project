// GE_Poison.h
#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "GE_Poison.generated.h"

/*
	독 상태 GameplayEffect
	- 10초 동안 지속
	- 1초마다 Health -1 감소
	- State.Status.Poison 태그 부여
	- GameplayCue.Status.Poisoned 시각 효과 트리거
*/
UCLASS()
class TINYSURVIVOR_API UGE_Poison : public UGameplayEffect
{
	GENERATED_BODY()
	
public:
	UGE_Poison();
	
private:
	// GC 직접 참조 (강제 로드)
	UPROPERTY()
	TSubclassOf<class AGameplayCueNotify_Actor> PoisonCueClass;
};
