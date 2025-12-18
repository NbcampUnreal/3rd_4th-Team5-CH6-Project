// GimmickZoneBase.cpp
#include "Gimmick/GimmickZoneBase.h"

#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffect.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"

// 로그 카테고리 정의 (이 파일 내에서만 사용)
DEFINE_LOG_CATEGORY_STATIC(LogGimmickZoneBase, Log, All);

AGimmickZoneBase::AGimmickZoneBase()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	bAlwaysRelevant = true; // 모든 클라이언트에게 항상 Relevant
	
	// Root Component
	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	SetRootComponent(RootComp);
	
	// Box Collision
	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	BoxCollision->SetupAttachment(RootComp);
	BoxCollision->SetBoxExtent(BoxExtent);
	BoxCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BoxCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	BoxCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	BoxCollision->SetGenerateOverlapEvents(true);
	
	// Sphere Collision
	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	SphereCollision->SetupAttachment(RootComp);
	SphereCollision->SetSphereRadius(SphereRadius);
	SphereCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 기본 비활성
	SphereCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	SphereCollision->SetGenerateOverlapEvents(true);
}

void AGimmickZoneBase::BeginPlay()
{
	Super::BeginPlay();
	
	// Collision Shape 초기화
	UpdateCollisionShape();
	
	// Server에서만 Overlap 이벤트 바인딩
	if (HasAuthority())
	{
		if (UShapeComponent* ActiveCollision = GetActiveCollisionComponent())
		{
			ActiveCollision->OnComponentBeginOverlap.AddDynamic(this, &AGimmickZoneBase::OnZoneBeginOverlap);
			ActiveCollision->OnComponentEndOverlap.AddDynamic(this, &AGimmickZoneBase::OnZoneEndOverlap);
		}
		
		// 주기적 체크가 필요한 경우 타이머 시작
		bool bNeedsPeriodicCheck = false;
		for (const FGimmickZoneEffectConfig& Config : ZoneEffects)
		{
			if (Config.bPeriodicCheckWhileInZone)
			{
				bNeedsPeriodicCheck = true;
				break;
			}
		}
		
		if (bNeedsPeriodicCheck)
		{
			StartPeriodicCheck();
		}
	}
}

void AGimmickZoneBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AGimmickZoneBase, bZoneEnabled);
}

#pragma region CollisionShapeManagement
//==========================================================================
// Collision Shape Management
//==========================================================================

void AGimmickZoneBase::UpdateCollisionShape()
{
	if (ZoneShapeType == EGimmickZoneShapeType::Box)
	{
		BoxCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		BoxCollision->SetBoxExtent(BoxExtent);
		BoxCollision->SetHiddenInGame(!bShowZoneVisualization);
		SphereCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		SphereCollision->SetHiddenInGame(true);
	}
	else
	{// Sphere
		SphereCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		SphereCollision->SetSphereRadius(SphereRadius);
		SphereCollision->SetHiddenInGame(!bShowZoneVisualization);
		BoxCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		BoxCollision->SetHiddenInGame(true);
	}
}

UShapeComponent* AGimmickZoneBase::GetActiveCollisionComponent() const
{
	return (ZoneShapeType == EGimmickZoneShapeType::Box)
		? static_cast<UShapeComponent*>(BoxCollision)
		: static_cast<UShapeComponent*>(SphereCollision);
}

#if WITH_EDITOR
void AGimmickZoneBase::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	
	FName PropertyName = PropertyChangedEvent.GetPropertyName();
	if (PropertyName == GET_MEMBER_NAME_CHECKED(AGimmickZoneBase, ZoneShapeType)
		|| PropertyName == GET_MEMBER_NAME_CHECKED(AGimmickZoneBase, BoxExtent)
		|| PropertyName == GET_MEMBER_NAME_CHECKED(AGimmickZoneBase, SphereRadius)
		|| PropertyName == GET_MEMBER_NAME_CHECKED(AGimmickZoneBase, bShowZoneVisualization))
	{
		UpdateCollisionShape();
	}
}
#endif
#pragma endregion
	
