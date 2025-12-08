#pragma once

#include "CoreMinimal.h"
#include "BaseAbility/TSGameplayAbilityBase.h"
#include "Item/Data/Common/ItemCommonEnums.h"
#include "GA_LeftClick.generated.h"

class UGameplayEffect;

UCLASS()
class TINYSURVIVOR_API UGA_LeftClick : public UTSGameplayAbilityBase
{
	GENERATED_BODY()
public: 
	UGA_LeftClick();
	
	//---------------------스태미나 10 이상이어야 공격 가능---------------------
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	//---------------------활성화 (GE 적용) + 몽타주 재생---------------------
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack")
	float AttackRange = 100.0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack")
	FVector BoxHalfSize = FVector(30.f, 30.f, 60.f);
	
	// -----------------------몽타주-----------------------
	//기본 공격 (맨손, 일반 아이템)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	TObjectPtr<UAnimMontage> NoneMontage;

	//도구
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	TObjectPtr<UAnimMontage> PickMontage; // 곡괭이 타입 도구

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	TObjectPtr<UAnimMontage> AxeMontage; // 도끼 타입 도구

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	TObjectPtr<UAnimMontage> HammerMontage; // 망치 타입 도구
	
	//무기
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	TObjectPtr<UAnimMontage> WeaponMeleeMontage; // 근거리 전투용 무기
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	TObjectPtr<UAnimMontage> WeaponSpearMontage; // 창 타입 전투용 무기 ------ 보류
	
	// -----------------------GE-----------------------
	// Stamina 관련 GE 들
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack")
	TSubclassOf<UGameplayEffect> AttackCostEffectClass; // 바로 -20 스태미나
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack")
	TSubclassOf<UGameplayEffect> StaminaDelayEffectClass; //EndAbility 후 1초 딜레이
	
	// 적 Health 관련 GE
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack")
	TSubclassOf<UGameplayEffect> EnemyDamageEffectClass; // 적에게 데미지 입히는 GE 
	
	// 공격 상태 관련 태그 
	UPROPERTY(EditDefaultsOnly, Category = "Attack|Tags")
	FGameplayTag AttackTag; // 몽타주 재생동안 state.Status.attack 태그 활성화

	UPROPERTY(EditDefaultsOnly, Category = "Attack|Tags")
	FGameplayTag StaminaBlockTag; // 몽타주 재생동안 stamina 회복 차단
	
	// 몽타주 종료시 처리 
	UFUNCTION()
	void OnAttackMontageFinished();
	
	// -----------------------노티파이 받는 함수-----------------------
	UFUNCTION()
	void ReceivedNotify(FGameplayEventData EventData);

private:
	void BoxTrace(UAbilitySystemComponent* ASC, EItemAnimType ItemAnimType, int32& ATK);
};
