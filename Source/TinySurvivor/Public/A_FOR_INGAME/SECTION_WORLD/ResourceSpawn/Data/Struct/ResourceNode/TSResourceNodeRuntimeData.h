// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "TSResourceNodeStaticData.h"
#include "TSResourceNodeDynamicData.h"
#include "TSResourceNodeRuntimeData.generated.h"


class ATSResourceActorBase;
class ATSResourceBucketActor;
class ATSResourceNodeActor;

USTRUCT(BlueprintType)
struct TINYSURVIVOR_API FTSResourceNodeRunTimeData
{
	GENERATED_BODY()

	// 현재 소환된 자원
	UPROPERTY()
	TObjectPtr<ATSResourceActorBase> CurrentSpawnedResourcePtr = nullptr;
	
	// 자기 자신
	UPROPERTY() 
	TObjectPtr<ATSResourceNodeActor> ThisDataOwnerNodePtr = nullptr;
	
	// 부모 버킷
	UPROPERTY() 
	TObjectPtr<ATSResourceBucketActor> ThisDataOwnerBucketPtr = nullptr;
	
	UPROPERTY(SaveGame)
	FTSResourceNodeDynamicData DynamicData;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="TS | Resoruce")
	FTSResourceNodeStaticData StaticData;
	
};