#pragma region OverlapEvents
//==========================================================================
// Overlap Events (Server Only)
//==========================================================================

void AGimmickZoneBase::OnZoneBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (!HasAuthority() || !bZoneEnabled || !OtherActor)
	{
		return;
	}
	
	// ASC 체크
	UAbilitySystemComponent* TargetASC = GetASCFromActor(OtherActor);
	if (!TargetASC)
	{
		return;
	}
	
	// 영역 내 Actor 목록에 추가
	ActorsInZone.Add(OtherActor);
	
	if (bShowDebugInfo)
	{
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
		UE_LOG(LogGimmickZoneBase, Warning, TEXT("[%s] 기믹 존 진입 감지 - 액터: %s"),
			*GetName(), *OtherActor->GetName());
#endif
	}
	
	ApplyZoneEffects(OtherActor);
}

void AGimmickZoneBase::OnZoneEndOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	if (!HasAuthority() || !OtherActor)
	{
		return;
	}
	
	UAbilitySystemComponent* TargetASC = GetASCFromActor(OtherActor);
	if (!TargetASC)
	{
		return;
	}
	
	// 영역 내 Actor 목록에서 제거
	ActorsInZone.Remove(OtherActor);
	
	if (bShowDebugInfo)
	{
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
		UE_LOG(LogGimmickZoneBase, Warning, TEXT("[%s] 기믹 존 이탈 감지 - 액터: %s"),
			*GetName(), *OtherActor->GetName());
#endif
	}
	
	RemoveZoneEffects(OtherActor);
}
#pragma endregion

#pragma region EffectApplicationLogic
//==========================================================================
// Effect Application Logic
//==========================================================================

void AGimmickZoneBase::ApplyZoneEffects(AActor* TargetActor)
{
	if (!TargetActor)
	{
		return;
	}
	
	UAbilitySystemComponent* TargetASC = GetASCFromActor(TargetActor);
	if (!TargetASC)
	{
		return;
	}
	
	for (const FGimmickZoneEffectConfig& Config : ZoneEffects)
	{
		if (!Config.GameplayEffectClass)
		{
			continue;
		}
		
		// 면역 체크
		if (IsImmuneToEffect(TargetASC, Config))
		{
			if (bShowDebugInfo)
			{
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
				UE_LOG(LogGimmickZoneBase, Log, TEXT("[%s] 대상이 해당 효과에 면역 상태입니다: %s"),
					*GetName(), *Config.GameplayEffectClass->GetName());
#endif
			}
			continue;
		}
		
		// OnEnter 재진입 쿨다운 체크
		if (Config.EffectType == EGimmickZoneEffectType::OnEnter)
		{
			if (!CanApplyOnEnterEffect(TargetActor, Config))
			{
				if (bShowDebugInfo)
				{
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
					UE_LOG(LogGimmickZoneBase, Log,
						TEXT("[%s] 진입(OnEnter) 효과가 아직 쿨타임이라 적용되지 않습니다"),
						*GetName());
#endif
				}
				continue;
			}
		}
		
		ApplySingleEffect(TargetASC, Config);
	}
}

void AGimmickZoneBase::RemoveZoneEffects(AActor* TargetActor)
{
	if (!TargetActor)
	{
		return;
	}
	
	UAbilitySystemComponent* TargetASC = GetASCFromActor(TargetActor);
	if (!TargetASC)
	{
		return;
	}
	
	// WhileInZone GE 제거 (영역 내에서만 유효)
	RemoveWhileInZoneEffects(TargetASC);
	
	// OnEnter 타입은 Duration이 있으므로 영역 퇴장 시 제거하지 않음
	// (면역 획득 시에만 PeriodicCheck에서 제거됨)
	//RemoveOnEnterEffects(TargetASC, TargetActor);
	
	// OnExit 타입 효과 적용
	for (const FGimmickZoneEffectConfig& Config : ZoneEffects)
	{
		if (Config.EffectType == EGimmickZoneEffectType::OnExit && Config.GameplayEffectClass)
		{
			ApplySingleEffect(TargetASC, Config);
		}
	}
	
	// WhileInZone 추적 데이터만 정리
	ActiveEffectHandles.Remove(TargetActor);
	
	// OnEnter 추적 데이터는 유지 (Duration 종료 시까지)
	// GE가 자동으로 만료되면 Handle이 무효화되므로 문제없음
	//OnEnterEffectHandles.Remove(TargetActor);
}

