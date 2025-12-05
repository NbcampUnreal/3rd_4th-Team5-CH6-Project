// GA_ConsumeItem_Base.cpp
#include "GAS/GA/Item/Consumable/GA_ConsumeItem_Base.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "GameFramework/Character.h"
#include "Item/System/ItemDataSubsystem.h"
#include "GameplayTags/AbilityGameplayTags.h"
#include "GameplayTags/ItemGameplayTags.h"

// 로그 카테고리 정의
DEFINE_LOG_CATEGORY_STATIC(LogConsumeAbility, Log, All);

UGA_ConsumeItem_Base::UGA_ConsumeItem_Base()
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
			Ability 활성화 요청은 서버에서 처리되며,
			클라이언트 입력은 서버로 전달되어 서버가 Ability를 시작함.
			(치트 방지 및 권한 제어가 필요한 Ability에 적합)
	*/
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;
	
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
void UGA_ConsumeItem_Base::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	if (!ActorInfo)
	{
		return;
	}
	
	// Ability 실행 전에 Commit 시도 (비용 지불, 쿨다운 적용), 실패하면 Ability 종료
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	// 서버에서만 실행
	if (!ActorInfo->IsNetAuthority())
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}
	
	// EventData에서 슬롯 인덱스 수신
	if (TriggerEventData)
	{
		ConsumedSlotIndex = static_cast<int32>(TriggerEventData->EventMagnitude);
		UE_LOG(LogConsumeAbility, Log, TEXT("받은 슬롯 인덱스: %d"), ConsumedSlotIndex);
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
				.AddUObject(this, &UGA_ConsumeItem_Base::OnCancelTagChanged);
	}
	
	//=======================================================================
	// 2단계: 아이템 데이터 로드
	//=======================================================================
	if (!LoadItemData())
	{// 아이템 데이터를 불러오는 데 실패한 경우
		UE_LOG(LogConsumeAbility, Error, TEXT("아이템 ID %d의 데이터를 불러오지 못했습니다."), ItemID);
		
		// Ability를 취소 처리하고 종료
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	// 디버그 로그
	LogItemInfo();
	
	//=======================================================================
	// 3단계: ConsumptionTime 대기
	//=======================================================================
	bIsConsuming = true;
	WaitForConsumption();
}

