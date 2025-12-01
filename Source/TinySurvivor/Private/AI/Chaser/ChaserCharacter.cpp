// ChaserCharacter.cpp

#include "AI/Chaser/ChaserCharacter.h"
#include "Net/UnrealNetwork.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Item/LootComponent.h"

AChaserCharacter::AChaserCharacter()
{
	bReplicates = true;
	LootComponent = CreateDefaultSubobject<ULootComponent>("LootComponent");
}

void AChaserCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (HasAuthority())
	{
		CurrentHealth = MaxHealth;	
	}
}

void AChaserCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AChaserCharacter, CurrentState);
}

void AChaserCharacter::OnRep_ChaserState()
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

void AChaserCharacter::PerformAttack()
{
	if (!HasAuthority())
		return;
	
	// 데미지 판정 로직
	
	// 몽타주 재생 명령 (모든 클라에게 전송)
	Multicast_PlayerAttackMontage();
}

void AChaserCharacter::Multicast_PlayerAttackMontage_Implementation()
{
	if (AttackMontage)
	{
		PlayAnimMontage(AttackMontage);
	}
}

float AChaserCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
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

void AChaserCharacter::Die()
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