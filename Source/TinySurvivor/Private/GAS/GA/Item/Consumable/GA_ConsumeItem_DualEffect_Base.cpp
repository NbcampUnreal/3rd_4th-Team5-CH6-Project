// GA_ConsumeItem_DualEffect_Base.cpp
#include "GAS/GA/Item/Consumable/GA_ConsumeItem_DualEffect_Base.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "GameFramework/Character.h"
#include "Item/System/ItemDataSubsystem.h"
#include "GameplayTags/AbilityGameplayTags.h"
#include "GameplayTags/ItemGameplayTags.h"
#include "Character/TSCharacter.h"

// 로그 카테고리 정의
DEFINE_LOG_CATEGORY_STATIC(LogConsumeAbilityDureEffect, Log, All);

UGA_ConsumeItem_DualEffect_Base::UGA_ConsumeItem_DualEffect_Base()
{
	//=======================================================================
	// 기본 설정
	//=======================================================================
	
	/*
		Ability 인스턴싱 방식 설정
		InstancedPerActor: 각 Actor마다 Ability 인스턴스가 1개씩 생성되며,
		Ability 내부에서 상태 정보를 안전하게 저장할 수 있음.
	*/
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	
	/*
		Ability 실행 권한 설정
		- ServerInitiated:
			ServerInitiated = 클라 입력은 가능하지만 실행은 서버.
			클라이언트 입력 → 서버로 RPC 전송 → 서버가 Ability를 Activate.
			ActivateAbility()는 서버만 호출하지만,
			Ability 시작 자체는 "클라이언트 입력 기반"으로 트리거됨.
			(치트 방지 및 입력 권한 제어가 필요한 Ability에 적합)
		- ServerOnly:
			ActivateAbility는 서버만 실행, 클라는 절대 진입 불가.
			Ability ActivateAbility()는 오직 서버에서만 실행됨.
			클라이언트는 ActivateAbility() 블록에 절대 진입하지 않음.
			즉, Ability 활성화 요청을 클라이언트가 직접 발생시키지 않고,
			서버 내부 로직 또는 서버 이벤트로만 Ability가 시작됨.
			(완전한 서버 권한 기반 Ability — 아이템 소비, 인벤토리 처리, 서버 계산 전용 Ability에 적합)
	*/
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
	
	// Ability 실행 중 입력이 다시 들어와도 재실행(재트리거)되지 않도록 설정
	bRetriggerInstancedAbility = false;
	
	//=======================================================================
	// Ability Tags 설정
	//=======================================================================
	
	// 이 Ability가 활성화되는 동안 부여할 태그
	ActivationOwnedTags.AddTag(AbilityTags::TAG_State_Item_Consuming);
	
	// 이 태그가 있으면 Ability 활성화 차단
	ActivationBlockedTags.AddTag(AbilityTags::TAG_State_Item_Consuming);
	ActivationBlockedTags.AddTag(AbilityTags::TAG_State_Move_WASD);
	ActivationBlockedTags.AddTag(AbilityTags::TAG_State_Move_Sprint);
	ActivationBlockedTags.AddTag(AbilityTags::TAG_State_Move_Roll);
	ActivationBlockedTags.AddTag(AbilityTags::TAG_State_Move_Jump);
	//ActivationBlockedTags.AddTag(AbilityTags::TAG_State_Move_Crouch);
	//ActivationBlockedTags.AddTag(AbilityTags::TAG_State_Combat_Hit); // 예상
	
	// Ability 활성화에 필요한 태그
	// ActivationRequiredTags.AddTag(...);
	
	// 이 Ability가 활성화되면 취소할 다른 Ability의 태그
	// CancelAbilitiesWithTag.AddTag(...);
}

