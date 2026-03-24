// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "Data/Struct/TSSaveMasterData.h"
#include "GameFramework/SaveGame.h"
#include "TSSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class TINYSURVIVOR_API UTSSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite, Category = "TS | Save")
	bool IsThisMasterDataIsDontHavAnySavedData = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TS | SaveGame")
	FTSSaveMasterData MasterData;
};
