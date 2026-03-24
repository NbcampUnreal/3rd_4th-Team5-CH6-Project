// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "TSGiveGAGEDataAsset.generated.h"

class UGameplayAbility;
class UGameplayEffect;
/**
 * 
 */
UCLASS()
class TINYSURVIVOR_API UTSGiveGAGEDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TS | GAGE")
	TArray<TSubclassOf<UGameplayAbility>> BaseGameplayAbilities; 

	// 반드시 숙지.
	// 단순 gameplayeffect 라고 해도 반드시 어빌리티를 통해 부여할 것.
	// 추후 단순 gameplayeffect 에 VFX 나 다른 로직을 넣으려고 할 때 해당 어빌리티에 추가하면 되므로 확장이 용이함.
	
};