void AGimmickZoneBase::ApplySingleEffect(UAbilitySystemComponent* TargetASC, const FGimmickZoneEffectConfig& Config)
{
	if (!TargetASC || !Config.GameplayEffectClass)
	{
		return;
	}
	
	AActor* TargetActor = TargetASC->GetAvatarActor();
	if (!TargetActor)
	{
		return;
	}
	
	// *** OnEnter 타입일 때 기존 활성 GE가 있으면 먼저 제거 (중복 방지) ***
	if (Config.EffectType == EGimmickZoneEffectType::OnEnter)
	{
		TMap<TSubclassOf<UGameplayEffect>, FActiveGameplayEffectHandle>* HandleMap =
			OnEnterEffectHandles.Find(TargetActor);
		
		if (HandleMap)
		{
			FActiveGameplayEffectHandle* OldHandle = HandleMap->Find(Config.GameplayEffectClass);
			if (OldHandle && OldHandle->IsValid())
			{
				const FActiveGameplayEffect* ActiveGE = TargetASC->GetActiveGameplayEffect(*OldHandle);
				if (ActiveGE)
				{
					// 아직 활성 상태인 GE가 있으면 제거
					TargetASC->RemoveActiveGameplayEffect(*OldHandle);
					
					if (bShowDebugInfo)
					{
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
						UE_LOG(LogGimmickZoneBase, Warning,
							TEXT("[%s] 이전 활성 GE 제거 후 재적용: %s"),
							*GetName(), *Config.GameplayEffectClass->GetName());
#endif
					}
				}
				// 무효화된 Handle은 그냥 덮어씀
				HandleMap->Remove(Config.GameplayEffectClass);
			}
		}
	}
	
	FGameplayEffectContextHandle ContextHandle = TargetASC->MakeEffectContext();
	ContextHandle.AddSourceObject(this);
	
	FGameplayEffectSpecHandle SpecHandle = TargetASC->MakeOutgoingSpec(
		Config.GameplayEffectClass, 
		Config.EffectLevel, 
		ContextHandle
	);
	
	if (!SpecHandle.IsValid())
	{
		return;
	}
	
	FActiveGameplayEffectHandle ActiveHandle = TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	
	// WhileInZone 타입이면 Handle 저장 (나중에 제거용)
	if (Config.EffectType == EGimmickZoneEffectType::WhileInZone && ActiveHandle.IsValid())
	{
		ActiveEffectHandles.FindOrAdd(TargetActor).Add(ActiveHandle);
	}
	
	// OnEnter 타입일 때 Handle과 시간 모두 기록
	if (Config.EffectType == EGimmickZoneEffectType::OnEnter && ActiveHandle.IsValid())
	{
		// Handle 저장
		OnEnterEffectHandles.FindOrAdd(TargetActor).Add(Config.GameplayEffectClass, ActiveHandle);
		
		// 시간 저장 (로그용)
		LastEnterTimes.FindOrAdd(TargetActor).Add(
			Config.GameplayEffectClass, GetWorld()->GetTimeSeconds());
	}
	
	if (bShowDebugInfo)
	{
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
		UE_LOG(LogGimmickZoneBase, Log, TEXT("[%s] GE 적용 완료: %s (타입: %d)"),
			*GetName(),
			*Config.GameplayEffectClass->GetName(),
			(int32)Config.EffectType);
#endif
	}
}

