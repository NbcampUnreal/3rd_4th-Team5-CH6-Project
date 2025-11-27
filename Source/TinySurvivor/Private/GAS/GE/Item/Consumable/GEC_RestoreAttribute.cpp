// GEC_RestoreAttribute.cpp
#include "GAS/GE/Item/Consumable/GEC_RestoreAttribute.h"

#include "AbilitySystemComponent.h"
#include "GAS/AttributeSet/TSAttributeSet.h"
#include "GameplayTags/ItemGameplayTags.h"
#include "Item/System/ItemDataSubsystem.h"
#include "Item/Data/ItemData.h"

// 로그 카테고리 정의
DEFINE_LOG_CATEGORY_STATIC(LogRestoreExecution, Log, All);

// Attribute Capture 정의
struct FRestoreAttributeStatics
{
	/*
		Capture할 Attribute 정의 (Target = 효과 받는 캐릭터)
		Restore에 필요한 Attribute(Health, Hunger, Thirst...)를 정의.
		ExecutionCalculation에서 속성 값을 계산할 때 Attribute Capture가 필요.
		DEFINE_ATTRIBUTE_CAPTUREDEF을 사용하여 Target(효과를 받는 캐릭터)의 속성을 실시간 읽도록 설정.
	*/
	DECLARE_ATTRIBUTE_CAPTUREDEF(Health);
	DECLARE_ATTRIBUTE_CAPTUREDEF(MaxHealth);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Hunger);
	DECLARE_ATTRIBUTE_CAPTUREDEF(MaxHunger);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Thirst);
	DECLARE_ATTRIBUTE_CAPTUREDEF(MaxThirst);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Sanity);
	DECLARE_ATTRIBUTE_CAPTUREDEF(MaxSanity);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Temperature);
	DECLARE_ATTRIBUTE_CAPTUREDEF(MaxTemperature);

	FRestoreAttributeStatics()
	{
		/*
			Attribute Capture 설정
			실행 시점의 Target 예:Health를 Capture
			DEFINE_ATTRIBUTE_CAPTUREDEF(클래스, 속성, Source/Target, Snapshot)
		*/
		DEFINE_ATTRIBUTE_CAPTUREDEF(UTSAttributeSet, Health, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UTSAttributeSet, MaxHealth, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UTSAttributeSet, Hunger, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UTSAttributeSet, MaxHunger, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UTSAttributeSet, Thirst, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UTSAttributeSet, MaxThirst, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UTSAttributeSet, Sanity, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UTSAttributeSet, MaxSanity, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UTSAttributeSet, Temperature, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UTSAttributeSet, MaxTemperature, Target, false);
	}
};

static const FRestoreAttributeStatics& GetRestoreAttributeStatics()
{
	static FRestoreAttributeStatics Statics;
	return Statics;
}

#pragma region Constructor
UGEC_RestoreAttribute::UGEC_RestoreAttribute()
{
	/*
		Capture할 Attribute 등록
		ExecutionCalculation이 실행될 때,
		GAS가 “이 Attribute들 계산할거니까 Capture해놓아라” 하고 알려주는 단계.
	*/
	const FRestoreAttributeStatics& Statics = GetRestoreAttributeStatics();

	RelevantAttributesToCapture.Add(Statics.HealthDef);
	RelevantAttributesToCapture.Add(Statics.MaxHealthDef);
	RelevantAttributesToCapture.Add(Statics.HungerDef);
	RelevantAttributesToCapture.Add(Statics.MaxHungerDef);
	RelevantAttributesToCapture.Add(Statics.ThirstDef);
	RelevantAttributesToCapture.Add(Statics.MaxThirstDef);
	RelevantAttributesToCapture.Add(Statics.SanityDef);
	RelevantAttributesToCapture.Add(Statics.MaxSanityDef);
	RelevantAttributesToCapture.Add(Statics.TemperatureDef);
	RelevantAttributesToCapture.Add(Statics.MaxTemperatureDef);
}
#pragma endregion