void UGA_ConsumeItem_Base::EndAbility(
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
		if (ActiveMontageTask.IsValid())
		{
			ActiveMontageTask->EndTask();
			ActiveMontageTask = nullptr;
		}
		if (ActiveDelayTask.IsValid())
		{
			ActiveDelayTask->EndTask();
			ActiveDelayTask = nullptr;
		}
		
		// 모든 클라이언트의 몽타주 정지
		Multicast_StopConsumeMontage();
	}
	
	// Tag 이벤트를 해제
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (ASC)
	{
		for (const FGameplayTag& Tag : CancelTags)
		{
			ASC->RegisterGameplayTagEvent(Tag, EGameplayTagEventType::NewOrRemoved)
				.RemoveAll(this);
		}
	}
	
	// 캐릭터 가져오기
	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!Character)
	{
		UE_LOG(LogConsumeAbility, Warning, TEXT("캐릭터를 가져오지 못했습니다."));
		Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
		return;
	}
	
	// 애니메이션 인스턴스 가져오기
	UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance();
	if (!AnimInstance)
	{
		UE_LOG(LogConsumeAbility, Warning, TEXT("애니메이션 인스턴스를 가져오지 못했습니다."));
		Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
		return;
	}
	
	// Consume 몽타주 즉시 정지
	if (CachedItemData.ConsumableData.ConsumptionMontage.IsValid())
	{
		UAnimMontage* Montage = CachedItemData.ConsumableData.ConsumptionMontage.LoadSynchronous();
		if (Montage && AnimInstance->Montage_IsPlaying(Montage))
		{
			AnimInstance->Montage_Stop(0.2f, Montage); // 0.2초 블렌드 아웃
			UE_LOG(LogConsumeAbility, Log, TEXT("아이템 ID %d의 몽타주를 정지했습니다."), ItemID);
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
		UE_LOG(LogConsumeAbility, Warning, TEXT("아이템 ID %d의 소비 어빌리티가 취소되었습니다."), ItemID);
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
#pragma endregion

#pragma region Multicast
void UGA_ConsumeItem_Base::Multicast_PlayConsumeMontage_Implementation(UAnimMontage* Montage, float ServerStartTime)
{
	UE_LOG(LogConsumeAbility, Log, TEXT("[Multicast] PlayConsumeMontage 호출 시작"));
	
	if (GetAvatarActorFromActorInfo()->HasAuthority())
	{// 서버는 재생하지 않음 (중복 재생 방지)
		UE_LOG(LogConsumeAbility, Log, TEXT("[Server] Multicast 스킵 (Task가 이미 실행 중)"));
		return;
	}
	
	// 클라이언트만 여기서 몽타주 재생
	UE_LOG(LogConsumeAbility, Log, TEXT("[Client] Multicast로 몽타주 재생"));
	
	if (!Montage)
	{
		UE_LOG(LogConsumeAbility, Warning, TEXT("[Multicast] Montage가 nullptr입니다! 함수 종료"));
		return;
	}
	
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor)
	{
		UE_LOG(LogConsumeAbility, Warning, TEXT("[Multicast] AvatarActor가 nullptr입니다! 함수 종료"));
		return;
	}
	
	ACharacter* Character = Cast<ACharacter>(AvatarActor);
	if (!Character)
	{
		UE_LOG(LogConsumeAbility, Warning, TEXT("[Multicast] AvatarActor를 ACharacter로 캐스트 실패: %s"), *AvatarActor->GetName());
		return;
	}
	
	UE_LOG(LogConsumeAbility, Log, TEXT("[Multicast] 캐릭터 찾음: %s"), *Character->GetName());
	
	USkeletalMeshComponent* MeshComp = Character->GetMesh();
	if (!MeshComp)
	{
		UE_LOG(LogConsumeAbility, Warning, TEXT("[Multicast] 캐릭터의 Mesh가 nullptr입니다!"));
		return;
	}
	
	UAnimInstance* AnimInstance = MeshComp->GetAnimInstance();
	if (!AnimInstance)
	{
		UE_LOG(LogConsumeAbility, Warning, TEXT("[Multicast] AnimInstance가 nullptr입니다!"));
		return;
	}
	
	//=======================================================================
	// 실제 몽타주 재생
	// 서버 시작 시간과 현재 시간 차이만큼 건너뛰고 재생
	//=======================================================================
	
	// 1. 현재 클라이언트 시간
	float LocalTime = GetWorld()->GetTimeSeconds();
	
	// 2. 서버가 시작한 시점부터 얼마나 지났는지 계산
	float Elapsed = FMath::Max(LocalTime - ServerStartTime, 0.0f); // 음수 방지 (FMath::Max 적용)
	
	// 3. 몽타주 길이 초과 방지
	float MontageLength = Montage->GetPlayLength();
	if (Elapsed >= MontageLength)
	{
		// 옵션 1: 그냥 스킵
		// 이미 몽타주가 끝난 시점이면 재생하지 않음
		UE_LOG(LogConsumeAbility, Warning,
			TEXT("몽타주가 이미 끝났습니다 (경과 시간=%.2f, 길이=%.2f)"), Elapsed, MontageLength);
		return;
		
		// 옵션 2: 즉시 완료 처리 (필요시 활성화)
		// 극단적 지연 상황이 생기면 활성화 예정
		// 몽타주를 재생하지 않고 즉시 완료 처리
		//OnConsumptionCompleted();
		//return;
	}
	
	// 4. 몽타주 재생 (이미 지난 시간만큼 건너뛰고 재생)
	float PlayedLength = AnimInstance->Montage_Play(Montage, 1.0f, EMontagePlayReturnType::MontageLength, Elapsed);
	if (PlayedLength <= 0.f)
	{
		UE_LOG(LogConsumeAbility, Warning, TEXT("[Multicast] Montage_Play 실패: %s"), *Montage->GetName());
	}
	else
	{
		UE_LOG(LogConsumeAbility, Log, TEXT("[Multicast] 몽타주 재생 성공: %s, 길이: %.2f초, 시작 위치: %.2f초"), 
			*Montage->GetName(), MontageLength, Elapsed);
	}
}

void UGA_ConsumeItem_Base::Multicast_StopConsumeMontage_Implementation()
{
	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!Character) return;
	
	UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance();
	if (!AnimInstance) return;
	
	if (CachedItemData.ConsumableData.ConsumptionMontage.IsValid())
	{
		UAnimMontage* Montage = CachedItemData.ConsumableData.ConsumptionMontage.LoadSynchronous();
		if (Montage && AnimInstance->Montage_IsPlaying(Montage))
		{
			AnimInstance->Montage_Stop(0.2f, Montage);
		}
	}
}
#pragma endregion

