// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "A_FOR_INGAME/SECTION_GAS/Comp/TSAttributeSet.h"
#include "TSGE_ExeCalBase.generated.h"

struct TSMovementStatics
{
	// 1. 캡처 정의 변수 선언
	DECLARE_ATTRIBUTE_CAPTUREDEF(Health);
	DECLARE_ATTRIBUTE_CAPTUREDEF(MaxHealth);
	
	DECLARE_ATTRIBUTE_CAPTUREDEF(Stamina);
	DECLARE_ATTRIBUTE_CAPTUREDEF(MaxStamina);
	
	DECLARE_ATTRIBUTE_CAPTUREDEF(Mentality);
	DECLARE_ATTRIBUTE_CAPTUREDEF(MaxMentality);
	
	DECLARE_ATTRIBUTE_CAPTUREDEF(Hunger);
	DECLARE_ATTRIBUTE_CAPTUREDEF(MaxHunger);
	
	DECLARE_ATTRIBUTE_CAPTUREDEF(Thirst);
	DECLARE_ATTRIBUTE_CAPTUREDEF(MaxThirst);
	
	DECLARE_ATTRIBUTE_CAPTUREDEF(Temperature);
	DECLARE_ATTRIBUTE_CAPTUREDEF(AttackPower);
	DECLARE_ATTRIBUTE_CAPTUREDEF(DefensePower);
	DECLARE_ATTRIBUTE_CAPTUREDEF(MovementSpeed); 
	DECLARE_ATTRIBUTE_CAPTUREDEF(TotalWeight); 
	
	TSMovementStatics()
	{
		// 2. 캡처 정의 초기화
		// 파라미터: (클래스명, 속성명, Source/Target, 스냅샷 여부)
		
		DEFINE_ATTRIBUTE_CAPTUREDEF(UTSAttributeSet, Health, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UTSAttributeSet, MaxHealth, Target, false);
	
		DEFINE_ATTRIBUTE_CAPTUREDEF(UTSAttributeSet, Stamina, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UTSAttributeSet, MaxStamina, Target, false);
	
		DEFINE_ATTRIBUTE_CAPTUREDEF(UTSAttributeSet, Mentality, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UTSAttributeSet, MaxMentality, Target, false);
	
		DEFINE_ATTRIBUTE_CAPTUREDEF(UTSAttributeSet, Hunger, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UTSAttributeSet, MaxHunger, Target, false);
	
		DEFINE_ATTRIBUTE_CAPTUREDEF(UTSAttributeSet, Thirst, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UTSAttributeSet, MaxThirst, Target, false);
	
		DEFINE_ATTRIBUTE_CAPTUREDEF(UTSAttributeSet, Temperature, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UTSAttributeSet, AttackPower, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UTSAttributeSet, DefensePower, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UTSAttributeSet, MovementSpeed, Target, false); 
		DEFINE_ATTRIBUTE_CAPTUREDEF(UTSAttributeSet, TotalWeight, Target, false); 
	}
};

// static 함수로 구조체 싱글톤처럼 접근
static const TSMovementStatics& MoveStatics()
{
	static TSMovementStatics Statics;
	return Statics;
}

/**
 * 
 */
UCLASS()
class TINYSURVIVOR_API UTSGE_ExeCalBase : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
};
