#include "GAS/GA/GA_LeftClick.h"
#include "Character/TSCharacter.h"
#include "Kismet/KismetSystemLibrary.h"
#include "System/ResourceControl/TSResourceItemInterface.h"
#include "GAS/AttributeSet/TSAttributeSet.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "GameplayTags/AbilityGameplayTags.h"
#include "GameplayTags/ItemGameplayTags.h"
#include "Building/Actor/TSBuildingActorBase.h"
#include "Sound/Hit/HitComponent.h"

UGA_LeftClick::UGA_LeftClick()
{
}

bool UGA_LeftClick::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}
	const UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC)
	{
		return false;
	}
	const float CurrentStamina = ASC->GetNumericAttribute(UTSAttributeSet::GetStaminaAttribute());
	return CurrentStamina >= 10.0f;
}

void UGA_LeftClick::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	
	//Attack, block 태그 알림
	if (AttackTag.IsValid())
	{
		ASC->AddLooseGameplayTag(AttackTag);
	}
	if (StaminaBlockTag.IsValid())
	{
		ASC->AddLooseGameplayTag(StaminaBlockTag);
	}
	if (AttackCostEffectClass)
	{
		FGameplayEffectContextHandle ContextHandle = MakeEffectContext(Handle, ActorInfo);
		FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(AttackCostEffectClass, GetAbilityLevel(), ContextHandle);
		
		if (SpecHandle.IsValid())
		{
			ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}
	
	ATSCharacter* Character = Cast<ATSCharacter>(GetAvatarActorFromActorInfo());
	EItemAnimType Type = EItemAnimType::NONE;
	//================================
	// AnimType에 따른 몽타주 선택
	//================================
	if (Character)
	{
		Type = Character->GetAnimType(); 
	}
	switch (Type)
	{
	// ------------주먹------------
	case EItemAnimType::NONE: 
		FirstMontage = NoneMontage;
		break;
	
	// ------------도구------------
	case EItemAnimType::PICK: // 곡괭이 타입 도구
		FirstMontage = PickMontage;
		break;
	case EItemAnimType::AXE: // 도끼 타입 도구
		FirstMontage = AxeMontage;
		break;
	case EItemAnimType::HAMMER: // 도끼 타입 도구
		FirstMontage = HammerMontage;
		break;
		
	// ------------무기------------
	case EItemAnimType::WEAPON_MELEE: // 근거리 전투용 무기
		FirstMontage = WeaponMeleeMontage;
		break;
	case EItemAnimType::WEAPON_SPEAR: // 창 타입 전투용 무기 
		FirstMontage = WeaponSpearMontage;
		break;
	}
	
	
	if (FirstMontage)
	{
		//================================
		// AnimType에 따른 공격 속도 계산
		//================================
		float Speed = 1.0; 
		if (const UTSAttributeSet* AS = ASC->GetSet<UTSAttributeSet>())
		{
			Speed = AS->GetBaseAttackSpeed() * AS->GetAttackSpeedBonus();
		}
		
		// 몽타주 재생 Task
		UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, FirstMontage, Speed, NAME_None, false);
		// 노티파이 대기 Task
		UAbilityTask_WaitGameplayEvent* WaitTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this,AbilityTags::TAG_Event_Montage_HitCheck, nullptr, false, false);
		
		WaitTask->EventReceived.AddDynamic(this, &UGA_LeftClick::ReceivedNotify);
		
		// Task 종료 처리는 베이스에 이미 있는 함수 재사용
		MontageTask->OnBlendOut.AddDynamic(this, &UGA_LeftClick::OnAttackMontageFinished);
		MontageTask->OnCompleted.AddDynamic(this, &UGA_LeftClick::OnAttackMontageFinished);
		MontageTask->OnInterrupted.AddDynamic(this, &UGA_LeftClick::OnAttackMontageFinished);
		MontageTask->OnCancelled.AddDynamic(this, &UGA_LeftClick::OnAttackMontageFinished);
		
		// 태스크 활성화
		MontageTask->ReadyForActivation();
		WaitTask->ReadyForActivation();
		
	} else
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	}
}