#pragma region LoadItemData
bool UGA_ConsumeItem_Base::LoadItemData()
{
	// ItemID 유효성 검사
	if (ItemID <= 0)
	{
		UE_LOG(LogConsumeAbility, Error, TEXT("Invalid ItemID: %d (Must be > 0)"), ItemID);
		return false;
	}
	
	// ItemDataSubsystem 가져오기
	UItemDataSubsystem* ItemDataSys = UItemDataSubsystem::GetItemDataSubsystem(this);
	if (!ItemDataSys)
	{
		UE_LOG(LogConsumeAbility, Error, TEXT("ItemDataSubsystem not found!"));
		return false;
	}
	
	// 아이템 데이터 조회
	if (!ItemDataSys->GetItemDataSafe(ItemID, CachedItemData))
	{
		UE_LOG(LogConsumeAbility, Error, TEXT("Item data not found for ItemID: %d"), ItemID);
		return false;
	}
	
	// 소모품 카테고리 검증
	if (CachedItemData.Category != EItemCategory::CONSUMABLE)
	{
		UE_LOG(LogConsumeAbility, Error, 
			TEXT("ItemID %d is not a consumable! Category: %d"),
			ItemID, static_cast<int32>(CachedItemData.Category));
		return false;
	}
	
	// 효과 태그 검증
	if (!CachedItemData.HasEffect())
	{
		UE_LOG(LogConsumeAbility, Error, TEXT("ItemID %d has no effect tag!"), ItemID);
		return false;
	}
	
	// 버프 아이템은 아예 사용 불가 처리
	// if (CachedItemData.ConsumableData.EffectDuration > 0.0f)
	// {
	// 	UE_LOG(LogConsumeAbility, Error,
	// 		TEXT("ItemID %d is a buff item (Duration=%.1f). Buff items are not yet implemented!"),
	// 		ItemID, CachedItemData.ConsumableData.EffectDuration);
	// 	return false; 
	// }
	
	return true;
}
#pragma endregion

