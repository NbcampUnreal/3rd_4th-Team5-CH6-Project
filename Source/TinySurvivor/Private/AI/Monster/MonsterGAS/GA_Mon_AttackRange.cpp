// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/Monster/MonsterGAS/GA_Mon_AttackRange.h"
#include "Abilities/Async/AbilityAsync_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "AI/Monster/Projectile/Mon_ProjectileBase.h"
#include "GameFramework/Character.h"
#include "Tasks/GameplayTask_SpawnActor.h"

UGA_Mon_AttackRange::UGA_Mon_AttackRange()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ClientOrServer;
}

void UGA_Mon_AttackRange::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                          const FGameplayAbilityActorInfo* ActorInfo, 
                                          const FGameplayAbilityActivationInfo ActivationInfo,
                                          const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	if (!K2_CommitAbility() || !IsValid(AbilityMontage))
	{
		K2_EndAbility();
		return;
	}
	
	ProjectileSpawnLocation = GetAvatarActorFromActorInfo()->GetActorLocation();
	ProjectileSpawnRotation = GetAvatarActorFromActorInfo()->GetActorRotation();
	
	
	UAbilityTask_PlayMontageAndWait* Task =
	UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,                   // OwningAbility
		NAME_None,              // TaskInstanceName
		AbilityMontage,      // 재생할 몽타주
		1.0f,                   // 재생 속도
		NAME_None,              // StartSectionName
		true,                   // StopWhenAbilityEnds
		1.0f                    // RootMotionTranslationScale
	);
	
	if (!IsValid(Task))
	{
		K2_EndAbility();
		return;
	}
	
	Task->OnCompleted.AddDynamic(this, &ThisClass::OnMontageCompleted);
	Task->OnCancelled.AddDynamic(this, &ThisClass::OnMontageCancelled);
	Task->OnInterrupted.AddDynamic(this, &ThisClass::OnMontageInterrupted);
	Task->OnBlendOut.AddDynamic(this, &ThisClass::OnMontageBlendOut);
	
	UAbilityTask_WaitGameplayEvent* CancelInteractTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this,ReceiveShootProjectileTag);
	CancelInteractTask->EventReceived.AddDynamic(this, &ThisClass::OnReceived);
	
	CancelInteractTask->ReadyForActivation();
	Task->ReadyForActivation();
}

void UGA_Mon_AttackRange::OnReceived(FGameplayEventData Payload)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("OnReceived"));
	
	// 플레이어 검증
	if (!IsValid(Payload.Target)) return;
	ACharacter* PlayerChar = Cast<ACharacter>(Payload.Target);
	if (!PlayerChar) return;
	APlayerController* ThePlayerController = Cast<APlayerController>(PlayerChar->GetController()); 
	if (!IsValid(ThePlayerController)) return;
	
	// 발사체 생성
	ProjectileSpawnLocation += FVector(0.f, 0.f, 200.f);
	FTransform SpawnTransform(ProjectileSpawnRotation, ProjectileSpawnLocation);
	
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AActor* SpawnedProjectile = GetWorld()->SpawnActor(ProjectileClass, &SpawnTransform, SpawnParams);
	if (!IsValid(SpawnedProjectile)) return;
	
	// 발사체 위치 설정
	FTransform EndPointTransform = PlayerChar->GetActorTransform();
	FVector TargetLocation = EndPointTransform.GetLocation();
	
	AMon_ProjectileBase* Projectile = Cast<AMon_ProjectileBase>(SpawnedProjectile);
	if (!IsValid(Projectile)) return;
	
	// 발사체 초기화
	Projectile->SetTargetLocation(TargetLocation);
	Projectile->TryActivateProjectile();
	
}

void UGA_Mon_AttackRange::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}


void UGA_Mon_AttackRange::OnMontageCompleted()
{
	SendFinishMontageEventToStateTree(MontageEndTag);
	K2_EndAbility();
}

void UGA_Mon_AttackRange::OnMontageCancelled()
{
	SendFinishMontageEventToStateTree(MontageEndTag);
	K2_EndAbility();
}

void UGA_Mon_AttackRange::OnMontageInterrupted()
{
	SendFinishMontageEventToStateTree(MontageEndTag);
	K2_EndAbility();
}

void UGA_Mon_AttackRange::OnMontageBlendOut()
{
	SendFinishMontageEventToStateTree(MontageEndTag);
	K2_EndAbility();
}
