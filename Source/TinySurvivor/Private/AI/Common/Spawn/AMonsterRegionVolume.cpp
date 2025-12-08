// MonsterRegionVolume.cpp

#include "AI/Common/Spawn/AMonsterRegionVolume.h"

AAMonsterRegionVolume::AAMonsterRegionVolume()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = false;
	
	RegionVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("RegionVolume"));
	RootComponent = RegionVolume;
	
	// 물리 충돌은 끄고, Overlap 감지만 켜둠
	RegionVolume->SetSimulatePhysics(false);
	RegionVolume->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	
	// 에디터에서 잘 보이게 초록색 설정
	RegionVolume->ShapeColor = FColor::Green;
	RegionVolume->SetLineThickness(5.0f);
}