#pragma region Consumption
void UGA_ConsumeItem_Base::WaitForConsumption()
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
		UE_LOG(LogConsumeAbility, Log, TEXT("아이템 ID %d는 소비 시간이나 몽타주가 없어 즉시 효과를 적용합니다."), ItemID);
		OnConsumptionCompleted();
		return;
	}
	
	if (ConsumeMontage)
	{
		//=======================================================================
		// Montage가 있으면 Montage Task 사용
		//=======================================================================
		
		// Task 생성 (서버에서만)
		ActiveMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this, NAME_None, ConsumeMontage);
		
		// 콜백 바인딩 - 소비 완료 시 호출
		ActiveMontageTask->OnCompleted.AddDynamic(this, &UGA_ConsumeItem_Base::OnConsumptionCompleted);
		
		// 콜백 바인딩 - 소비 취소 시 호출
		ActiveMontageTask->OnInterrupted.AddDynamic(this, &UGA_ConsumeItem_Base::OnConsumptionCancelled);
		ActiveMontageTask->OnCancelled.AddDynamic(this, &UGA_ConsumeItem_Base::OnConsumptionCancelled);
		
		// Task를 활성화하고 대기 시작
		// Task가 만들어졌다고 바로 실행되는 것은 아니므로 ReadyForActivation() 필수
		ActiveMontageTask->ReadyForActivation();
		
		UE_LOG(LogConsumeAbility, Log, TEXT("아이템 ID %d의 몽타주를 재생합니다."), ItemID);
		UE_LOG(LogConsumeAbility, Log,
			TEXT("Montage Name: %s, Length: %.2f, Ability Owner: %s"),
			*ConsumeMontage->GetName(),
			ConsumeMontage->GetPlayLength(),
			*GetAvatarActorFromActorInfo()->GetName());
		
		// 서버 시작 시간 기록
		float ServerStartTime = GetWorld()->GetTimeSeconds();
		
		// Multicast 호출 (모든 클라이언트 + 서버에 전달)
		Multicast_PlayConsumeMontage(ConsumeMontage, ServerStartTime);
	}
	else
	{
		//=======================================================================
		// Montage가 없으면 ConsumptionTime 만큼 WaitDelay fallback
		//=======================================================================
		
		// Task 생성
		// 지정된 시간 동안 대기 후 이벤트를 호출하는 Unreal Ability Task
		ActiveDelayTask = UAbilityTask_WaitDelay::WaitDelay(this, ConsumptionTime);
		if (!ActiveDelayTask.IsValid())
		{
			UE_LOG(LogConsumeAbility, Error, TEXT("WaitDelay Task 생성에 실패했습니다!"));
			
			// Ability 강제 종료
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
			return;
		}
		
		// Task 완료 콜백 바인딩
		ActiveDelayTask->OnFinish.AddDynamic(this, &UGA_ConsumeItem_Base::OnConsumptionCompleted);
		
		// Task 활성화
		ActiveDelayTask->ReadyForActivation();
		
		UE_LOG(LogConsumeAbility, Log,
			TEXT("소비 대기 중: %.1f 초 (ItemID: %d)"),
			ConsumptionTime, ItemID);
	}
	
	//=======================================================================
	// 움직임 체크 타이머 시작
	//=======================================================================
	// GetWorld()->GetTimerManager().SetTimer(
	// 	MovementCheckTimerHandle,
	// 	this, // 콜백을 호출할 객체 (UGA_ConsumeItem_Base 인스턴스)
	// 	&UGA_ConsumeItem_Base::CheckMovement, // 주기적으로 호출할 함수 포인터
	// 	0.1f, // 호출 간격, 0.1초마다 실행
	// 	true  // 반복 여부, true → 0.1초마다 계속 반복
	// );
}

