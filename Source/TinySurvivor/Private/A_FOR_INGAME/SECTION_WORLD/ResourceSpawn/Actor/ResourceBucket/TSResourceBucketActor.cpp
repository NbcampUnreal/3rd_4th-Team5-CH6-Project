// All CopyRight From YulRyongGameStudio //


#include "A_FOR_INGAME/SECTION_WORLD/ResourceSpawn/Actor/ResourceBucket/TSResourceBucketActor.h"
#include "A_FOR_INGAME/SECTION_WORLD/ResourceSpawn/System/TSResourceNodeAndBucketGetHelperSystem.h"

//======================================================================================================================	
#pragma region 라이프_사이클
	
	//━━━━━━━━━━━━━━━━━━━━
	// 라이프 사이클
	//━━━━━━━━━━━━━━━━━━━━

ATSResourceBucketActor::ATSResourceBucketActor()
{
	PrimaryActorTick.bCanEverTick = false;
	BucketData.ThisDataOwnerBucketPtr = this;
	
	bReplicates = true;
	SetReplicatingMovement(true);
}

void ATSResourceBucketActor::BeginPlay()
{
	Super::BeginPlay();
	
	if (!HasAuthority()) return;
	UTSResourceNodeAndBucketGetHelperSystem* System = UTSResourceNodeAndBucketGetHelperSystem::Get(this);
	if (!IsValid(System)) return;
	
	System->RequestRegisterBucket(this);
}

#pragma endregion
//======================================================================================================================	
