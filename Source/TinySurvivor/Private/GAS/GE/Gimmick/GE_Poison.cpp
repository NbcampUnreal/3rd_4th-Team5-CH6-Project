// GE_Poison.cpp
#include "GAS/GE/Gimmick/GE_Poison.h"

#include "GameplayTags/GameplayCueTags.h"
#include "GAS/AttributeSet/TSAttributeSet.h"
#include "GAS/GC/Gimmick/GC_Poison_Status.h"
#include "GAS/GC/Gimmick/GC_Poison_Material.h"
#include "GAS/GC/Gimmick/GC_Poison_Bubble.h"

UGE_Poison::UGE_Poison()
{
	// Duration 설정 (10초)
	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	DurationMagnitude = FScalableFloat(10.0f);
	
	// Period 설정 (1초마다 실행)
	Period = 1.0f;
	bExecutePeriodicEffectOnApplication = true; // 적용 즉시 첫 틱 실행
	PeriodicInhibitionPolicy = EGameplayEffectPeriodInhibitionRemovedPolicy::NeverReset;
	
	//==========================================================================
	// Granted Tags: State.Status.Poison
	// 독 상태 태그 부여는 에디터에서 설정.
	// C++에서 직접 설정하는 게 deprecated
	//==========================================================================
	// FInheritedTagContainer TagContainer;
	// TagContainer.Added.AddTag(AbilityTags::TAG_State_Status_Poison);
	// TagContainer.CombinedTags.AddTag(AbilityTags::TAG_State_Status_Poison);
	
	//==========================================================================
	// Modifier: Health -1 (주기적, Periodic)
	//==========================================================================
	FGameplayModifierInfo HealthModifier;
	HealthModifier.ModifierMagnitude = FScalableFloat(-1.0f);
	HealthModifier.ModifierOp = EGameplayModOp::Additive;
	HealthModifier.Attribute = UTSAttributeSet::GetHealthAttribute();
	Modifiers.Add(HealthModifier);
	
	//==========================================================================
	// GameplayCue 등록
	//==========================================================================
	// 1. VFX 효과 (독 거품을 나타내는 나이아가라 시스템 적용)
	FGameplayEffectCue BubbleCue;
	BubbleCue.GameplayCueTags.AddTag(CueTags::TAG_GameplayCue_Poison_Bubble);
	GameplayCues.Add(BubbleCue);
	
	// 2. Material 효과 (독 중독 상태를 강조하기 위해 초록빛 머티리얼로 변경)
	FGameplayEffectCue MaterialCue;
	MaterialCue.GameplayCueTags.AddTag(CueTags::TAG_GameplayCue_Poison_Material);
	GameplayCues.Add(MaterialCue);
	
	// 3. PostProcess 효과 (로컬 플레이어에게만 독 상태 필터 적용)
	FGameplayEffectCue StatusCue;
	StatusCue.GameplayCueTags.AddTag(CueTags::TAG_GameplayCue_Poison_Status);
	GameplayCues.Add(StatusCue);
	
	// Stacking 설정
	//StackingType = EGameplayEffectStackingType::None;
	
	//==========================================================================
	// GameplayCue 클래스 강제 참조 (메모리에 로드됨: Asset Manager 로딩 보장)
	//==========================================================================
	PoisonBubbleCueClass  = AGC_Poison_Bubble::StaticClass();
	PoisonMaterialCueClass = AGC_Poison_Material::StaticClass();
	PoisonStatusCueClass = AGC_Poison_Status::StaticClass();
}