void UGA_ConsumeItem_Base::OnConsumptionCompleted()
{
	if (!bIsConsuming)
	{
		return;
	}
	
	// GameplayEffect 적용
	ApplyRestoreEffect();
	
	// 인벤토리에 아이템 소비 알림
	NotifyItemConsumed();
	
	UE_LOG(LogConsumeAbility, Log, TEXT("아이템 ID %d의 소비가 완료되었습니다."), ItemID);
	
	// Ability 종료
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_ConsumeItem_Base::OnConsumptionCancelled()
{
	UE_LOG(LogConsumeAbility, Warning, TEXT("Consumption cancelled for ItemID: %d"), ItemID);
	
	// Ability 취소 종료
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UGA_ConsumeItem_Base::OnCancelTagChanged(const FGameplayTag Tag, int32 NewCount)
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
// void UGA_ConsumeItem_Base::CheckMovement()
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
// 		UE_LOG(LogConsumeAbility, Warning, TEXT("소비 취소됨: 캐릭터가 이동 중 (속도: %.1f)"), CurrentSpeed);
// 		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo,true, true);
// 	}
// }
#pragma endregion

#pragma region EffectApplication
void UGA_ConsumeItem_Base::ApplyRestoreEffect()
{
	// RestoreEffectClass 유효성 검사
	if (!RestoreEffectClass)
	{// GameplayEffect 클래스가 설정되지 않은 경우
		UE_LOG(LogConsumeAbility, Error, TEXT("RestoreEffectClass가 설정되지 않았습니다!"));
		return;
	}
	
	// AbilitySystemComponent 가져오기
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC)
	{// AbilitySystemComponent를 찾을 수 없는 경우
		UE_LOG(LogConsumeAbility, Error, TEXT("AbilitySystemComponent를 찾을 수 없습니다!"));
		return;
	}
	
	//=======================================================================
	// GameplayEffectSpec 생성
	// GameplayEffectSpec: 실제로 누구에게 어떤 효과를 어떻게 적용할지를 담은 "설계도"
	//=======================================================================
	// ContextHandle은 이 회복 효과가 어떤 아이템에서 발생했는지, 누가 사용했는지를 기록
	FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
	
	// this는 현재 이 Ability(즉, UGA_ConsumeItem_Base) 객체 자체
	// 즉, 이 효과의 출처를 Context에 기록
	ContextHandle.AddSourceObject(this);
	
	// 실제 적용할 효과의 설계도(스펙) 만들기
	FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(
		RestoreEffectClass,	// 어떤 효과를 적용할지 선택
		GetAbilityLevel(),	// 효과 레벨 (나중에 강력한 아이템일수록 더 큰 회복량)
		ContextHandle	// 누가, 어디서, 어떤 상황에서 적용했는지 정보
	);
	
	if (!SpecHandle.IsValid())
	{// GameplayEffectSpec 생성에 실패한 경우
		UE_LOG(LogConsumeAbility, Error, TEXT("GameplayEffectSpec 생성에 실패했습니다!"));
		return;
	}
	
	//=======================================================================
	// SetByCaller로 ItemID 전달 (GEC에서 아이템 정보 재조회)
	// SetByCaller를 사용하면, 런타임에 값 전달 가능
	// 아이템 ID를 GE에 전달
	//=======================================================================
	FGameplayTag ItemIDTag = ItemTags::TAG_Data_ItemID;
	SpecHandle.Data->SetSetByCallerMagnitude(ItemIDTag, static_cast<float>(ItemID));
	
	//=======================================================================
	// EffectDuration 설정 (0보다 크면 지속 효과)
	// 단순 회복 아이템은 순간 적용만 하고, 지속 회복/버프는 다른 GE에서 처리
	//=======================================================================
	
	// Duration 설정
	// float EffectDuration = CachedItemData.ConsumableData.EffectDuration;
	// if (EffectDuration > 0.0f)
	// {
	// 	SpecHandle.Data->SetDuration(EffectDuration, false);
	// 	UE_LOG(LogConsumeAbility, Log, 
	// 		TEXT("Set effect duration: %.1f seconds"), EffectDuration);
	// }
	
	// Duration 확인
	// Duration 설정하지 않음, 버프형 아이템은 별도 GE 사용
	float EffectDuration = CachedItemData.ConsumableData.EffectDuration;
	if (EffectDuration > 0.0f)
	{
		UE_LOG(LogConsumeAbility, Warning,
			TEXT("아이템 ID %d의 EffectDuration=%.1f (버프형 아이템은 별도 GE 구현 필요)"),
			ItemID, EffectDuration);
	}
	
	UE_LOG(LogConsumeAbility, Log, 
		TEXT("Applying effect: ItemID=%d, Tag=%s, Value=%.1f, Duration=%.1f"), 
		ItemID,
		*CachedItemData.EffectTag_Consumable.ToString(),
		CachedItemData.EffectValue,
		EffectDuration);
	
	//=======================================================================
	// 특수 효과 처리: POISON_HEAL (→ 별도 GEC로 처리 변경)
	//=======================================================================
	// if (CachedItemData.EffectTag.MatchesTagExact(ItemTags::TAG_Item_Effect_POISON_HEAL))
	// {
	// 	if (ASC)
	// 	{
	// 		FGameplayTag PoisonTag = AbilityTags::TAG_State_Status_Poison;
	// 		
	// 		// 1. Loose 태그 제거
	// 		if (ASC->HasMatchingGameplayTag(PoisonTag))
	// 		{
	// 			ASC->RemoveLooseGameplayTag(PoisonTag);
	// 			UE_LOG(LogConsumeAbility, Log, TEXT("독 Loose 태그 제거"));
	// 		}
	// 		
	// 		// 2. 독 GE 제거 (나중에 독 시스템 구현 시)
	// 		FGameplayTagContainer PoisonTags;
	// 		PoisonTags.AddTag(PoisonTag);
	// 		ASC->RemoveActiveEffectsWithTags(PoisonTags);
	// 		
	// 		UE_LOG(LogConsumeAbility, Log, TEXT("독 상태 완전 제거 완료"));
	// 	}
	// }
	
	//=======================================================================
	// GameplayEffect 적용
	//=======================================================================
	// 캐릭터 자신에게 스펙대로 효과가 적용
	FActiveGameplayEffectHandle ActiveHandle
		= ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	
	if (!ActiveHandle.WasSuccessfullyApplied())
	{// GameplayEffect 적용에 실패한 경우
		UE_LOG(LogConsumeAbility, Error, TEXT("GameplayEffect 적용에 실패했습니다!"));
		return;
	}
	
	// 효과 적용 성공
	UE_LOG(LogConsumeAbility, Log, TEXT("아이템 ID %d의 스탯 회복 효과가 성공적으로 적용되었습니다."), ItemID);
}
#pragma endregion

#pragma region InventoryNotification
void UGA_ConsumeItem_Base::NotifyItemConsumed()
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
	
	UE_LOG(LogConsumeAbility, Log, 
		TEXT("Item.Consumed 이벤트 발송됨: 슬롯 인덱스=%d, 아이템 ID=%d"),  
		ConsumedSlotIndex, ItemID);
}
#pragma endregion

#pragma region Debug
void UGA_ConsumeItem_Base::LogItemInfo() const
{
	UE_LOG(LogConsumeAbility, Display, TEXT("======================================================================="));
	UE_LOG(LogConsumeAbility, Display, TEXT("Consume Item Info:"));
	UE_LOG(LogConsumeAbility, Display, TEXT("  ItemID: %d"), ItemID);
	UE_LOG(LogConsumeAbility, Display, TEXT("  Name: %s"), *CachedItemData.Name_EN.ToString());
	UE_LOG(LogConsumeAbility, Display, TEXT("  EffectTag: %s"), *CachedItemData.EffectTag_Consumable.ToString());
	UE_LOG(LogConsumeAbility, Display, TEXT("  EffectValue: %.1f"), CachedItemData.EffectValue);
	UE_LOG(LogConsumeAbility, Display, TEXT("  ConsumptionTime: %.1f"), CachedItemData.ConsumableData.ConsumptionTime);
	UE_LOG(LogConsumeAbility, Display, TEXT("  EffectDuration: %.1f"), CachedItemData.ConsumableData.EffectDuration);
	UE_LOG(LogConsumeAbility, Display, TEXT("======================================================================="));
}
#pragma endregion