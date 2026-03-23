// All CopyRight From YulRyongGameStudio //


#include "A_FOR_INGAME/SECTION_ITEM/Item/Actor/StaticMeshItem/TSStaticMeshItem.h"
#include "Components/WidgetComponent.h"


//======================================================================================================================	
#pragma region 라이프_사이클
	
	
	//━━━━━━━━━━━━━━━━━━━━
	// 라이프 사이클
	//━━━━━━━━━━━━━━━━━━━━	

ATSStaticMeshItem::ATSStaticMeshItem()
{
	PrimaryActorTick.bCanEverTick = false;
	
	ItemStaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemStaticMeshComp"));
	SetRootComponent(ItemStaticMeshComp);	
	
	InteractWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractWidgetComp"));
	InteractWidgetComp->SetupAttachment(GetRootComponent(), FName("InteractUISocket"));
}

void ATSStaticMeshItem::BeginPlay()
{
	Super::BeginPlay();

	// 서버에서 피직스 실행
	if (HasAuthority()) ItemStaticMeshComp->SetSimulatePhysics(true);
	
}

#pragma endregion
//======================================================================================================================