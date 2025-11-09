// Copyright 2023 X-Games. All Rights Reserved.


#include "PoolableActorBase.h"

#include "ActorPool.h"


APoolableActorBase::APoolableActorBase()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	
	bInitializeTickEnabled = false;
	bInitializeHiddenInGame = true;
	bInitializeEnableCollision = false;
	InitializeNetDormancy = ENetDormancy::DORM_DormantAll;

	bAcquireTickEnabled = true;
	bAcquireHiddenInGame = false;
	bAcquireEnableCollision = true;
	AcquireNetDormancy = ENetDormancy::DORM_Awake;

	bReleaseTickEnabled = false;
	bReleaseHiddenInGame = true;
	bReleaseEnableCollision = false;
	ReleaseNetDormancy = ENetDormancy::DORM_DormantAll;
}

bool APoolableActorBase::IsActive()
{
	return bIsActive;
}

AActorPool* APoolableActorBase::GetPoolOwner() const
{
	if ( this->PoolOwner.IsValid())
	{
		return this->PoolOwner.Get();
	}
	return nullptr;
}

void APoolableActorBase::OnInitialize_Implementation()
{
	bIsActive = false;
	SetActorTickEnabled(bInitializeTickEnabled);
	SetActorHiddenInGame(bInitializeHiddenInGame);
	SetActorEnableCollision(bInitializeEnableCollision);
	SetNetDormancy(InitializeNetDormancy);
	ForceNetUpdate();
}

void APoolableActorBase::OnAcquire_Implementation(const int32& IntParam, const FString& StringParam, const UObject* ObjectParam)
{
	bIsActive = true;
	SetActorTickEnabled(bAcquireTickEnabled);
	SetActorHiddenInGame(bAcquireHiddenInGame);
	SetActorEnableCollision(bAcquireEnableCollision);
	SetNetDormancy(AcquireNetDormancy);
	ForceNetUpdate();
}

void APoolableActorBase::OnRelease_Implementation()
{
	bIsActive = false;
	SetActorTickEnabled(bReleaseTickEnabled);
	SetActorHiddenInGame(bReleaseHiddenInGame);
	SetActorEnableCollision(bReleaseEnableCollision);
	SetNetDormancy(ReleaseNetDormancy);
	ForceNetUpdate();
}

void APoolableActorBase::SetPoolOwner(AActorPool* InPoolOwner)
{
	// this->PoolOwner = MakeShareable<AActorPool>(InPoolOwner);
	this->PoolOwner = InPoolOwner;
}
