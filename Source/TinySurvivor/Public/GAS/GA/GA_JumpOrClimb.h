#pragma once

#include "CoreMinimal.h"
#include "BaseAbility/TSGameplayAbilityBase.h"
#include "GA_JumpOrClimb.generated.h"

class UGameplayEffect;
class UAnimMontage;

UCLASS()
class TINYSURVIVOR_API UGA_JumpOrClimb : public UTSGameplayAbilityBase
{
	GENERATED_BODY()
public:
	UGA_JumpOrClimb();
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Climb")
	TSubclassOf<UGameplayEffect> JumpStateTagEffectClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Climb")
	TSubclassOf<UGameplayEffect> ClimbStateTagEffectClass; 
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Climb")
	TSubclassOf<UGameplayEffect> ClimbCostEffectClass; 
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Climb")
	TSubclassOf<UGameplayEffect> StaminaDelayEffectClass; 
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Climb")
	float TraceDistance = 120.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Climb") 
	float TraceRadius = 30.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Climb")
	FName ClimbableTagName = FName("Climbable");
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Climb")
	float EyeHeight = 50.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Climb")
	UAnimMontage* MantleMontage;
	
private:
	FTimerHandle ClimbableCheckTimerHandle;
	FTimerHandle MantleFinishTimerHandle;
	
	FActiveGameplayEffectHandle CostHandle;
	FActiveGameplayEffectHandle ClimbStateTagHandle;
	
	//Delegate Handle
	FDelegateHandle StaminaDelegateHandle;
	
	bool ClimbableActor(FHitResult& OutHit);
	void StartJump();
	void StartClimb(const FHitResult& TargetHit);
	void CheckClimbingState();
	void OnAttributeChanged(const FOnAttributeChangeData& Data); //스태미나 변하는거 감지하는 함수
};
