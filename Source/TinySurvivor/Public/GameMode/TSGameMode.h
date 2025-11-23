#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "TSGameMode.generated.h"


UCLASS()
class TINYSURVIVOR_API ATSGameMode : public AGameMode
{
	GENERATED_BODY()
public:
	ATSGameMode();
	
	void PostLogin(APlayerController* NewPlayer) override;
};