void AGimmickZoneBase::RemoveWhileInZoneEffects(UAbilitySystemComponent* TargetASC)
{
	if (!TargetASC)
	{
		return;
	}
	
	AActor* TargetActor = TargetASC->GetAvatarActor();
	if (!TargetActor)
	{
		return;
	}
	
	TArray<FActiveGameplayEffectHandle>* Handles = ActiveEffectHandles.Find(TargetActor);
	if (!Handles)
	{
		return;
	}
	
	for (const FActiveGameplayEffectHandle& Handle : *Handles)
	{
		if (Handle.IsValid())
		{
			TargetASC->RemoveActiveGameplayEffect(Handle);
			
			if (bShowDebugInfo)
			{
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
				UE_LOG(LogGimmickZoneBase, Log, TEXT("[%s] WhileInZone GE 제거: %s"),
					*GetName(), *TargetActor->GetName());
#endif
			}
		}
	}
}

void AGimmickZoneBase::RemoveOnEnterEffects(UAbilitySystemComponent* TargetASC, AActor* TargetActor)
{
	if (!TargetASC || !TargetActor)
	{
		return;
	}
	
	TMap<TSubclassOf<UGameplayEffect>, FActiveGameplayEffectHandle>* HandleMap =
		OnEnterEffectHandles.Find(TargetActor);
	
	if (!HandleMap)
	{
		return;
	}
	
	for (auto& Pair : *HandleMap)
	{
		if (Pair.Value.IsValid())
		{
			TargetASC->RemoveActiveGameplayEffect(Pair.Value);
			
			if (bShowDebugInfo)
			{
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
				UE_LOG(LogGimmickZoneBase, Log, TEXT("[%s] OnEnter GE 제거: %s"),
					*GetName(), *TargetActor->GetName());
#endif
			}
		}
	}
}
#pragma endregion

#pragma region Immunity&CooldownChecks
//==========================================================================
// Immunity & Cooldown Checks
//==========================================================================

bool AGimmickZoneBase::IsImmuneToEffect(
	UAbilitySystemComponent* TargetASC,
	const FGimmickZoneEffectConfig& Config) const
{
	if (!TargetASC || Config.ImmunityTags.IsEmpty())
	{
		return false;
	}
	
	// ImmunityTags 중 하나라도 있으면 면역
	return TargetASC->HasAnyMatchingGameplayTags(Config.ImmunityTags);
}

bool AGimmickZoneBase::CanApplyOnEnterEffect(
	AActor* TargetActor,
	const FGimmickZoneEffectConfig& Config) const
{
	// 주기적 체크가 활성화된 경우, 쿨다운 무시 (Duration 종료 감지를 위해)
	if (Config.bPeriodicCheckWhileInZone)
	{
		return true;
	}
	
	if (Config.ReEnterCooldown <= 0.0f)
	{// 쿨다운 없으면 항상 적용 가능
		return true;
	}
	
	const TMap<TSubclassOf<UGameplayEffect>, float>* ActorEnterTimes = LastEnterTimes.Find(TargetActor);
	if (!ActorEnterTimes)
	{// 첫 진입
		return true;
	}
	
	const float* LastTime = ActorEnterTimes->Find(Config.GameplayEffectClass);
	if (!LastTime)
	{
		return true;
	}
	
	float CurrentTime = GetWorld()->GetTimeSeconds();
	return (CurrentTime - *LastTime) >= Config.ReEnterCooldown;
}
#pragma endregion

#pragma region Helpers
//==========================================================================
// Helpers
//==========================================================================

UAbilitySystemComponent* AGimmickZoneBase::GetASCFromActor(AActor* Actor) const
{
	if (!Actor)
	{
		return nullptr;
	}
	
	// IAbilitySystemInterface 구현 체크
	if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(Actor))
	{
		return ASI->GetAbilitySystemComponent();
	}
	
	return nullptr;
}

