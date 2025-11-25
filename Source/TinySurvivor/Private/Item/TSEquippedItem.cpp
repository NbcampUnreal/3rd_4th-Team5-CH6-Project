// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/TSEquippedItem.h"

#include "Net/UnrealNetwork.h"

// Sets default values
ATSEquippedItem::ATSEquippedItem()
{
	bReplicates = true;
	bAlwaysRelevant = true;
	SetReplicateMovement(true);

	// 기본 메시(머리, 몸통)
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	SetRootComponent(MeshComp);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	MeshComp->SetIsReplicated(true);

	// 왼쪽 다리
	LeftLegMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftLegMeshComp"));
	LeftLegMeshComp->SetupAttachment(MeshComp);
	LeftLegMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LeftLegMeshComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	LeftLegMeshComp->SetIsReplicated(true);

	// 오른쪽 다리
	RightLegMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightLegMeshComp"));
	RightLegMeshComp->SetupAttachment(MeshComp);
	RightLegMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightLegMeshComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	RightLegMeshComp->SetIsReplicated(true);
}

void ATSEquippedItem::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ATSEquippedItem, MeshComp);
	DOREPLIFETIME(ATSEquippedItem, LeftLegMeshComp);
	DOREPLIFETIME(ATSEquippedItem, RightLegMeshComp);
}

void ATSEquippedItem::SetMesh(UStaticMesh* Mesh)
{
	if (MeshComp && Mesh)
	{
		MeshComp->SetStaticMesh(Mesh);
	}
}

void ATSEquippedItem::SetLegMesh(UStaticMesh* Mesh)
{
	if (Mesh && LeftLegMeshComp && RightLegMeshComp)
	{
		LeftLegMeshComp->SetStaticMesh(Mesh);
		RightLegMeshComp->SetStaticMesh(Mesh);
		if (MeshComp)
		{
			MeshComp->SetVisibility(false);
		}
	}
}
