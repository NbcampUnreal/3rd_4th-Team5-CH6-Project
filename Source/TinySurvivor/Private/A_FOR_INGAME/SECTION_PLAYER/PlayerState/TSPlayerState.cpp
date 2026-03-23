// All CopyRight From YulRyongGameStudio //


#include "A_FOR_INGAME/SECTION_PLAYER/PlayerState/TSPlayerState.h"
#include "A_FOR_INGAME/SECTION_GAS/Comp/TSAttributeSet.h"
#include "AbilitySystemComponent.h"

//======================================================================================================================	
#pragma region 라이프_사이클
	
	//━━━━━━━━━━━━━━━━━━━━
	// 라이프 사이클
	//━━━━━━━━━━━━━━━━━━━━	

ATSPlayerState::ATSPlayerState()
{
	PrimaryActorTick.bCanEverTick = false;
	
	bReplicates = true;
	
	SetNetUpdateFrequency(100.f);
	SetMinNetUpdateFrequency(100.f);

	AttributeSet = CreateDefaultSubobject<UTSAttributeSet>(TEXT("AttributeSet"));
	
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	
}

#pragma endregion
//======================================================================================================================	
#pragma region GAS
	//━━━━━━━━━━━━━━━━━━━━
	// GAS
	//━━━━━━━━━━━━━━━━━━━━

UAbilitySystemComponent* ATSPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

#pragma endregion
//======================================================================================================================	
