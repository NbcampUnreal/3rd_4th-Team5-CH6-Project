#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "TSGameInstance.generated.h"

class UTSAbilityDataAsset;

UCLASS()
class TINYSURVIVOR_API UTSGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ability")
	TObjectPtr<UTSAbilityDataAsset> AbilityAsset;
};