void UGA_LeftClick::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	// 태그 없애기
	if (AttackTag.IsValid())
	{
		ASC->RemoveLooseGameplayTag(AttackTag);
	}
	if (StaminaBlockTag.IsValid())
	{
		ASC->RemoveLooseGameplayTag(StaminaBlockTag);
	}
	// attack 끝나고 1초 동안 딜레이
	if (StaminaDelayEffectClass)
	{
		FGameplayEffectContextHandle ContextHandle = MakeEffectContext(CurrentSpecHandle, CurrentActorInfo);
		FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(StaminaDelayEffectClass, GetAbilityLevel(), ContextHandle);
			
		if (SpecHandle.IsValid())
		{
			ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_LeftClick::OnAttackMontageFinished()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_LeftClick::ReceivedNotify(FGameplayEventData EventData)
{
	ATSCharacter* Character = Cast<ATSCharacter>(GetAvatarActorFromActorInfo());
	UAbilitySystemComponent* ASC = Character->GetAbilitySystemComponent();
	
	EItemAnimType Type =  Character->GetAnimType(); 
	
	//================================
	// AnimType에 따른 공격력 계산
	//================================
	float Damage = 0.0f;
	if (const UTSAttributeSet* AS = ASC->GetSet<UTSAttributeSet>())
	{
		Damage = AS->GetBaseDamage() + AS->GetDamageBonus();
	}
	int32 AttackDamage = (int32)Damage;
	// 노티파이 감지 -> 박스 트레이스 쏘기
	BoxTrace(ASC, Type, AttackDamage);
}

void UGA_LeftClick::BoxTrace(UAbilitySystemComponent* ASC, EItemAnimType ItemAnimType, int32& ATK)
{
	ATSCharacter* Character = Cast<ATSCharacter>(GetAvatarActorFromActorInfo());
	UHitComponent* HitComponent = Character->FindComponentByClass<UHitComponent>();
	
	//================================
	// AnimType에 따른 공격 범위 계산
	//================================
	if (const UTSAttributeSet* AS = ASC->GetSet<UTSAttributeSet>())
	{
		AttackRange = AS->GetBaseAttackRange() * AS->GetAttackRangeBonus();
	}
	const FVector Start = Character->GetActorLocation();
	const FVector End = Start + (Character->GetActorForwardVector() * AttackRange);
	
	TArray<FHitResult> Hits; //박스트레이스 안에 들어온 모든 거 배열로 -> 반복문으로 인터페이스 골라내기
	TArray<AActor*> Ignored;
	Ignored.Add(Character);
	
	bool bHit = UKismetSystemLibrary::BoxTraceMulti(this, Start, End, BoxHalfSize, Character->GetActorRotation(),UEngineTypes::ConvertToTraceType(ECC_Visibility), false, Ignored, EDrawDebugTrace::None, Hits, true);

	if (bHit)
	{
		for (FHitResult Hit : Hits)
		{
			AActor* HitActor = Hit.GetActor();
			if (!HitActor)
			{
				continue;
			}
			
			// 타격 사운드 재생
			if (HitComponent)
			{
				HitComponent->PlayHitSound(Hit);
			}
			
			ATSBuildingActorBase* InteractionActorBase = Cast<ATSBuildingActorBase>(HitActor);
			float DamageAmount = (float)ATK;
			if (InteractionActorBase)
			{
				if (ItemAnimType == EItemAnimType::HAMMER)
				{
					InteractionActorBase->DamageDurability(ASC,DamageAmount);
					UE_LOG(LogTemp, Log, TEXT("Hammer Hit "));
				}
				
			}
			
			ITSResourceItemInterface* ResourceInterface = Cast<ITSResourceItemInterface>(HitActor);
			if (ResourceInterface) //만약 자원원천이면 
			{
				if (ItemAnimType == EItemAnimType::NONE ||
					ItemAnimType == EItemAnimType::PICK ||
					ItemAnimType == EItemAnimType::AXE)
				{
					//함수 불러오기
					ResourceInterface->GetItemFromResource(ASC, ItemAnimType, ATK, Hit.ImpactPoint, Hit.ImpactNormal, Character->GetActorLocation(), Character->GetActorForwardVector(), true);
				}
				return;
			}

			else if (HitActor->ActorHasTag(FName("Enemy")))
			{
				UE_LOG(LogTemp, Error, TEXT("적(Enemy) 발견! 이벤트 발사!!!"));
				// 무기 타입인지 확인
				bool bIsWeapon =
					(ItemAnimType == EItemAnimType::WEAPON_MELEE
					|| ItemAnimType == EItemAnimType::WEAPON_SPEAR);
				
				UAbilitySystemComponent* MonsterASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitActor);
				if (MonsterASC)
				{
					// 몬스터 피격 처리
					FGameplayEventData EnemyAttackDamage;
					EnemyAttackDamage.EventTag = ToSendMonsterAttackTag;
					EnemyAttackDamage.Instigator = Character;
					EnemyAttackDamage.Target = HitActor;
					EnemyAttackDamage.EventMagnitude = (float)ATK;
					
					UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(HitActor, ToSendMonsterAttackTag, EnemyAttackDamage);
					
					//FGameplayTagContainer MonsterTag;
					//MonsterTag.AddTag(ToSendMonsterAttackTag);
					//MonsterASC->TryActivateAbilitiesByTag(MonsterTag);
					//FGameplayEffectContextHandle ContextHandle = MakeEffectContext(CurrentSpecHandle, CurrentActorInfo);
					//MonsterASC->BP_ApplyGameplayEffectToTarget(EnemyDamageEffectClass, MonsterASC, 1, ContextHandle);
					
					//UE_LOG(LogTemp, Warning, TEXT("몬스터한테 데미지 적용 후 시각 처리 지시"))
					//================================
					// 무기 내구도 감소 이벤트 전송
					//================================
					if (bIsWeapon)
					{
						FGameplayEventData WeaponEventData;
						WeaponEventData.EventTag = ItemTags::TAG_Event_Item_Weapon_Attack;
						WeaponEventData.EventMagnitude = 0.0f;
						WeaponEventData.Instigator = ASC->GetAvatarActor();
						WeaponEventData.Target = ASC->GetAvatarActor();
						ASC->HandleGameplayEvent(ItemTags::TAG_Event_Item_Weapon_Attack, &WeaponEventData);
							
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
						UE_LOG(LogTemp, Log,
							TEXT("[무기] 몬스터 공격 완료 - 무기 내구도 감소 이벤트 전송 (AnimType=%d)"),
							(int32)ItemAnimType);
#endif
					}
					//================================
					// 도구로 몬스터 타격 시 (예외 처리)
					//================================
					else
					{
						FGameplayEventData ToolEventData;
						ToolEventData.EventTag = ItemTags::TAG_Event_Item_Tool_Harvest;
						ToolEventData.EventMagnitude = 0.0f;
						ToolEventData.Instigator = ASC->GetAvatarActor();
						ToolEventData.Target = ASC->GetAvatarActor();
						ASC->HandleGameplayEvent(ItemTags::TAG_Event_Item_Tool_Harvest, &ToolEventData);
							
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
						UE_LOG(LogTemp, Log,
							TEXT("[도구] 몬스터 공격 완료 - 도구 내구도 감소 이벤트 전송 (AnimType=%d)"),
							(int32)ItemAnimType);
#endif
					}
				}
			}
		}
	}
}
