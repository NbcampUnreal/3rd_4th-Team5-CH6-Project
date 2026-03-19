// All CopyRight From YulRyongGameStudio //


#include "A_FOR_INGAME/SECTION_PLAYER/PlayerState/TSPlayerState.h"
#include "AbilitySystemComponent.h"
#include "A_FOR_INGAME/SECTION_GAS/AttributeSet/TSAttributeSet.h"

ATSPlayerState::ATSPlayerState()
{
	bReplicates = true;
	
	ASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));
	ASC -> SetIsReplicated(true);
	ASC -> SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	Attributes = CreateDefaultSubobject<UTSAttributeSet>(TEXT("Attributes"));
}

void ATSPlayerState::BeginPlay()
{
	Super::BeginPlay();
	SetNetUpdateFrequency(100.f);
}

UAbilitySystemComponent* ATSPlayerState::GetAbilitySystemComponent() const
{
	return ASC;
}
UTSAttributeSet* ATSPlayerState::GetAttributeSet() const
{
	return Attributes;
}