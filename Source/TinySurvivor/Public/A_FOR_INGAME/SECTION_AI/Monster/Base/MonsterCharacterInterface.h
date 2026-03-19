// All CopyRight From YulRyongGameStudio //


#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "A_FOR_INGAME/SECTION_AI/Monster/TSMonsterTable.h"
#include "MonsterCharacterInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UMonsterCharacterInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class TINYSURVIVOR_API IMonsterCharacterInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void SetSpeedIncrease() = 0;
	virtual void ResetSpeed() = 0;
	virtual void StopWalk() = 0;
	virtual void RegainSpeed() = 0;
	virtual void SetDropRootItems(FTSMonsterTable& MonsterTable) = 0;
	virtual void RequestSpawnDropRooItems() = 0;
	virtual void MakeTimeToDead() = 0;
};
