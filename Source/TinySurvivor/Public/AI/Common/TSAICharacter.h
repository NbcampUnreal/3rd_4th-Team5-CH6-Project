// TSAICharacter.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "AI/Common/MonsterAttributeSet.h"
#include "Components/StateTreeComponent.h"
#include "TSAICharacter.generated.h"

class UMonsterAttributeSet;

UCLASS(Abstract)
class TINYSURVIVOR_API ATSAICharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	ATSAICharacter();
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	// 사망 처리
	UFUNCTION(BlueprintCallable, Category = "AI")
	virtual void OnDeath(AActor* Killer);
	virtual void OnDamaged(float DamageAmount, const FGameplayTagContainer& DamageTags, AActor* Attacker);
	
	// 풀에서 꺼낼 때 초기화하는 함수
	void ResetMonster();
	
protected:
	
	UPROPERTY(EditDefaultsOnly, Category = "Combat|Hit")
	UAnimMontage* HitReactMontageFront;
	
	UPROPERTY(EditDefaultsOnly, Category = "Combat|Hit")
	UAnimMontage* HitReactMontageBack;
	
	UPROPERTY(EditDefaultsOnly, Category = "Combat|Hit")
	UAnimMontage* HitReactMontageLeft;
	
	UPROPERTY(EditDefaultsOnly, Category = "Combat|Hit")
	UAnimMontage* HitReactMontageRight;
	
	UPROPERTY(EditDefaultsOnly, Category = "Combat|Death")
	UAnimMontage* DeathMontage;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UAbilitySystemComponent> ASC;
	
	UPROPERTY()
	TObjectPtr<UMonsterAttributeSet> AttributeSet;
	
private:
	// 방향에 따른 적절한 몽타주 반환 함수
	UAnimMontage* GetHitMontageByDirection(AActor* Attacker);
};