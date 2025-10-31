#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
//#include "AbilitySystemComponent.h"
#include "TSAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class TINYSURVIVOR_API UTSAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	//UTSAttributeSet();
	//virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override; //복제 설정
	//virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override; //GE 적용 후

	//이런식으로이름이랑값넣으면 됨
	//UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health , Category = "Health")
	//FGameplayAttributeData Health;
	//ATTRIBUTE_ACCESSORS(UTSAttributeSet, Health);

protected:
	//OnRep 클라 반영
	//UFUNCTION()
	//void OnRep_Health(const FGameplayAttributeData& Health);
};
