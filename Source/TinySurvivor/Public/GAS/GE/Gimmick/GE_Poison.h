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
	- 시각 효과 트리거 (3종류)
		1. VFX (독 거품을 나타내는 나이아가라 시스템 적용)
		2. Material (독 중독 상태를 강조하기 위해 초록빛 머티리얼로 변경)
		3. PostProcess (로컬 플레이어에게만 독 상태 필터 적용)
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
	TSubclassOf<class AGameplayCueNotify_Actor> PoisonBubbleCueClass;
	
	UPROPERTY()
	TSubclassOf<class AGameplayCueNotify_Actor> PoisonMaterialCueClass;
	
	UPROPERTY()
	TSubclassOf<class AGameplayCueNotify_Actor> PoisonStatusCueClass;
};
