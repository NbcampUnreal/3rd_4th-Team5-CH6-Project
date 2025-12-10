#include "AI/Monster/Base/TSAICBase.h"
#include "AbilitySystemComponent.h"
#include "Components/StateTreeAIComponent.h"
#include "Perception/AIPerceptionComponent.h"

ATSAICBase::ATSAICBase()
{
	PrimaryActorTick.bCanEverTick = false;
	ThisAIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>("ThisAIPerceptionComponent");
	ThisStateTreeAIComponent = CreateDefaultSubobject<UStateTreeAIComponent>("ThisStateTreeAIComponent");
}

UAbilitySystemComponent* ATSAICBase::GetAbilitySystemComponent() const
{
	if (IsValid(GetPawn())) return nullptr;
	
	IAbilitySystemInterface* PawnASCInterface = Cast<IAbilitySystemInterface>(GetPawn());
	if (!PawnASCInterface) return nullptr;
	return PawnASCInterface->GetAbilitySystemComponent();
}

void ATSAICBase::StartLogic()
{
	ThisStateTreeAIComponent->StartLogic();
}
