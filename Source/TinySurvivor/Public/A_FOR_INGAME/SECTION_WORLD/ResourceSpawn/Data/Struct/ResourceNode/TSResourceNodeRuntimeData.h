// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "TSResourceNodeStaticData.h"
#include "TSResourceNodeDynamicData.h"
#include "TSResourceNodeRuntimeData.generated.h"


class ATSResourceNodeActor;

USTRUCT(BlueprintType)
struct TINYSURVIVOR_API FTSResourceNodeRunTimeData
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<ATSResourceNodeActor> ThisDataOwnerNodePtr = nullptr;
	
	UPROPERTY(SaveGame)
	FTSResourceNodeDynamicData DynamicData;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="TS | Resoruce")
	FTSResourceNodeStaticData StaticData;
	
};
