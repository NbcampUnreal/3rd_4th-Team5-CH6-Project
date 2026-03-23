// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "A_FOR_INGAME/SECTION_ITEM/Item/Data/Struct/TSItemRuntimeData.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "TSITemSpawnSubSystem.generated.h"

class ATSItemActorBase;
/**
 * 
 */
UCLASS()
class TINYSURVIVOR_API UTSITemSpawnSubSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
//======================================================================================================================	
#pragma region 게터
	
	
	//━━━━━━━━━━━━━━━━━━━━
	// 게터
	//━━━━━━━━━━━━━━━━━━━━	
public:

	static UTSITemSpawnSubSystem* Get(const UObject* InWorldContextObject);
	
#pragma endregion
//======================================================================================================================	
#pragma region 외부_API
	
	//━━━━━━━━━━━━━━━━━━━━
	// 외부 API
	//━━━━━━━━━━━━━━━━━━━━
	
public:
	ATSItemActorBase* SpawnNewItem(int32 InSpawnItemID, int32 InWantStackSize, FVector InSpawnLocation);	
	ATSItemActorBase* DropItemFromSomeWhere(FTSItemRuntimeData& InItemRuntimeData, FVector InDropLocation);
	
#pragma endregion
//======================================================================================================================		
};