#pragma region Execute_Implementation
void UGEC_RestoreAttribute::Execute_Implementation(
	const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	//=======================================================================
	// 1. 기본 정보 가져오기
	//=======================================================================
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	
	// Target ASC (효과 받는 캐릭터)
	UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();
	if (!TargetASC)
	{
		UE_LOG(LogRestoreExecution, Error, TEXT("잘못된 TargetASC가 감지되었습니다."));
		return;
	}
	
	//=======================================================================
	// 2. SetByCaller로 ItemID 가져오기
	//=======================================================================
	FGameplayTag ItemIDTag = ItemTags::TAG_Data_ItemID;
	float ItemIDFloat = Spec.GetSetByCallerMagnitude(ItemIDTag, false, 0.0f);
	int32 ItemID = static_cast<int32>(ItemIDFloat);
	
	if (ItemID <= 0)
	{
		UE_LOG(LogRestoreExecution, Error, TEXT("GE Spec에 유효하지 않은 ItemID가 전달되었습니다. (ItemID: %d)"), ItemID);
		return;
	}
	
	//=======================================================================
	// 3. ItemDataSubsystem에서 아이템 정보 조회
	//=======================================================================
	UItemDataSubsystem* ItemDataSystem = UItemDataSubsystem::GetItemDataSubsystem(
		TargetASC->GetOwner()
	);
	
	if (!ItemDataSystem)
	{
		UE_LOG(LogRestoreExecution, Error, TEXT("ItemDataSubsystem을 찾을 수 없습니다!"));
		return;
	}
	
	FItemData ItemInfo;
	if (!ItemDataSystem->GetItemDataSafe(ItemID, ItemInfo))
	{
		UE_LOG(LogRestoreExecution, Error, TEXT("해당 ItemID의 아이템 데이터를 찾을 수 없습니다. (ItemID: %d)"), ItemID);
		return;
	}
	
	//=======================================================================
	// 4. EffectTag와 EffectValue 추출
	//=======================================================================
	FGameplayTag EffectTag = ItemInfo.EffectTag;
	float EffectValue = ItemInfo.EffectValue;
	
	if (!EffectTag.IsValid())
	{
		UE_LOG(LogRestoreExecution, Error, TEXT("유효하지 않은 EffectTag입니다. (ItemID: %d)"), ItemID);
		return;
	}
	
	if (EffectValue == 0.0f)
	{
		UE_LOG(LogRestoreExecution, Warning, TEXT("EffectValue 값이 0입니다. (ItemID: %d)"), ItemID);
		return;
	}
	
	// // 특수 아이템 처리: POISON_HEAL
	// if (EffectTag.MatchesTagExact(ItemTags::TAG_Item_Effect_POISON_HEAL))
	// {
	// 	if (TargetASC)
	// 	{
	// 		// 기존 독 상태 제거
	// 		FGameplayTagContainer PoisonTags;
	// 		PoisonTags.AddTag(FGameplayTag::RequestGameplayTag("State.Status.Poison"));
	// 		TargetASC->RemoveActiveEffectsWithTags(PoisonTags);
	// 		
	// 		UE_LOG(LogRestoreExecution, Log, TEXT("독 상태 효과를 제거했습니다. (ItemID=%d)"), ItemID);
	// 	}
	// 	
	// 	return; // Attribute 변경 없음
	// }
	
	//=======================================================================
	// 5. EffectTag에 따라 Attribute 결정
	//=======================================================================
	FGameplayAttribute TargetAttribute;
	if (!GetAttributeFromEffectTag(EffectTag, TargetAttribute))
	{
		UE_LOG(LogRestoreExecution, Error,
			TEXT("EffectTag를 속성(Attribute)으로 매핑하는 데 실패했습니다: %s (ItemID: %d)"),
			*EffectTag.ToString(), ItemID);
		return;
	}
	
	//=======================================================================
	// 6. 현재 Attribute 값 가져오기 (옵션)
	//=======================================================================
	FAggregatorEvaluateParameters EvaluateParams;
	EvaluateParams.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvaluateParams.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	
	float CurrentValue = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
		FGameplayEffectAttributeCaptureDefinition(TargetAttribute, EGameplayEffectAttributeCaptureSource::Target, false),
		EvaluateParams,
		CurrentValue
	);
	
	//=======================================================================
	// 7. 최종 변경량 계산
	//=======================================================================
	float FinalDelta = EffectValue;
	
	// 감소 효과는 음수로 변환
	if (IsNegativeEffect(EffectTag))
	{
		FinalDelta = -FMath::Abs(FinalDelta);
	}
	
	UE_LOG(LogRestoreExecution, Log, 
		TEXT("Applying effect: ItemID=%d, Tag=%s, Attribute=%s, CurrentValue=%.1f, Delta=%.1f"), 
		ItemID,
		*EffectTag.ToString(),
		*TargetAttribute.GetName(),
		CurrentValue,
		FinalDelta);
	
	//=======================================================================
	// 8. Attribute 변경 적용
	//=======================================================================
	OutExecutionOutput.AddOutputModifier(
		FGameplayModifierEvaluatedData(
			TargetAttribute,
			EGameplayModOp::Additive,
			FinalDelta
		)
	);
	
	UE_LOG(LogRestoreExecution, Log,
		TEXT("아이템 속성 변경을 성공적으로 적용했습니다. (ItemID: %d)"), ItemID);

}
#pragma endregion

