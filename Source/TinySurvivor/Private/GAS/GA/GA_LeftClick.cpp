#include "GAS/GA/GA_LeftClick.h"
#include "Character/TSCharacter.h"
#include "Kismet/KismetSystemLibrary.h"
#include "System/ResourceControl/TSResourceItemInterface.h"

UGA_LeftClick::UGA_LeftClick()
{
}

void UGA_LeftClick::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	ATSCharacter* Character = Cast<ATSCharacter>(GetAvatarActorFromActorInfo());
	EItemAnimType Type = EItemAnimType::NONE;
	if (Character)
	{
		Type = Character->GetAnimType(); 
	}
	switch (Type)
	{
	case EItemAnimType::NONE:
		FirstMontage = NoneMontage;
		break;
	case EItemAnimType::PICK:
		FirstMontage = PickMontage;
		break;
	case EItemAnimType::WEAPON_SPEAR:
		FirstMontage = WeaponSpearMontage;
		break;
	}
	
	PlayMontage();
	
	int32 ATK = 10;
	BoxTrace(ActorInfo->AbilitySystemComponent.Get(), Character->GetAnimType(), ATK);
	//EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UGA_LeftClick::BoxTrace(UAbilitySystemComponent* ASC, EItemAnimType ItemAnimType, int32& ATK)
{
	ATSCharacter* Character = Cast<ATSCharacter>(GetAvatarActorFromActorInfo());
	const FVector Start = Character->GetActorLocation();
	const FVector End = Start + (Character->GetActorForwardVector() * AttackRange);
	
	TArray<FHitResult> Hits; //박스트레이스 안에 들어온 모든 거 배열로 -> 반복문으로 인터페이스 골라내기
	TArray<AActor*> Ignored;
	Ignored.Add(Character);
	
	bool bHit = UKismetSystemLibrary::BoxTraceMulti(this, Start, End, BoxHalfSize, Character->GetActorRotation(),UEngineTypes::ConvertToTraceType(ECC_Visibility), false, Ignored, EDrawDebugTrace::ForDuration, Hits, true);

	if (bHit)
	{
		for (FHitResult Hit : Hits)
		{
			AActor* HitActor = Hit.GetActor();
			ITSResourceItemInterface* ResourceInterface = Cast<ITSResourceItemInterface>(HitActor);
			if (ResourceInterface) //만약 자원원천이면 
			{
				//함수 불러오기
				ResourceInterface->GetItemFromResource(ASC, ItemAnimType, ATK, Hit.ImpactPoint, Hit.ImpactNormal, Character->GetActorLocation(), Character->GetActorForwardVector(), true);
				// 주먹이라 Required Tool ID = 0
				return;
			}
		}
	}
}