void AGimmickZoneBase::PeriodicCheck()
{
	if (!HasAuthority() || !bZoneEnabled)
	{
		return;
	}
	
	// 영역 내 모든 Actor 체크
	TArray<AActor*> ActorsToCheck = ActorsInZone.Array();
	
	for (AActor* Actor : ActorsToCheck)
	{
		if (!Actor || !IsValid(Actor))
		{
			ActorsInZone.Remove(Actor);
			continue;
		}
		
		UAbilitySystemComponent* TargetASC = GetASCFromActor(Actor);
		if (!TargetASC)
		{
			continue;
		}
		
		// OnEnter + bPeriodicCheckWhileInZone 효과만 체크
		for (const FGimmickZoneEffectConfig& Config : ZoneEffects)
		{
			if (Config.EffectType != EGimmickZoneEffectType::OnEnter || !Config.bPeriodicCheckWhileInZone)
			{
				continue;
			}
			
			if (!Config.GameplayEffectClass)
			{
				continue;
			}
			
			// 면역 체크
			bool bIsImmune = IsImmuneToEffect(TargetASC, Config);
			
			// 기존 Handle 확인
			TMap<TSubclassOf<UGameplayEffect>, FActiveGameplayEffectHandle>* HandleMap =
				OnEnterEffectHandles.Find(Actor);
			
			FActiveGameplayEffectHandle* ExistingHandle = HandleMap ? HandleMap->Find(Config.GameplayEffectClass) : nullptr;
			
			// 면역 상태가 되었으면 기존 GE 제거
			if (bIsImmune)
			{
				if (ExistingHandle && ExistingHandle->IsValid())
				{
					TargetASC->RemoveActiveGameplayEffect(*ExistingHandle);
					HandleMap->Remove(Config.GameplayEffectClass);
					
					if (bShowDebugInfo)
					{
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
						UE_LOG(LogGimmickZoneBase, Log,
							TEXT("[%s] 면역으로 인해 GE 제거: %s (대상: %s)"),
							*GetName(), *Config.GameplayEffectClass->GetName(), *Actor->GetName());
#endif
					}
				}
				continue; // 면역이면 재적용하지 않음
			}
		
			// *** 핵심 수정: ASC에서 실제로 GE가 활성 상태인지 확인 ***
			bool bGEIsActive = false;
		
			if (ExistingHandle && ExistingHandle->IsValid())
			{
				// ASC에서 해당 Handle의 GE가 실제로 존재하는지 확인
				const FActiveGameplayEffect* ActiveGE = TargetASC->GetActiveGameplayEffect(*ExistingHandle);
				bGEIsActive = (ActiveGE != nullptr);
				
				if (!bGEIsActive)
				{
					// Handle은 유효하지만 GE가 실제로는 제거됨 (Duration 종료)
					HandleMap->Remove(Config.GameplayEffectClass);
					
					if (bShowDebugInfo)
					{
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
						UE_LOG(LogGimmickZoneBase, Log,
							TEXT("[%s] GE 만료 (지속시간 종료): %s"),
							*GetName(), *Config.GameplayEffectClass->GetName());
#endif
					}
				}
			}
			
			// GE가 활성 상태가 아니면 재적용
			if (!bGEIsActive)
			{
				ApplySingleEffect(TargetASC, Config);
				
				if (bShowDebugInfo)
				{
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
					UE_LOG(LogGimmickZoneBase, Log,
						TEXT("[%s] 주기적 재적용: %s → %s"),
						*GetName(), *Config.GameplayEffectClass->GetName(), *Actor->GetName());
#endif
				}
			}
		}
	}
	
	// 영역 밖에 있지만 OnEnter GE가 활성 상태인 Actor들도 면역 체크
	CheckImmuneActorsOutsideZone();
}

void AGimmickZoneBase::StartPeriodicCheck()
{
	if (!HasAuthority())
	{
		return;
	}
	
	// 가장 짧은 체크 간격 찾기
	float MinInterval = 1.0f;
	for (const FGimmickZoneEffectConfig& Config : ZoneEffects)
	{
		if (Config.bPeriodicCheckWhileInZone)
		{
			MinInterval = FMath::Min(MinInterval, Config.PeriodicCheckInterval);
		}
	}
	
	GetWorldTimerManager().SetTimer(
		PeriodicCheckTimerHandle,
		this,
		&AGimmickZoneBase::PeriodicCheck,
		MinInterval,
		true // 반복
	);
	
	if (bShowDebugInfo)
	{
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
		UE_LOG(LogGimmickZoneBase, Log, TEXT("[%s] Periodic 체크 시작 - Interval: %.2fs"),
			*GetName(), MinInterval);
#endif
	}
}