#pragma region GetAttributeFromEffectTag
bool UGEC_RestoreAttribute::GetAttributeFromEffectTag(
	const FGameplayTag& EffectTag,
	FGameplayAttribute& OutAttribute) const
{
	// EffectTag → Attribute 매핑
	if (EffectTag.MatchesTagExact(ItemTags::TAG_Item_Effect_HUNGER_RESTORE))
	{
		OutAttribute = UTSAttributeSet::GetHungerAttribute();
		return true;
	}
	else if (EffectTag.MatchesTagExact(ItemTags::TAG_Item_Effect_THIRST_RESTORE))
	{
		OutAttribute = UTSAttributeSet::GetThirstAttribute();
		return true;
	}
	else if (EffectTag.MatchesTagExact(ItemTags::TAG_Item_Effect_HP_HEAL))
	{
		OutAttribute = UTSAttributeSet::GetHealthAttribute();
		return true;
	}
	else if (EffectTag.MatchesTagExact(ItemTags::TAG_Item_Effect_SANITY_RESTORE))
	{
		OutAttribute = UTSAttributeSet::GetSanityAttribute();
		return true;
	}
	else if (EffectTag.MatchesTagExact(ItemTags::TAG_Item_Effect_TEMP_ADJUST))
	{
		OutAttribute = UTSAttributeSet::GetTemperatureAttribute();
		return true;
	}
	else if (EffectTag.MatchesTagExact(ItemTags::TAG_Item_Effect_HP_DAMAGE))
	{
		OutAttribute = UTSAttributeSet::GetHealthAttribute();
		return true;
	}
	else if (EffectTag.MatchesTagExact(ItemTags::TAG_Item_Effect_POISON_HEAL))
	{
		// 독 해제는 별도 GE에서 처리
		UE_LOG(LogRestoreExecution, Warning, TEXT("POISON_HEAL은 별도의 디버프 제거 GE가 필요합니다."));
		return false;
	}
	else if (EffectTag.MatchesTagExact(ItemTags::TAG_Item_Effect_CURE_DEBUFF))
	{
		// 디버프 해제는 별도 GE에서 처리
		UE_LOG(LogRestoreExecution, Warning, TEXT("CURE_DEBUFF는 별도의 디버프 제거 GE가 필요합니다."));
		return false;
	}
	
	UE_LOG(LogRestoreExecution, Error, TEXT("알 수 없는 EffectTag입니다: %s"), *EffectTag.ToString());
	return false;
}
#pragma endregion

#pragma region IsNegativeEffect
bool UGEC_RestoreAttribute::IsNegativeEffect(const FGameplayTag& EffectTag) const
{
	// 감소 효과 태그 목록
	return EffectTag.MatchesTagExact(ItemTags::TAG_Item_Effect_HP_DAMAGE);
}
#pragma endregion