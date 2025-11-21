#include "AI/Gaint/GA/TSGiantAbilityMaster.h"
#include "AI/Gaint/TSGiantCompInterface.h"
#include "Components/StateTreeAIComponent.h"
#include "Kismet/KismetSystemLibrary.h"

UTSGiantAbilityMaster::UTSGiantAbilityMaster()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

void UTSGiantAbilityMaster::OnMontageCompleted()
{
}

void UTSGiantAbilityMaster::OnMontageCancelled()
{
}

void UTSGiantAbilityMaster::OnMontageInterrupted()
{
}

void UTSGiantAbilityMaster::OnMontageBlendOut()
{
}

UStateTreeAIComponent* UTSGiantAbilityMaster::GiantStateTreeComponent()
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!IsValid(AvatarActor)) return nullptr;

	if (!UKismetSystemLibrary::DoesImplementInterface(AvatarActor, UTSGiantCompInterface::StaticClass())) return nullptr;
	return ITSGiantCompInterface::Execute_GetStateTreeAIComponent(AvatarActor);
}

void UTSGiantAbilityMaster::SendFinishMontageEventToStateTree(const FGameplayTag& InSendTag)
{
	if (!IsValid(GiantStateTreeComponent())) return;
	
	if (MontageEndTag == FGameplayTag::EmptyTag)
	{
		GiantStateTreeComponent()->SendStateTreeEvent(InSendTag);
	}
	else
	{
		GiantStateTreeComponent()->SendStateTreeEvent(MontageEndTag);
	}
}
