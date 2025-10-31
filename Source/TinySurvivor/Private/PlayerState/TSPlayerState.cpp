#include "PlayerState/TSPlayerState.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet/TSAttributeSet.h"

ATSPlayerState::ATSPlayerState()
{
	bReplicates = true;
	
	ASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));
	ASC -> SetIsReplicated(true);
	ASC -> SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	Attributes = CreateDefaultSubobject<UTSAttributeSet>(TEXT("Attributes"));
}
UAbilitySystemComponent* ATSPlayerState::GetAbilitySystemComponent() const
{
	return ASC;
}
UTSAttributeSet* ATSPlayerState::GetAttributeSet() const
{
	return Attributes;
}

//비긴 플레이에 HasAuthority () GiveAbilities 함수해줘야하나?

