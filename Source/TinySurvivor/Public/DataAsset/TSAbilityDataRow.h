#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "TSAbilityDataRow.generated.h"

class UGameplayAbility;

USTRUCT(BlueprintType)
struct FTSAbilityDataRow : public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability")
	FText DisplayName; //어빌리티 이름
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability")
	FText Description; //어빌리티 무슨 능력
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability")
	TSubclassOf<UGameplayAbility> AbilityClass; //어빌리티 클래스
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability")
	FGameplayTagContainer AbilityTag; // 태그표시용
};
