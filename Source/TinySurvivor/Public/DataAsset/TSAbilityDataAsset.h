#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "TSAbilityDataAsset.generated.h"

class UDataTable;

UCLASS()
class TINYSURVIVOR_API UTSAbilityDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere,Category="Ability")
	TMap<FName,UDataTable*> AbilityTables;
};
 