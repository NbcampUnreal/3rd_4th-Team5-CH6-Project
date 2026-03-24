// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "A_FOR_INGAME/SECTION_WORLD/ResourceSpawn/Data/Struct/ResourceNodeBucket/TSResourceNodeBucketRuntimeData.h"
#include "GameFramework/Actor.h"
#include "TSResourceBucketActor.generated.h"

UCLASS()
class TINYSURVIVOR_API ATSResourceBucketActor : public AActor
{
	GENERATED_BODY()

//======================================================================================================================	
#pragma region 라이프_사이클
	
	//━━━━━━━━━━━━━━━━━━━━
	// 라이프 사이클
	//━━━━━━━━━━━━━━━━━━━━
public:
	ATSResourceBucketActor();
	virtual void BeginPlay() override;
	
#pragma endregion
//======================================================================================================================	
#pragma region 데이터
	
	//━━━━━━━━━━━━━━━━━━━━
	// 데이터
	//━━━━━━━━━━━━━━━━━━━━

public:
	
	FORCEINLINE FTSResourceNodeBucketRuntimeData& GetBucketData() { return BucketData;};
	FORCEINLINE void SetBucketData(FTSResourceNodeBucketRuntimeData& InBucketData) { BucketData = InBucketData;};
	
protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TS | ResourceSpawn")
	FTSResourceNodeBucketRuntimeData BucketData;
	
#pragma endregion
//======================================================================================================================		
};
