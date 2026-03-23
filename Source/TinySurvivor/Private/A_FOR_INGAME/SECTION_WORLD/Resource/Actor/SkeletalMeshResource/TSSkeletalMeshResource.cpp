// All CopyRight From YulRyongGameStudio //


#include "A_FOR_INGAME/SECTION_WORLD/Resource/Actor/SkeletalMeshResource/TSSkeletalMeshResource.h"


//======================================================================================================================	
#pragma region 라이프_사이클
	
	
	//━━━━━━━━━━━━━━━━━━━━
	// 라이프 사이클
	//━━━━━━━━━━━━━━━━━━━━	
ATSSkeletalMeshResource::ATSSkeletalMeshResource()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ATSSkeletalMeshResource::BeginPlay()
{
	Super::BeginPlay();
	
	// 서버에서 피직스 실행
	if (HasAuthority()) ResourceSkeletalMeshComp->SetSimulatePhysics(true);
}

#pragma endregion
//======================================================================================================================