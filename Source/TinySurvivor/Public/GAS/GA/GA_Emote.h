#pragma once

#include "CoreMinimal.h"
#include "BaseAbility/TSGameplayAbilityBase.h"
#include "GA_Emote.generated.h"

UCLASS()
class TINYSURVIVOR_API UGA_Emote : public UTSGameplayAbilityBase
{
	GENERATED_BODY()
public:
	UGA_Emote();
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	TObjectPtr<UAnimMontage> HelloMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	TObjectPtr<UAnimMontage> Dance1Montage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	TObjectPtr<UAnimMontage> Dance2Montage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	TObjectPtr<UAnimMontage> Dance3Montage;
	
	UPROPERTY()
	TObjectPtr<UAudioComponent> ActiveEmoteSound;
	
	UPROPERTY(EditAnywhere, Category = "Sound")
	TObjectPtr<USoundBase> EmoteSound;
	UPROPERTY(EditAnywhere, Category = "Sound")
	TObjectPtr<USoundBase> Dance1Sound;
	UPROPERTY(EditAnywhere, Category = "Sound")
	TObjectPtr<USoundBase> Dance2Sound;
	UPROPERTY(EditAnywhere, Category = "Sound")
	TObjectPtr<USoundBase> Dance3Sound;
	
	UFUNCTION()
	void OnMontageEnded();
};
