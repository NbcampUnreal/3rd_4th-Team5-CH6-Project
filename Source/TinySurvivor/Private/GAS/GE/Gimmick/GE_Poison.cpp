// GE_Poison.cpp
#include "GAS/GE/Gimmick/GE_Poison.h"

#include "GameplayTags/AbilityGameplayTags.h"
#include "GAS/AttributeSet/TSAttributeSet.h"
#include "GAS/GC/Gimmick/GameplayCue_Status_Poisoned.h"

UGE_Poison::UGE_Poison()
{
	// GC 클래스 직접 참조 (강제 로드)
	PoisonCueClass = AGameplayCue_Status_Poisoned::StaticClass();
	
	// Duration 설정 (10초)
	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	DurationMagnitude = FScalableFloat(10.0f);
	
	// Period 설정 (1초마다 실행)
	Period = 1.0f;
	bExecutePeriodicEffectOnApplication = true; // 적용 즉시 첫 틱 실행
	PeriodicInhibitionPolicy = EGameplayEffectPeriodInhibitionRemovedPolicy::NeverReset;
	
	// 독 상태 태그 부여
	FInheritedTagContainer TagContainer;
	TagContainer.Added.AddTag(AbilityTags::TAG_State_Status_Poison);
	TagContainer.CombinedTags.AddTag(AbilityTags::TAG_State_Status_Poison);
	
	// 독 상태 태그 부여는 에디터에서 설정.
	// C++에서 직접 설정하는 게 deprecated
	
	// Health -1 감소 Modifier 설정
	FGameplayModifierInfo HealthModifier;
	HealthModifier.ModifierMagnitude = FScalableFloat(-1.0f);
	HealthModifier.ModifierOp = EGameplayModOp::Additive;
	HealthModifier.Attribute = UTSAttributeSet::GetHealthAttribute();
	Modifiers.Add(HealthModifier);
	
	// GameplayCue 설정 (시각 효과)
	FGameplayEffectCue PoisonCue;
	PoisonCue.GameplayCueTags.AddTag(AbilityTags::TAG_GameplayCue_Status_Poisoned);
	PoisonCue.MinLevel = 0.0f;
	PoisonCue.MaxLevel = 0.0f;
	GameplayCues.Add(PoisonCue);
	
	// Stacking 설정 (사진에서 None으로 되어있음)
	StackingType = EGameplayEffectStackingType::None;
}
