// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "A_FOR_INGAME/SECTION_WORLD/Resource/Data/Struct/TSResourceStaticData.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "TSResourceSpawnSystem.generated.h"

class ATSResourceActorBase;
class ATSResourceNodeActor;
/**
 * 
 */
UCLASS()
class TINYSURVIVOR_API UTSResourceSpawnSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
//======================================================================================================================	
#pragma region 게터
	
	
	//━━━━━━━━━━━━━━━━━━━━
	// 게터
	//━━━━━━━━━━━━━━━━━━━━	
public:

	static UTSResourceSpawnSystem* Get(const UObject* InWorldContextObject);

#pragma endregion
//======================================================================================================================
#pragma region 스폰_API
	
	//━━━━━━━━━━━━━━━━━━━━
	// 스폰 API
	//━━━━━━━━━━━━━━━━━━━━	
	
public:
	// 새로운 자원 스폰 
	bool SpawnNewResource(int32 InResourceID, FVector InSpawnLocation, FRotator InSpawnRotator, ATSResourceNodeActor* InSpawnNode);
	
protected:
	bool SpawnNewResource_Internal(FTSResourceStaticData& InResourceStaticData, TSubclassOf<ATSResourceActorBase> ResourceClass, FVector& InSpawnLocation,  FRotator& InSpawnRotator, ATSResourceNodeActor* InSpawnNode);
	
#pragma endregion
//======================================================================================================================	

};
