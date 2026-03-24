// All CopyRight From YulRyongGameStudio //


#include "A_FOR_INGAME/SECTION_WORLD/ResourceSpawn/Actor/ResourceBucket/TSResourceBucketActor.h"

#include "A_FOR_INGAME/SECTION_WORLD/ResourceSpawn/Actor/ResourceNode/TSResourceNodeActor.h"
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
	
	BucketData.ResourceNodePtrArray.Reserve(10);
}

void ATSResourceBucketActor::BeginPlay()
{
	Super::BeginPlay();
	
	// 서버에서 실행 
	if (!HasAuthority()) return;
	
	// 자식 액터 (노드들) 찾기
	TArray<UChildActorComponent*> ChildActorComps;
	this->GetComponents<UChildActorComponent>(ChildActorComps);
	for (UChildActorComponent* Comp : ChildActorComps)
	{
		if (!IsValid(Comp)) continue;
		ATSResourceNodeActor* ChildNode = CastChecked<ATSResourceNodeActor>(Comp->GetChildActor());
		if (IsValid(ChildNode)) continue;;
		
		// 자식에게 버킷을 부모 포인터로 받으라고 지시 및 값 설정 
		ChildNode->GetNodeData().ThisDataOwnerBucketPtr = this;
		ChildNode->GetNodeData().StaticData.bHasParentBucket = true;
		
		// 자식 포인터 캐싱
		BucketData.ResourceNodePtrArray.AddUnique(ChildNode);
	}
	
	// 마지막 위치 기억 및 죽음 유무
	BucketData.DynamicData.BucketLastTransform = this->GetActorTransform();
	BucketData.DynamicData.IsBucketIsDead = false;
	
	// 시스템에 버킷 등록
	UTSResourceNodeAndBucketGetHelperSystem* System = UTSResourceNodeAndBucketGetHelperSystem::Get(this);
	if (!IsValid(System)) return;
	System->RequestRegisterBucket(this);
	
}

#pragma endregion
//======================================================================================================================	
