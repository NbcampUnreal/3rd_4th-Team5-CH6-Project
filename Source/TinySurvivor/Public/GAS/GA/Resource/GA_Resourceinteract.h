#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Character/TSCharacter.h"
#include "GA_Resourceinteract.generated.h"

class ITSResourceItemInterface;
/**
 * 
 */
UCLASS()
class TINYSURVIVOR_API UGA_Resourceinteract : public UGameplayAbility
{
	GENERATED_BODY()
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma region AbilityLifecycle
	//------------------------------
	// UGA_Resourceinteract 라이프 사이클
	//------------------------------
	
public:
	UGA_Resourceinteract();
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

#pragma endregion
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma region ResourceTargeting
	//------------------------------
	// UGA_Resourceinteract 자원 타겟팅
	//------------------------------
	
protected:
	void BoxTrace();
	
	UFUNCTION()
	void DoInteract();
	
	UFUNCTION()
	void CheckTimeRemaining(float Progress);
	
	void CheckDistance();
	
	UFUNCTION()
	void OnCancelInteract(FGameplayEventData Payload);
	
	ITSResourceItemInterface* TargetResource = nullptr;
	FVector TargetHitLocation;
	FVector TargetHitNormal;

	float TargetHaverestTimeRemaining; 
	
	TWeakObjectPtr<ATSCharacter> ThePlayer;
	TWeakObjectPtr<UAbilitySystemComponent> CachedASC;
	
	// 디버깅
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Erosion | Debug")
	bool bShowDebug = false;
#pragma endregion	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma region AbilityOption
	//------------------------------
	// UGA_Resourceinteract 어빌리티 옵션
	//------------------------------
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CanInteractRange")
	FGameplayTag ReceiveEndTag;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CanInteractRange")
	float CanInteractRange = 200.0f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BoxHalfSize")
	FVector BoxHalfSize = FVector(30.f, 30.f, 60.f);

#pragma endregion
};
	