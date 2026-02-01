#include "System/ResourceControl/TSResourcePoint.h"
#include "System/ResourceControl/TSResourceControlSubSystem.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	

	//------------------------------
	// ATSResourcePoint 라이프 사이클 
	//------------------------------

ATSResourcePoint::ATSResourcePoint()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ATSResourcePoint::BeginPlay()
{
	Super::BeginPlay();
	
	UTSResourceControlSubSystem* ResourceControlSubSystem = UTSResourceControlSubSystem::GetUTSResourceControlSubSystem(this);
	if (!IsValid(ResourceControlSubSystem))
	{
		if (bShowDebug) UE_LOG(ResourceControlSystem, Error, TEXT("자원 원천 관리 시스템을 찾지 못함."));
		return;
	}
	
	ResourceControlSubSystem->AssignReSourcePointToSector(this);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//------------------------------
	// ATSResourcePoint 자원 원천 포인트
	//------------------------------

void ATSResourcePoint::SetAllocatedResource(const TWeakObjectPtr<ATSResourceBaseActor> NewResource)
{
	AllocatedResource = NewResource; 
	bIsAllocated = true;
}
