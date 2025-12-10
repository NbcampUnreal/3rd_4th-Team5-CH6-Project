#include "AI/Monster/Base/TSMonsterBaseCharacter.h"
#include "AI/Monster/MonsterGAS/TSMonsterAS.h"
#include "AI/Monster/MonsterGAS/TSMonsterASC.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
	
	//---------------------------------------
	// UTSMonsterSpawnSystem 라이프 사이클
	//--------------------------------------
	
ATSMonsterBaseCharacter::ATSMonsterBaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	
	MonsterAS = CreateDefaultSubobject<UTSMonsterAS>("MonsterAS");
	
	MonsterASC = CreateDefaultSubobject<UTSMonsterASC>("MonsterASC");
	MonsterASC->SetIsReplicated(true);
	MonsterASC->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 360.0f, 0.0f);
	GetCharacterMovement()->bOrientRotationToMovement = true;
	
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Block);
	GetMesh()->SetCollisionResponseToAllChannels(ECR_Ignore);
	
	bReplicates = true;
	SetReplicatingMovement(true);
	
	SetNetUpdateFrequency(60.f);
	SetMinNetUpdateFrequency(10.f);
	SetNetCullDistanceSquared(1000000.0f);
}

void ATSMonsterBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (!IsValid(MonsterASC)) return;
	MonsterASC->InitAbilityActorInfo(this,this);
	
	if (HasAuthority())
	{
		for (auto& GiveGA : GiveAbilities)
		{
			MonsterASC->K2_GiveAbility(GiveGA);
		}
		
		for (auto& GiveGE : GiveGameplayEffects)
		{
			FGameplayEffectContextHandle GameplayEffectContext;
			GameplayEffectContext.AddSourceObject(this);
			MonsterASC->BP_ApplyGameplayEffectToSelf(GiveGE, 1.0f, GameplayEffectContext);
		}
	}
}

UAbilitySystemComponent* ATSMonsterBaseCharacter::GetAbilitySystemComponent() const
{
	return MonsterASC;
}
