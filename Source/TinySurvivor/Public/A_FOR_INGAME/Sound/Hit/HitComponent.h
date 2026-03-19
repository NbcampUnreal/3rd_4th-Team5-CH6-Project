// All CopyRight From YulRyongGameStudio //


#pragma once

#include "CoreMinimal.h"
#include "A_FOR_INGAME/Sound/SoundComponent.h"
#include "HitComponent.generated.h"


enum class EItemAnimType : uint8;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TINYSURVIVOR_API UHitComponent : public USoundComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UHitComponent();
	
	UPROPERTY(EditDefaultsOnly, Category = "HitSound")
	TMap<TEnumAsByte<EPhysicalSurface>, USoundBase*> HitSounds;
    
	UPROPERTY(EditDefaultsOnly, Category = "HitSound")
	USoundBase* DefaultHitSound;

	UFUNCTION(BlueprintCallable, Category = "HitSound")
	void PlayHitSound(const FHitResult& HitResult);
};
