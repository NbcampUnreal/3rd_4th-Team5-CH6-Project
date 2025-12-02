// TSAICharacter.cpp

#include "AI/Common/TSAICharacter.h"
#include "Net/UnrealNetwork.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Item/LootComponent.h"
#include "AbilitySystemComponent.h"

ATSAICharacter::ATSAICharacter()
{
	bReplicates = true;
	
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	
	LootComponent = CreateDefaultSubobject<ULootComponent>("LootComponent");
}

UAbilitySystemComponent* ATSAICharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ATSAICharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (HasAuthority())
	{
		CurrentHealth = MaxHealth;	
	}
}

void ATSAICharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ATSAICharacter, CurrentState);
}

void ATSAICharacter::OnRep_ChaserState()
{
	if (CurrentState == EChaserState::Chase)
	{
		GetCharacterMovement()->MaxWalkSpeed = 600.0f;	// 뜀
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = 300.0f;	// 걷기
	}
}

void ATSAICharacter::PerformAttack()
{
	if (!HasAuthority())
		return;
	
	// 데미지 판정 로직
	
	// 몽타주 재생 명령 (모든 클라에게 전송)
	Multicast_PlayerAttackMontage();
}

void ATSAICharacter::Multicast_PlayerAttackMontage_Implementation()
{
	if (AttackMontage)
	{
		PlayAnimMontage(AttackMontage);
	}
}

float ATSAICharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	class AController* EventInstigator, AActor* DamageCauser)
{
	if (!HasAuthority() || CurrentState == EChaserState::Dead)
		return 0.0f;
	
	CurrentHealth -= DamageAmount;
	if (CurrentHealth <= 0.0f)
	{
		Die();
	}
	
	return DamageAmount;
}

void ATSAICharacter::Die()
{
	CurrentState = EChaserState::Dead;
	
	if (LootComponent)
	{
		LootComponent->SpawnLoot();
	}
	
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
	
	SetLifeSpan(5.0f);
}