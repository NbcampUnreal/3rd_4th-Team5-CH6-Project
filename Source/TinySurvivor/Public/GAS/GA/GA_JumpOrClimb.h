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
	TSubclassOf<UGameplayEffect> ClimbCostEffectClass; // 0.125초당 스태미나 -1
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Climb")
	TSubclassOf<UGameplayEffect> StaminaDelayEffectClass; // EndAbility 후 1초 딜레이  
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Climb")
	float TraceDistance = 50.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Climb") 
	float TraceRadius = 30.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Climb")
	FName ClimbableTagName = FName("Climbable");
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Climb")
	float EyeHeight = 50.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Climb")
	UAnimMontage* MantleMontage; // 마지막에 난간 올라가는 몽타주
	
private:
	FTimerHandle ClimbableCheckTimerHandle;
	FTimerHandle MantleFinishTimerHandle; // 마지막 난간 끝날 때 호출용 타이머
	
	FActiveGameplayEffectHandle CostHandle;
	
	//Delegate Handle
	FDelegateHandle StaminaDelegateHandle;
	
	bool ClimbableActor(FHitResult& OutHit); // 클라이밍 가능한 액터가 있는지
	void StartJump();
	void StartClimb(const FHitResult& TargetHit);
	void CheckClimbingState();
	void OnAttributeChanged(const FOnAttributeChangeData& Data); //스태미나 변하는거 감지하는 함수
};
