// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "TSResourceNodeBucketDynamicData.h"
#include "TSResourceNodeBucketStaticData.h"
#include "TSResourceNodeBucketRuntimeData.generated.h"


class ATSResourceNodeActor;
class ATSResourceBucketActor;

USTRUCT(BlueprintType)
struct TINYSURVIVOR_API FTSResourceNodeBucketRuntimeData
{
	GENERATED_BODY()
	
	// 자기 자신
	UPROPERTY()
	TObjectPtr<ATSResourceBucketActor> ThisDataOwnerBucketPtr = nullptr;
	
	// 버킷 내부 노드들(포인터)
	UPROPERTY()
	TArray<TObjectPtr<ATSResourceNodeActor>> ResourceNodePtrArray;
	
	UPROPERTY(SaveGame)
	FTSResourceNodeBucketDynamicData DynamicData;
	
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite, Category="TS | Resoruce")
	FTSResourceNodeBucketStaticData StaticData;
	
};
