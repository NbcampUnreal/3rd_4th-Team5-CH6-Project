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

	// 서버에서만 실행 
	if (!HasAuthority()) return;

	// NOTE : 부모를 여기서 등록하지 않음.
	// 부모 버킷이 알아서 다 등록하도록 설계함.
	// 자식은 아무것도 하지 않음.
	
	// 자식 등록 
	UTSResourceNodeAndBucketGetHelperSystem* System = UTSResourceNodeAndBucketGetHelperSystem::Get(this);
	if (!IsValid(System)) return;
	System->RequestRegisterNode(this);
}

#pragma endregion
//======================================================================================================================