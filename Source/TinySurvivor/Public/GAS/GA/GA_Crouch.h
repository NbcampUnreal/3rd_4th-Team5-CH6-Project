#pragma once

#include "CoreMinimal.h"
#include "BaseAbility/TSGameplayAbilityBase.h"
#include "GA_Crouch.generated.h"

class UGameplayEffect;

UCLASS()
class TINYSURVIVOR_API UGA_Crouch : public UTSGameplayAbilityBase
{
	GENERATED_BODY()
	
public:
	UGA_Crouch();
	
	// 조건 체크 (roll 중이면 crouch 불가)
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	
	// c 키 눌렀을때 호출 ( IsFalling (공중)-> 착지 후 crouch, 서있는 상태 -> 바로 crouch )
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	//종료 (일어나기 / 달리기 연계)
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Crouch")
	TSubclassOf<UGameplayEffect> CrouchSpeedEffectClass; // 스피드 * 0.75
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Crouch")
	TSubclassOf<UGameplayAbility> SprintAbilityClass; // 일어설때 shift 눌려있으면 달리기 실행 = BP_GA_Crouch
private:
	FActiveGameplayEffectHandle SpeedHandle;
	
	UFUNCTION()
	void OnLanded(EMovementMode NewMovementMode); //공중에서 사용했을때, 발이 지면에 닿으면 호출
};