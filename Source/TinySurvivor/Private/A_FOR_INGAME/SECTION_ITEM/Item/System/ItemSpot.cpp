// All CopyRight From YulRyongGameStudio //


#include "A_FOR_INGAME/SECTION_ITEM/Item/System/ItemSpot.h"
#include "A_FOR_INGAME/SECTION_ITEM/Item/Runtime/ItemInstance.h"
#include "A_FOR_INGAME/SECTION_ITEM/Item/System/WorldItemInstanceSubsystem.h"

AItemSpot::AItemSpot()
{
	PrimaryActorTick.bCanEverTick = false; // 틱 필요 없음

	bReplicates = false;
	
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;
	// 충돌 꺼둠 (배치용)
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AItemSpot::BeginPlay()
{
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	
	// 서버에서만 실행 (클라이언트는 리플리케이션으로 인스턴스 정보를 받음)
	if (HasAuthority())
	{
		UWorld* World = GetWorld();
		if (World)
		{
			auto* InstanceSys = World->GetSubsystem<UWorldItemInstanceSubsystem>();
			if (InstanceSys)
			{
				// 데이터 구성
				FSlotStructMaster NewItemData;
				NewItemData.ItemData.StaticDataID = ItemID;

				// 서브시스템에 인스턴스 등록 요청 (HISM에 추가됨)
				InstanceSys->AddInstance(NewItemData, GetActorTransform());
			}
		}
	}

	SetLifeSpan(0.01f);
	
	Super::BeginPlay();
}