// All CopyRight From YulRyongGameStudio //


#include "A_FOR_INGAME/Sound/Footstep/AN_Footstep.h"
#include "A_FOR_INGAME/Sound/Footstep/FootstepComponent.h"

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
		FootstepComp->PlayFootstepSound(SocketLocation);
	}
}
