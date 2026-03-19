// All CopyRight From YulRyongGameStudio //


#include "A_FOR_INGAME/SECTION_ITEM/Building/Actor/TSCampfire.h"
#include "NiagaraComponent.h"
#include "A_FOR_INGAME/SECTION_ITEM/Item/Data/BuildingData.h"


// Sets default values
ATSCampfire::ATSCampfire()
{
	bSetLightScaleByErosion = false;
	FireEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("FireParticle"));
	FireEffect->SetupAttachment(RootComponent);
}

void ATSCampfire::InitializeFromBuildingData(const FBuildingData& BuildingInfo, const int32 StaticDataID)
{
	Super::InitializeFromBuildingData(BuildingInfo, StaticDataID);
	if (HasAuthority())
	{
		LifeTime = BuildingInfo.MaxMaintenance;
	}
}

// Called when the game starts or when spawned
void ATSCampfire::BeginPlay()
{
	Super::BeginPlay();
	// LifeTime 지나면 Destory
	GetWorldTimerManager().SetTimer(LifeTimeTimerHandle, this, &ATSCampfire::OnLifeTimeExpired, LifeTime, false);
}

void ATSCampfire::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (LifeTimeTimerHandle.IsValid())
	{
		GetWorldTimerManager().ClearTimer(LifeTimeTimerHandle);
	}
	Super::EndPlay(EndPlayReason);
}

void ATSCampfire::OnLifeTimeExpired()
{
	if (FireEffect)
	{
		FireEffect->Deactivate();
	}
	Multicast_PlayDestroyEffect();
	SetLifeSpan(1.f);
}
