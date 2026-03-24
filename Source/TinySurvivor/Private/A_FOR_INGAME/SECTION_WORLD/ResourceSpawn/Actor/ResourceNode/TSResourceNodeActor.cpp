// All CopyRight From YulRyongGameStudio //


#include "A_FOR_INGAME/SECTION_WORLD/ResourceSpawn/Actor/ResourceNode/TSResourceNodeActor.h"
#include "A_FOR_INGAME/SECTION_WORLD/ResourceSpawn/System/TSResourceNodeAndBucketGetHelperSystem.h"


//======================================================================================================================
#pragma region 라이프_사이클
	
	//━━━━━━━━━━━━━━━━━━━━
	// 라이프 사이클
	//━━━━━━━━━━━━━━━━━━━━

ATSResourceNodeActor::ATSResourceNodeActor()
{
	PrimaryActorTick.bCanEverTick = true;
	NodeData.ThisDataOwnerNodePtr = this;
	
	bReplicates = false;
}

void ATSResourceNodeActor::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority()) return;
	UTSResourceNodeAndBucketGetHelperSystem* System = UTSResourceNodeAndBucketGetHelperSystem::Get(this);
	if (!IsValid(System)) return;
	
	System->RequestRegisterNode(this);
}

#pragma endregion
//======================================================================================================================