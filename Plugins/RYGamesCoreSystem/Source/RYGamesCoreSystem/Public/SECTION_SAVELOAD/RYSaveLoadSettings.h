// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "GameFramework/SaveGame.h"
#include "RYSaveLoadSettings.generated.h"

/**
 * 
 */
UCLASS(config = Game, DefaultConfig, meta = ( DisplayName = "RY Save Load Settings" ))
class RYGAMESCORESYSTEM_API URYSaveLoadSettings : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RY | Core | SaveLoad")
	TSubclassOf<USaveGame> SaveGameClass = nullptr;
};
