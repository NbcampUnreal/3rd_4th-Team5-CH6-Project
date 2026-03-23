// All CopyRight From YulRyongGameStudio //


#include "A_FOR_INGAME/SECTION_WORLD/Resource/Actor/StaticMeshResource/TSStaticMeshResource.h"
#include "Components/WidgetComponent.h"

//======================================================================================================================	
#pragma region 라이프_사이클
	
	
	//━━━━━━━━━━━━━━━━━━━━
	// 라이프 사이클
	//━━━━━━━━━━━━━━━━━━━━	

ATSStaticMeshResource::ATSStaticMeshResource()
{
	PrimaryActorTick.bCanEverTick = false;
	
	ResourceStaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ResourceStaticMeshComp"));
	SetRootComponent(ResourceStaticMeshComp);	
	
	InteractWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractWidgetComp"));
	InteractWidgetComp->SetupAttachment(GetRootComponent(), FName("InteractUISocket"));
	
}

void ATSStaticMeshResource::BeginPlay()
{
	Super::BeginPlay();

	// 서버에서 피직스 실행
	if (HasAuthority()) ResourceStaticMeshComp->SetSimulatePhysics(true);
}

#pragma endregion
//======================================================================================================================