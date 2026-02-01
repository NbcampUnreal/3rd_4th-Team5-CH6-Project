// GE_ElectricShock.h
#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "GE_ElectricShock.generated.h"

/*
	전기 감전 상태 GameplayEffect
	
	- State.Status.ElectricShock 태그 부여
	- 시각 효과 트리거 (3종류)
		1. VFX (전기 스파크 이펙트를 나타내는 나이아가라 시스템 적용)
		2. Material (해골 모양의 머티리얼로 깜빡임 효과와 감전 사운드 적용)
		3. PostProcess (로컬 플레이어 화면에 감전 상태를 나타내는 노이즈와 깜빡임 효과 적용)
*/
UCLASS()
class TINYSURVIVOR_API UGE_ElectricShock : public UGameplayEffect
{
	GENERATED_BODY()
	
public:
	UGE_ElectricShock();
	
private:
	// GC 직접 참조 (강제 로드)
	UPROPERTY()
	TSubclassOf<class AGameplayCueNotify_Actor> ElectricShockSparkCueClass;
	
	UPROPERTY()
	TSubclassOf<class AGameplayCueNotify_Actor> ElectricShockMaterialCueClass;
	
	UPROPERTY()
	TSubclassOf<class AGameplayCueNotify_Actor> ElectricShockStatusCueClass;
};
