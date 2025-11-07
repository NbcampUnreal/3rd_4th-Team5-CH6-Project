#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TSPlayerController.generated.h"

class ATSCharacter;

UCLASS()
class TINYSURVIVOR_API ATSPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void InitializePlayerHUD(ATSCharacter* InCharacter);
};
