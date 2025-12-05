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
	virtual void OnDeath(AActor* Killer);
	
	// 풀에서 꺼낼 때 초기화하는 함수
	void ResetMonster();
	
protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UAbilitySystemComponent> ASC;
	
	UPROPERTY()
	TObjectPtr<UMonsterAttributeSet> AttributeSet;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	TObjectPtr<UStateTreeComponent> StateTreeComponent;
};