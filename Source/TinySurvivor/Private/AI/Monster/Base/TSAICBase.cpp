#include "AI/Monster/Base/TSAICBase.h"
#include "AbilitySystemComponent.h"
#include "AI/Monster/MonsterTag.h"
#include "Components/StateTreeAIComponent.h"
#include "GameFramework/Character.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"

ATSAICBase::ATSAICBase()
{
	PrimaryActorTick.bCanEverTick = false;
	
	ThisAIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>("ThisAIPerceptionComponent");
	
	ThisStateTreeAIComponent = CreateDefaultSubobject<UStateTreeAIComponent>("ThisStateTreeAIComponent");
	
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->SightRadius = 2000.f;
	SightConfig->LoseSightRadius = 2500.f;
	SightConfig->PeripheralVisionAngleDegrees = 90.f;
	SightConfig->SetMaxAge(10.f);

	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

	ThisAIPerceptionComponent->ConfigureSense(*SightConfig);
	ThisAIPerceptionComponent->SetDominantSense(UAISense_Sight::StaticClass());
}

void ATSAICBase::BeginPlay()
{
	Super::BeginPlay();
	
	if (IsValid(ThisAIPerceptionComponent))
	{
		ThisAIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &ATSAICBase::OnTargetPerceptionUpdated);
	}
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

void ATSAICBase::SetInstigator(AActor* InInstigator)
{
	if (!IsValid(InInstigator)) return;
	FirstInstigator = InInstigator;
}

AActor* ATSAICBase::GetCauseInstigator()
{
	if (!FirstInstigator.IsValid()) return nullptr;
	return FirstInstigator.Get();
}

void ATSAICBase::OnTargetPerceptionUpdated(AActor* SensedActor, FAIStimulus Stimulus)
{
	if (!IsValid(SensedActor)) return;
	if (SensedActor == this) return;
	ACharacter* SensedCharacter = Cast<ACharacter>(SensedActor);
	if (!IsValid(SensedCharacter)) return;
	if (!SensedCharacter->IsPlayerControlled()) return;

	if (Stimulus.WasSuccessfullySensed())
	{
		ThisStateTreeAIComponent->SendStateTreeEvent(TSMoonsterTag::TP_MONSTER_NEED_TO_FIND_TARGET);
	}
}