void AGimmickZoneBase::StopPeriodicCheck()
{
	if (PeriodicCheckTimerHandle.IsValid())
	{
		GetWorldTimerManager().ClearTimer(PeriodicCheckTimerHandle);
	}
}

void AGimmickZoneBase::CheckImmuneActorsOutsideZone()
{
	if (!HasAuthority())
	{
		return;
	}
	
	TArray<AActor*> ActorsToCheck;
	OnEnterEffectHandles.GetKeys(ActorsToCheck);
	
	for (AActor* Actor : ActorsToCheck)
	{
		// 영역 내에 있으면 PeriodicCheck에서 이미 처리됨
		if (ActorsInZone.Contains(Actor))
		{
			continue;
		}
		
		if (!Actor || !IsValid(Actor))
		{
			OnEnterEffectHandles.Remove(Actor);
			LastEnterTimes.Remove(Actor);
			continue;
		}
		
		UAbilitySystemComponent* TargetASC = GetASCFromActor(Actor);
		if (!TargetASC)
		{
			continue;
		}
		
		TMap<TSubclassOf<UGameplayEffect>, FActiveGameplayEffectHandle>* HandleMap =
			OnEnterEffectHandles.Find(Actor);
		
		if (!HandleMap)
		{
			continue;
		}
		
		// 이 영역의 각 Config에 대해 면역 체크
		TArray<TSubclassOf<UGameplayEffect>> GEClassesToRemove;
		
		for (const FGimmickZoneEffectConfig& Config : ZoneEffects)
		{
			if (Config.EffectType != EGimmickZoneEffectType::OnEnter || !Config.bPeriodicCheckWhileInZone)
			{
				continue;
			}
			
			if (!Config.GameplayEffectClass)
			{
				continue;
			}
			
			FActiveGameplayEffectHandle* ExistingHandle = HandleMap->Find(Config.GameplayEffectClass);
			if (!ExistingHandle || !ExistingHandle->IsValid())
			{
				continue;
			}
			
			// 면역 상태면 GE 제거
			if (IsImmuneToEffect(TargetASC, Config))
			{
				TargetASC->RemoveActiveGameplayEffect(*ExistingHandle);
				GEClassesToRemove.Add(Config.GameplayEffectClass);
				
				if (bShowDebugInfo)
				{
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
					UE_LOG(LogGimmickZoneBase, Log,
						TEXT("[%s] 면역으로 인해 GE 제거 (zone 외부): %s (출처: %s)"),
						*GetName(), *Config.GameplayEffectClass->GetName(), *Actor->GetName());
#endif
				}
			}
		}
		
		// 제거된 GE들을 HandleMap에서 삭제
		for (TSubclassOf<UGameplayEffect> GEClass : GEClassesToRemove)
		{
			HandleMap->Remove(GEClass);
		}
		
		// HandleMap이 비었으면 Actor 자체를 제거
		if (HandleMap->Num() == 0)
		{
			OnEnterEffectHandles.Remove(Actor);
			LastEnterTimes.Remove(Actor);
		}
	}
}
#pragma endregion

#pragma region BlueprintInterface
//==========================================================================
// Blueprint Interface
//==========================================================================

void AGimmickZoneBase::SetZoneEnabled(bool bEnabled)
{
	if (!HasAuthority())
	{
		return;
	}
	
	bZoneEnabled = bEnabled;
	
	// 비활성화 시 모든 활성 효과 제거
	if (!bZoneEnabled)
	{
		TArray<AActor*> ActorsToRemove;
		ActiveEffectHandles.GetKeys(ActorsToRemove);
		
		for (AActor* Actor : ActorsToRemove)
		{
			RemoveZoneEffects(Actor);
		}
	}
}
#pragma endregion