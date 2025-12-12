// Fill out your copyright notice in the Description page of Project Settings.


#include "Footstep/AN_Footstep.h"

#include "Footstep/FootstepComponent.h"

void UAN_Footstep::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	if (!MeshComp || !MeshComp->GetOwner())
	{
		return;
	}
	UFootstepComponent* FootstepComp = MeshComp->GetOwner()->FindComponentByClass<UFootstepComponent>();
	if (FootstepComp)
	{
		FVector SocketLocation = MeshComp->GetSocketLocation(SocketName);
		FootstepComp->PlayFootstepSound(SocketLocation, IsLeft);
	}
}
