#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "TSMonsterASC.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TINYSURVIVOR_API UTSMonsterASC : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	UTSMonsterASC();
};
