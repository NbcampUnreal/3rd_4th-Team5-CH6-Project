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
	
	//---------------------활성화 (GE 적용) + 몽타주 재생---------------------
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

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

	//무기
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	TObjectPtr<UAnimMontage> WeaponMeleeMontage; // 근거리 전투용 무기
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	TObjectPtr<UAnimMontage> WeaponSpearMontage; // 창 타입 전투용 무기 ------ 보류
	
	// -----------------------GE-----------------------
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack")
	TSubclassOf<UGameplayEffect> EnemyDamageEffectClass; // 적에게 데미지 입히는 GE 
	
	// -----------------------노티파이 받는 함수-----------------------
	UFUNCTION()
	void ReceivedNotify(FGameplayEventData EventData);
private:
	void BoxTrace(UAbilitySystemComponent* ASC, EItemAnimType ItemAnimType, int32& ATK);
};
