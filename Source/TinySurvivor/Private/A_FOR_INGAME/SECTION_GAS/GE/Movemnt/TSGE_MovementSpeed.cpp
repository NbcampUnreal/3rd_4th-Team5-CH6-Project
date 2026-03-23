// All CopyRight From YulRyongGameStudio //


#include "A_FOR_INGAME/SECTION_GAS/GE/Movemnt/TSGE_MovementSpeed.h"
#include "A_FOR_INGAME/SECTION_GAS/Comp/TSAttributeSet.h"

UTSGE_MovementSpeed::UTSGE_MovementSpeed()
{
	// 1. 지속 시간 설정 (예: 10초 동안 유지)
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	// 2. 주기 설정 (1초마다 실행)
	Period = 1.0f;
    
    // 3. 예측 설정 
	bExecutePeriodicEffectOnApplication = true;
}
