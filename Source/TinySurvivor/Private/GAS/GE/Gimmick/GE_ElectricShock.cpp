// GE_ElectricShock.cpp
#include "GAS/GE/Gimmick/GE_ElectricShock.h"

#include "GameplayTags/GameplayCueTags.h"
#include "GAS/AttributeSet/TSAttributeSet.h"
#include "GAS/GC/Gimmick/GC_ElectricShock_Material.h"
#include "GAS/GC/Gimmick/GC_ElectricShock_Spark.h"
#include "GAS/GC/Gimmick/GC_ElectricShock_Status.h"

UGE_ElectricShock::UGE_ElectricShock()
{
	//==========================================================================
	// Duration 설정: 짧은 지속 시간 (1.5초 - 깜빡임 효과 시간)
	// 시각 효과를 유지하기 위해 Duration 사용
	//==========================================================================
	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	DurationMagnitude = FScalableFloat(1.5f);
	
	// Period 없음
	bExecutePeriodicEffectOnApplication = false;
	
	//==========================================================================
	// Granted Tags: State.Status.ElectricShock
	// 전기 마비 상태 태그 부여는 에디터에서 설정.
	// C++에서 직접 설정하는 게 deprecated
	//==========================================================================
	
	//==========================================================================
	// Modifier: ex) Health -5
	// Duration이 아닌 별도 GE에서 instant로 적용해야함.
	//==========================================================================
	
	//==========================================================================
	// GameplayCue 등록
	//==========================================================================
	// 1. VFX 효과 (전기 스파크 이펙트를 나타내는 나이아가라 시스템 적용)
	FGameplayEffectCue SparkCue;
	SparkCue.GameplayCueTags.AddTag(CueTags::TAG_GameplayCue_ElectricShock_Spark);
	GameplayCues.Add(SparkCue);
	
	// 2. Material 효과 (해골 모양의 머티리얼로 깜빡임 효과와 감전 사운드 적용)
	FGameplayEffectCue MaterialCue;
	MaterialCue.GameplayCueTags.AddTag(CueTags::TAG_GameplayCue_ElectricShock_Material);
	GameplayCues.Add(MaterialCue);
	
	// 3. PostProcess 효과 (로컬 플레이어 화면에 감전 상태를 나타내는 노이즈와 깜빡임 효과 적용)
	FGameplayEffectCue StatusCue;
	StatusCue.GameplayCueTags.AddTag(CueTags::TAG_GameplayCue_ElectricShock_Status);
	GameplayCues.Add(StatusCue);
	
	//==========================================================================
	// Stacking 설정 (선택사항)
	// 여러 번 맞아도 중첩되지 않고 갱신되도록
	//==========================================================================
	// StackingType = EGameplayEffectStackingType::AggregateBySource;
	// StackLimitCount = 1;
	// StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	
	//==========================================================================
	// GameplayCue 클래스 강제 참조 (메모리에 로드됨: Asset Manager 로딩 보장)
	//==========================================================================
	ElectricShockSparkCueClass = AGC_ElectricShock_Spark::StaticClass();
	ElectricShockMaterialCueClass = AGC_ElectricShock_Material::StaticClass();
	ElectricShockStatusCueClass = AGC_ElectricShock_Status::StaticClass();
}