#pragma region AbilityLifecycle
void UGA_ConsumeItem_DualEffect_Base::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{// 서버 전용 Ability -> 여기 들어왔다 = 무조건 서버
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	if (!ActorInfo || !ActorInfo->AvatarActor.IsValid())
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	// Ability 실행 전에 Commit 시도 (비용 지불, 쿨다운 적용), 실패하면 Ability 종료
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	// 슬롯 인덱스 입력 처리: EventData에서 슬롯 인덱스 수신
	if (TriggerEventData)
	{
		ConsumedSlotIndex = static_cast<int32>(TriggerEventData->EventMagnitude);
		
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
		UE_LOG(LogConsumeAbilityDureEffect, Log, TEXT("받은 슬롯 인덱스: %d"), ConsumedSlotIndex);
#endif
	}
	else
	{
		UE_LOG(LogConsumeAbilityDureEffect, Warning, TEXT("TriggerEventData 없음 — 슬롯 인덱스를 수신하지 못함."));
	}
	
	//=======================================================================
	// 1. Cancel 태그 이벤트 구독
	//=======================================================================
	UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
	
	CancelTags = {
		AbilityTags::TAG_State_Move_WASD,
		AbilityTags::TAG_State_Move_Sprint,
		AbilityTags::TAG_State_Move_Crouch,
		AbilityTags::TAG_State_Move_Roll,
		AbilityTags::TAG_State_Move_Jump,
		AbilityTags::TAG_State_Move_Climb,
		// TODO: 피격 태그 생성되면 추가
		//AbilityTags::TAG_State_Combat_Hit (예상)
	};
	
	for (const FGameplayTag& Tag : CancelTags)
	{// 태그가 새로 추가되면 OnCancelTagChanged 호출
		ASC->RegisterGameplayTagEvent(Tag, EGameplayTagEventType::NewOrRemoved)
				.AddUObject(this, &UGA_ConsumeItem_DualEffect_Base::OnCancelTagChanged);
	}
	
	//=======================================================================
	// 2단계: 아이템 데이터 로드
	//=======================================================================
	if (!LoadItemData())
	{// 아이템 데이터를 불러오는 데 실패한 경우
		UE_LOG(LogConsumeAbilityDureEffect, Error, TEXT("아이템 ID %d의 데이터를 불러오지 못했습니다."), ItemID);
		
		// Ability를 취소 처리하고 종료
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	// 디버그 로그
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	LogItemInfo();
#endif
	
	//=======================================================================
	// 3단계: ConsumptionTime 대기
	//=======================================================================
	bIsConsuming = true;
	WaitForConsumption();
}

void UGA_ConsumeItem_DualEffect_Base::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled)
{
	// 현재 Ability가 소비 중 상태임을 나타내는 플래그 초기화
	bIsConsuming = false;
	
	// Task 종료 (서버에서만)
	if (GetCurrentActorInfo()->IsNetAuthority())
	{
		if (ActiveDelayTask.IsValid())
		{
			ActiveDelayTask->EndTask();
			ActiveDelayTask = nullptr;
		}
		
		// 몽타주 정지 (Multicast)
		if (CachedItemData.ConsumableData.ConsumptionMontage.IsValid())
		{
			UAnimMontage* Montage = CachedItemData.ConsumableData.ConsumptionMontage.LoadSynchronous();
			if (Montage)
			{
				ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
				if (Character)
				{
					ATSCharacter* TSCharacter = Cast<ATSCharacter>(Character);
					if (TSCharacter)
					{
						TSCharacter->Multicast_StopConsumeMontage(Montage);
						
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
						UE_LOG(LogConsumeAbilityDureEffect, Log, TEXT("[Server] 몽타주 정지 Multicast 호출"));
#endif
					}
				}
			}
		}
	}
	
	// Tag 이벤트 해제
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (ASC)
	{
		for (const FGameplayTag& Tag : CancelTags)
		{
			ASC->RegisterGameplayTagEvent(Tag, EGameplayTagEventType::NewOrRemoved)
				.RemoveAll(this);
		}
	}
	
	// 타이머 정리
	// if (MovementCheckTimerHandle.IsValid())
	// {
	// 	GetWorld()->GetTimerManager().ClearTimer(MovementCheckTimerHandle);
	// }
	
	// 취소 로그
	if (bWasCancelled)
	{
		UE_LOG(LogConsumeAbilityDureEffect, Warning, TEXT("아이템 ID %d의 소비 어빌리티가 취소되었습니다."), ItemID);
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
#pragma endregion

#pragma region LoadItemData
bool UGA_ConsumeItem_DualEffect_Base::LoadItemData()
{
	// ItemID 유효성 검사
	if (ItemID <= 0)
	{
		UE_LOG(LogConsumeAbilityDureEffect, Error, TEXT("유효하지 않은 ItemID: %d (0보다 커야 함)"), ItemID);
		return false;
	}
	
	// ItemDataSubsystem 가져오기
	UItemDataSubsystem* ItemDataSys = UItemDataSubsystem::GetItemDataSubsystem(this);
	if (!ItemDataSys)
	{
		UE_LOG(LogConsumeAbilityDureEffect, Error, TEXT("ItemDataSubsystem을 찾을 수 없습니다!"));
		return false;
	}
	
	// 아이템 데이터 조회
	if (!ItemDataSys->GetItemDataSafe(ItemID, CachedItemData))
	{
		UE_LOG(LogConsumeAbilityDureEffect, Error, TEXT("ItemID %d에 대한 아이템 데이터를 찾을 수 없습니다"), ItemID);
		return false;
	}
	
	// 소모품 카테고리 검증
	if (CachedItemData.Category != EItemCategory::CONSUMABLE)
	{
		UE_LOG(LogConsumeAbilityDureEffect, Error, 
			TEXT("ItemID %d는 소모품이 아닙니다! 카테고리: %d"),
			ItemID, static_cast<int32>(CachedItemData.Category));
		return false;
	}
	
	// 효과 태그 검증
	if (!CachedItemData.HasEffect())
	{
		UE_LOG(LogConsumeAbilityDureEffect, Error, TEXT("ItemID %d에는 효과 태그가 없습니다!"), ItemID);
		return false;
	}
	
	// 버프 아이템은 아예 사용 불가 처리
	// if (CachedItemData.ConsumableData.EffectDuration > 0.0f)
	// {
	// 	UE_LOG(LogConsumeAbilityDureEffect, Error,
	// 		TEXT("ItemID %d is a buff item (Duration=%.1f). Buff items are not yet implemented!"),
	// 		ItemID, CachedItemData.ConsumableData.EffectDuration);
	// 	return false; 
	// }
	
	return true;
}
#pragma endregion

#pragma region Consumption
void UGA_ConsumeItem_DualEffect_Base::WaitForConsumption()
{
	//=======================================================================
	// 캐싱
	//=======================================================================
	
	// 아이템 소비에 걸리는 시간
	float ConsumptionTime = CachedItemData.ConsumableData.ConsumptionTime;
	
	// 소비 시 재생할 애니메이션 몽타주
	UAnimMontage* ConsumeMontage = CachedItemData.ConsumableData.ConsumptionMontage.LoadSynchronous();
	
	//=======================================================================
	// 몽타주가 없거나 소비 시간이 0이면 즉시 효과 적용
	//=======================================================================
	if (!ConsumeMontage || ConsumptionTime <= 0.0f)
	{
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
		UE_LOG(LogConsumeAbilityDureEffect, Log,
			TEXT("아이템 ID %d는 소비 시간이나 몽타주가 없어 즉시 효과를 적용합니다."), ItemID);
#endif
		OnConsumptionCompleted();
		return;
	}
	
	//=======================================================================
	// 몽타주는 이미 Multicast로 재생되었으므로, 여기서는 Delay만 사용
	//=======================================================================
	if (ConsumptionTime > 0.0f)
	{
		// Task 생성
		// 지정된 시간 동안 대기 후 이벤트를 호출하는 Unreal Ability Task
		ActiveDelayTask = UAbilityTask_WaitDelay::WaitDelay(this, ConsumptionTime);
		if (!ActiveDelayTask.IsValid())
		{
			UE_LOG(LogConsumeAbilityDureEffect, Error, TEXT("WaitDelay Task 생성에 실패했습니다!"));
		
			// Ability 강제 종료
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
			return;
		}
		
		// Task 완료 콜백 바인딩
		ActiveDelayTask->OnFinish.AddDynamic(this, &UGA_ConsumeItem_DualEffect_Base::OnConsumptionCompleted);
		
		// Task 활성화
		ActiveDelayTask->ReadyForActivation();
		
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
		UE_LOG(LogConsumeAbilityDureEffect, Log, TEXT("소비 대기 중: %.1f 초 (ItemID: %d)"),
			ConsumptionTime, ItemID);
#endif
	}
	
	//=======================================================================
	// 움직임 체크 타이머 시작
	//=======================================================================
	// GetWorld()->GetTimerManager().SetTimer(
	// 	MovementCheckTimerHandle,
	// 	this, // 콜백을 호출할 객체 (UGA_ConsumeItem_DualEffect_Base 인스턴스)
	// 	&UGA_ConsumeItem_DualEffect_Base::CheckMovement, // 주기적으로 호출할 함수 포인터
	// 	0.1f, // 호출 간격, 0.1초마다 실행
	// 	true  // 반복 여부, true → 0.1초마다 계속 반복
	// );
}

void UGA_ConsumeItem_DualEffect_Base::OnConsumptionCompleted()
{
	if (!bIsConsuming)
	{
		return;
	}
	
	ApplyEffect(); // GameplayEffect 적용
	NotifyItemConsumed(); // 인벤토리에 아이템 소비 알림
	
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	UE_LOG(LogConsumeAbilityDureEffect, Log, TEXT("아이템 소비가 완료되었습니다. ItemID: %d"), ItemID);
#endif
	
	// Ability 종료
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_ConsumeItem_DualEffect_Base::OnConsumptionCancelled()
{
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	UE_LOG(LogConsumeAbilityDureEffect, Warning, TEXT("아이템 소비가 취소되었습니다. ItemID: %d"), ItemID);
#endif
	
	// Ability 취소 종료
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UGA_ConsumeItem_DualEffect_Base::OnCancelTagChanged(const FGameplayTag Tag, int32 NewCount)
{
	// 태그가 새로 추가되었을 때만 처리
	if (NewCount > 0)
	{
		// Ability 강제 종료
		CancelAbility(
			GetCurrentAbilitySpecHandle(),
			GetCurrentActorInfo(),
			GetCurrentActivationInfo(),
			true // bReplicateCancel: 서버/클라이언트 동기화
		);
		
		// 기존 취소 처리 함수 호출
		OnConsumptionCancelled();
	}
}
#pragma endregion

#pragma region CheckMovement
// void UGA_ConsumeItem_DualEffect_Base::CheckMovement()
// {
// 	// Ability가 적용되는 캐릭터 객체 가져오기, 캐릭터가 아니면 함수 종료
// 	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
// 	if (!Character)
// 	{
// 		return;
// 	}
// 	
// 	//=======================================================================
// 	// 캐릭터가 움직이면 취소
// 	//=======================================================================
// 	
// 	// 캐릭터의 현재 이동 속도 구하기
// 	// X, Y 평면(지면) 속도만 계산, 수직(Z) 방향은 무시
// 	float CurrentSpeed = Character->GetVelocity().Size2D();
// 	if (CurrentSpeed > 10.0f) // 10cm/s 이상 움직임
// 	{
// 		UE_LOG(LogConsumeAbilityDureEffect, Warning, TEXT("소비 취소됨: 캐릭터가 이동 중 (속도: %.1f)"), CurrentSpeed);
// 		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo,true, true);
// 	}
// }
#pragma endregion

#pragma region EffectApplication
void UGA_ConsumeItem_DualEffect_Base::ApplyEffect()
{
	//=======================================================================
	// 1. GameplayEffect 클래스 유효성 검사
	//=======================================================================
	if (!RemoveTagEffectClass)
	{
		UE_LOG(LogConsumeAbilityDureEffect, Error, TEXT("RemoveTagEffectClass가 설정되지 않았습니다!"));
		return;
	}
	if (!AddTagEffectClass)
	{
		UE_LOG(LogConsumeAbilityDureEffect, Error, TEXT("AddTagEffectClass가 설정되지 않았습니다!"));
		return;
	}
	
	//=======================================================================
	// 2. AbilitySystemComponent 가져오기
	//=======================================================================
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC)
	{
		UE_LOG(LogConsumeAbilityDureEffect, Error, TEXT("AbilitySystemComponent를 찾을 수 없습니다!"));
		return;
	}
	
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	UE_LOG(LogConsumeAbilityDureEffect, Log, TEXT("========================================"));
	UE_LOG(LogConsumeAbilityDureEffect, Log, TEXT("아이템 ID %d 이중 효과 적용 시작"), ItemID);
	UE_LOG(LogConsumeAbilityDureEffect, Log, TEXT("========================================"));
#endif
	
	//=======================================================================
	// 3. 첫 번째 GE 적용: RemoveTag Effect (Instant)
	//=======================================================================
	{
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
		UE_LOG(LogConsumeAbilityDureEffect, Log, TEXT("[1/2] RemoveTag Effect 적용 시작"));
#endif
		
		// Context 생성
		FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
		ContextHandle.AddSourceObject(this);
		
		// Spec 생성
		FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(
			RemoveTagEffectClass,
			GetAbilityLevel(),
			ContextHandle
		);
		
		if (!SpecHandle.IsValid())
		{
			UE_LOG(LogConsumeAbilityDureEffect, Error, TEXT("RemoveTag EffectSpec 생성 실패!"));
			return;
		}
		
		// SetByCaller로 ItemID 전달 (GEC에서 사용 가능)
		FGameplayTag ItemIDTag = ItemTags::TAG_Data_ItemID;
		SpecHandle.Data->SetSetByCallerMagnitude(ItemIDTag, static_cast<float>(ItemID));
		
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
		UE_LOG(LogConsumeAbilityDureEffect, Log, TEXT("  - ItemID: %d"), ItemID);
		UE_LOG(LogConsumeAbilityDureEffect, Log, TEXT("  - GE Class: %s"), *RemoveTagEffectClass->GetName());
#endif
		
		// GE 적용
		FActiveGameplayEffectHandle RemoveHandle = 
			ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		
		if (!RemoveHandle.WasSuccessfullyApplied())
		{
			UE_LOG(LogConsumeAbilityDureEffect, Error, TEXT("RemoveTag Effect 적용 실패!"));
			return;
		}
		
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
		UE_LOG(LogConsumeAbilityDureEffect, Log, TEXT("[1/2] RemoveTag Effect 적용 완료"));
#endif
	}
	
	//=======================================================================
	// 4. 두 번째 GE 적용: AddTag Effect (Duration)
	//=======================================================================
	{
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
		UE_LOG(LogConsumeAbilityDureEffect, Log, TEXT("[2/2] AddTag Effect 적용 시작"));
#endif
		
		// Context 생성
		FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
		ContextHandle.AddSourceObject(this);
		
		// Spec 생성
		FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(
			AddTagEffectClass,
			GetAbilityLevel(),
			ContextHandle
		);
		
		if (!SpecHandle.IsValid())
		{
			UE_LOG(LogConsumeAbilityDureEffect, Error, TEXT("AddTag EffectSpec 생성 실패!"));
			return;
		}
		
		// SetByCaller로 ItemID 전달
		FGameplayTag ItemIDTag = ItemTags::TAG_Data_ItemID;
		SpecHandle.Data->SetSetByCallerMagnitude(ItemIDTag, static_cast<float>(ItemID));
		
		// Duration 설정 (HasDuration GE만 해당)
		float EffectDuration = CachedItemData.ConsumableData.EffectDuration;
		if (EffectDuration > 0.0f)
		{
			// GE의 DurationPolicy 확인
			UGameplayEffect* GECDO = AddTagEffectClass->GetDefaultObject<UGameplayEffect>();
			if (GECDO && GECDO->DurationPolicy == EGameplayEffectDurationType::HasDuration)
			{
				SpecHandle.Data->SetDuration(EffectDuration, false);
				
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
				UE_LOG(LogConsumeAbilityDureEffect, Log, TEXT("  - Duration: %.1f초"), EffectDuration);
#endif
			}
		}
		
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
		UE_LOG(LogConsumeAbilityDureEffect, Log, TEXT("  - ItemID: %d"), ItemID);
		UE_LOG(LogConsumeAbilityDureEffect, Log, TEXT("  - GE Class: %s"), *AddTagEffectClass->GetName());
		UE_LOG(LogConsumeAbilityDureEffect, Log, TEXT("  - EffectTag: %s"), *CachedItemData.EffectTag_Consumable.ToString());
#endif
		
		// GE 적용
		FActiveGameplayEffectHandle AddHandle = 
			ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		
		if (!AddHandle.WasSuccessfullyApplied())
		{
			UE_LOG(LogConsumeAbilityDureEffect, Error, TEXT("AddTag Effect 적용 실패!"));
			return;
		}
		
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
		UE_LOG(LogConsumeAbilityDureEffect, Log, TEXT("[2/2] AddTag Effect 적용 완료"));
#endif
	}
	
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	//=======================================================================
	// 5. 완료 로그
	//=======================================================================
	UE_LOG(LogConsumeAbilityDureEffect, Log, TEXT("========================================"));
	UE_LOG(LogConsumeAbilityDureEffect, Log, TEXT("아이템 ID %d 이중 효과 적용 성공"), ItemID);
	UE_LOG(LogConsumeAbilityDureEffect, Log, TEXT("  1. RemoveTag: %s (Instant)"), *RemoveTagEffectClass->GetName());
	UE_LOG(LogConsumeAbilityDureEffect, Log, TEXT("  2. AddTag: %s (Duration: %.1fs)"), 
			*AddTagEffectClass->GetName(), CachedItemData.ConsumableData.EffectDuration);
	UE_LOG(LogConsumeAbilityDureEffect, Log, TEXT("========================================"));
#endif
}
#pragma endregion

#pragma region InventoryNotification
void UGA_ConsumeItem_DualEffect_Base::NotifyItemConsumed()
{
	//=======================================================================
	// GameplayEvent 발송하여 인벤토리에 알림
	//=======================================================================
	
	// 이 Ability를 가진 캐릭터(또는 소유자)를 가져오기
	AActor* OwnerActor = GetAvatarActorFromActorInfo();
	if (!OwnerActor)
	{
		return;
	}
	
	// 이벤트 태그 준비
	// GAS에서는 이벤트를 태그로 구분
	FGameplayTag ConsumedTag = ItemTags::TAG_Event_Item_Consumed;
	
	// 이벤트 데이터 준비
	// 이 이벤트는 내가 발생시켰고, 나 자신에게 적용되며, 소비된 슬롯 번호는 X다
	FGameplayEventData EventData;
	EventData.EventTag = ConsumedTag;
	EventData.EventMagnitude = static_cast<float>(ConsumedSlotIndex); // 슬롯 인덱스 전달
	EventData.Instigator = OwnerActor; // 이벤트를 발생시킨 주체
	EventData.Target = OwnerActor; // 이벤트를 받을 대상
	
	// 이벤트 발송
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
		OwnerActor,
		ConsumedTag,
		EventData
	);
	
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	UE_LOG(LogConsumeAbilityDureEffect, Log,
		TEXT("Item.Consumed 이벤트 발송됨: 슬롯 인덱스=%d, 아이템 ID=%d"),
		ConsumedSlotIndex, ItemID);
#endif
}
#pragma endregion

#pragma region Debug
void UGA_ConsumeItem_DualEffect_Base::LogItemInfo() const
{
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	UE_LOG(LogConsumeAbilityDureEffect, Display, TEXT("======================================================================="));
	UE_LOG(LogConsumeAbilityDureEffect, Display, TEXT("Consume Item Info:"));
	UE_LOG(LogConsumeAbilityDureEffect, Display, TEXT("  ItemID: %d"), ItemID);
	UE_LOG(LogConsumeAbilityDureEffect, Display, TEXT("  Name: %s"), *CachedItemData.Name_EN.ToString());
	UE_LOG(LogConsumeAbilityDureEffect, Display, TEXT("  EffectTag: %s"), *CachedItemData.EffectTag_Consumable.ToString());
	UE_LOG(LogConsumeAbilityDureEffect, Display, TEXT("  EffectValue: %.1f"), CachedItemData.EffectValue);
	UE_LOG(LogConsumeAbilityDureEffect, Display, TEXT("  ConsumptionTime: %.1f"), CachedItemData.ConsumableData.ConsumptionTime);
	UE_LOG(LogConsumeAbilityDureEffect, Display, TEXT("  EffectDuration: %.1f"), CachedItemData.ConsumableData.EffectDuration);
	UE_LOG(LogConsumeAbilityDureEffect, Display, TEXT("======================================================================="));
#endif
}
#pragma endregion