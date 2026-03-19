// All CopyRight From YulRyongGameStudio //


#include "A_FOR_INGAME/Sound/Footstep/AN_Climbing.h"
#include "A_FOR_INGAME/Sound/Footstep/FootstepComponent.h"

void UAN_Climbing::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
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
		FootstepComp->PlayClimbingSound(SocketLocation);
	}
}
