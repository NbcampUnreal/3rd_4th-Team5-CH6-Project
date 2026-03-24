// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "TSResourceNodeBucketDynamicData.h"
#include "TSResourceNodeBucketStaticData.h"
#include "TSResourceNodeBucketRuntimeData.generated.h"


class ATSResourceBucketActor;

USTRUCT(BlueprintType)
struct TINYSURVIVOR_API FTSResourceNodeBucketRuntimeData
{
	GENERATED_BODY()
	
	UPROPERTY()
	TObjectPtr<ATSResourceBucketActor> ThisDataOwnerBucketPtr = nullptr;
	
	UPROPERTY(SaveGame)
	FTSResourceNodeBucketDynamicData DynamicData;
	
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite, Category="TS | Resoruce")
	FTSResourceNodeBucketStaticData StaticData;
	
};
