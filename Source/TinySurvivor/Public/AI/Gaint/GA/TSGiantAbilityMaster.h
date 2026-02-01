#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "TSGiantAbilityMaster.generated.h"

class UStateTreeAIComponent;
/**
 * 
 */
UCLASS(Abstract, NotBlueprintable, NotBlueprintType)
class TINYSURVIVOR_API UTSGiantAbilityMaster : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UTSGiantAbilityMaster();

protected:
	// 재생할 몽타주
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Ability|Montage")
	TObjectPtr<UAnimMontage> AbilityMontage;

	// 델리게이트로 받을 함수들
	UFUNCTION()
	virtual void OnMontageCompleted();

	UFUNCTION()
	virtual void OnMontageCancelled();

	UFUNCTION()
	virtual void OnMontageInterrupted();

	UFUNCTION()
	virtual void OnMontageBlendOut();
	
	// 몽타주 재생 이후 호출 보낼 이벤트 태그
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability|Montage")
	FGameplayTag MontageEndTag = FGameplayTag::EmptyTag;
	
	// 스테이트 트리
	UFUNCTION(BlueprintCallable)
	UStateTreeAIComponent* GiantStateTreeComponent();
	
	UFUNCTION(BlueprintCallable)
	void SendFinishMontageEventToStateTree(const FGameplayTag& InSendTag);
};
