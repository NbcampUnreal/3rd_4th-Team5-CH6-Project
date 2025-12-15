#include "GAS/GA/Resource/GA_Resourceinteract.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "System/ResourceControl/TSResourceItemInterface.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Character/TSCharacter.h"
#include "GAS/GA/Resource/GAT_TimeRemaining.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma region AbilityLifecycle
	//------------------------------
	// UGA_Resourceinteract 라이프 사이클
	//------------------------------

UGA_Resourceinteract::UGA_Resourceinteract()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
	if (bShowDebug) UE_LOG(LogTemp, Warning, TEXT("자원 채취 시작"));
}

void UGA_Resourceinteract::ActivateAbility(const FGameplayAbilitySpecHandle Handle,const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	if (!K2_CommitAbility())
	{
		if (bShowDebug) UE_LOG(LogTemp, Warning, TEXT("자원 채취 소모 비용 감당 실패"));
		K2_EndAbility();
	}
	else
	{
		if (bShowDebug) UE_LOG(LogTemp, Warning, TEXT("자원 채취 소모 비용 감당 성공"));
	}
	
	BoxTrace();
	
	if (GetWorld())
	{
		FTimerHandle DistanceCheckTimer;
		ThePlayer = Cast<ATSCharacter>(GetAvatarActorFromActorInfo());
		GetWorld()->GetTimerManager().SetTimer(DistanceCheckTimer,this, &ThisClass::CheckDistance,0.1f,true);
	}
	else
	{
		if (bShowDebug) UE_LOG(LogTemp, Warning, TEXT("월드 존재하지 않음"));
	}
	
	UAbilityTask_WaitGameplayEvent* CancelInteractTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this,ReceiveEndTag);
	CancelInteractTask->EventReceived.AddDynamic(this, &ThisClass::OnCancelInteract);
	CancelInteractTask->ReadyForActivation();
}

void UGA_Resourceinteract::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
	}
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

#pragma endregion
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma region ResourceTargeting
	//------------------------------
	// UGA_Resourceinteract 자원 타겟팅
	//------------------------------

void UGA_Resourceinteract::BoxTrace()
{
	ATSCharacter* Character = Cast<ATSCharacter>(GetAvatarActorFromActorInfo());
	const FVector Start = Character->GetActorLocation();
	const FVector End = Start + (Character->GetActorForwardVector() * CanInteractRange);
	
	TArray<FHitResult> Hits; //박스트레이스 안에 들어온 모든 거 배열로 -> 반복문으로 인터페이스 골라내기
	TArray<AActor*> Ignored;
	Ignored.Add(Character);

	if (bool bHit = UKismetSystemLibrary::BoxTraceMulti(this, Start, End, BoxHalfSize, Character->GetActorRotation(),UEngineTypes::ConvertToTraceType(ECC_Visibility), false, Ignored, EDrawDebugTrace::ForDuration, Hits, true))
	{
		for (auto& Hit : Hits)
		{
			AActor* HitActor = Hit.GetActor();
			if (!IsValid(HitActor)) continue;
			
			ITSResourceItemInterface* ResourceInterface = Cast<ITSResourceItemInterface>(HitActor);
			if (ResourceInterface)
			{
				ThePlayer = Character;
				CachedASC = GetAbilitySystemComponentFromActorInfo();
				TargetHitLocation = Hit.ImpactPoint;
				TargetHitNormal = Hit.ImpactNormal;
				TargetResource = ResourceInterface;
				TargetHaverestTimeRemaining = ResourceInterface->GetTargetHaverestTimeRemaining();
				
				UGAT_TimeRemaining* TimeRemainingTask = UGAT_TimeRemaining::StartTimeRemaining(this, TargetHaverestTimeRemaining);
				TimeRemainingTask->OnProgress.AddDynamic(this, &ThisClass::CheckTimeRemaining);
				TimeRemainingTask->CallOnEndTask.AddDynamic(this, &ThisClass::DoInteract);
				TimeRemainingTask->ReadyForActivation();
				
				return;
			}
		}
	}
}

void UGA_Resourceinteract::CheckTimeRemaining(float Progress)
{
	if (bShowDebug) UE_LOG(LogTemp, Warning, TEXT("INTERACTTAG_RESOURCE_STARTINTERACT is Not GO %f"), Progress);
	// UI 업데이트 하고 싶으면 추후에 여기 로직 추가해서...
}

void UGA_Resourceinteract::CheckDistance()
{
	if (!ThePlayer.IsValid()) return;
	
	float Distance = FVector::Dist2D(ThePlayer->GetActorLocation(), TargetHitLocation);

	if (Distance > CanInteractRange)
	{
		if (bShowDebug) UE_LOG(LogTemp, Warning, TEXT("자원 채취 소모 거리 재기 실패"));
		K2_EndAbility();
	}
}

void UGA_Resourceinteract::OnCancelInteract(FGameplayEventData Payload)
{
	K2_EndAbility();
}

void UGA_Resourceinteract::DoInteract()
{
	int32 ATK = 1;
	EItemAnimType ItemAnimType = EItemAnimType::INTERACT;
	if (!TargetResource || !CachedASC.IsValid()) return;
	
	TargetResource->GetItemFromResource(
		CachedASC.Get(), 
		ItemAnimType, 
		ATK, TargetHitLocation, 
		TargetHitNormal, 
		CachedASC.Get()->GetAvatarActor()->GetActorLocation(), 
		CachedASC.Get()->GetAvatarActor()->GetActorForwardVector(), 
		false);
	
	K2_EndAbility();
}

#pragma endregion
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////