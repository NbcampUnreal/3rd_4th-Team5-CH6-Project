// All CopyRight From YulRyongGameStudio //


#include "A_FOR_INGAME/SECTION_ITEM/Item/Actor/SkeletalMeshItem/TSSkeletalMeshItem.h"

#include "Components/WidgetComponent.h"


//======================================================================================================================	
#pragma region 라이프_사이클
	
	
	//━━━━━━━━━━━━━━━━━━━━
	// 라이프 사이클
	//━━━━━━━━━━━━━━━━━━━━	

ATSSkeletalMeshItem::ATSSkeletalMeshItem()
{
	PrimaryActorTick.bCanEverTick = false;
	
	ItemSkeletalMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ItemSkeletalMeshComp"));
	SetRootComponent(ItemSkeletalMeshComp);
	
	InteractWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractWidgetComp"));
	InteractWidgetComp->SetupAttachment(GetRootComponent());
}

void ATSSkeletalMeshItem::BeginPlay()
{
	Super::BeginPlay();
	
	// 서버에서 피직스 실행
	if (HasAuthority()) ItemSkeletalMeshComp->SetSimulatePhysics(true);
	
}

#pragma endregion
//======================================================================================================================