// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/TSEquippedItem.h"

#include "Net/UnrealNetwork.h"

// Sets default values
ATSEquippedItem::ATSEquippedItem()
{
	bReplicates = true;
	bAlwaysRelevant = true;
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	SetRootComponent(MeshComp);
	
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	MeshComp->SetIsReplicated(true);
}

void ATSEquippedItem::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ATSEquippedItem, MeshComp);
}

void ATSEquippedItem::SetMesh(UStaticMesh* Mesh)
{
	if (MeshComp&&Mesh)
	{
		MeshComp->SetStaticMesh(Mesh);
	}
}
