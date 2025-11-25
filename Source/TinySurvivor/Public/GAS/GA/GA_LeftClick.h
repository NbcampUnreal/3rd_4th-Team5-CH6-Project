#pragma once

#include "CoreMinimal.h"
#include "BaseAbility/TSGameplayAbilityBase.h"
#include "GA_LeftClick.generated.h"

UCLASS()
class TINYSURVIVOR_API UGA_LeftClick : public UTSGameplayAbilityBase
{
	GENERATED_BODY()
public: 
	UGA_LeftClick();
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack")
	float AttackRange = 100.0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack")
	FVector BoxHalfSize = FVector(30.f, 30.f, 60.f);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	TObjectPtr<UAnimMontage> NoneMontage; // 주먹

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	TObjectPtr<UAnimMontage> PickMontage; // 곡괭이?

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	TObjectPtr<UAnimMontage> WeaponSpearMontage; //창?
private:
	void BoxTrace();
};
