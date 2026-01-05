// TSCharacter.cpp
#include "Character/TSCharacter.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "PlayerState/TSPlayerState.h"
#include "GAS/AbilityManager/TSAbilityManagerSubSystem.h"
#include "GameplayTagContainer.h"
#include "DataAsset/TSPlayerInputDataAsset.h"
#include "AbilitySystemComponent.h"
#include "Controller/TSPlayerController.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "GAS/AttributeSet/TSAttributeSet.h"
#include "Item/Interface/IInteraction.h"
#include "GameplayTags/AbilityGameplayTags.h"
#include "Net/UnrealNetwork.h"
#include "Building/TSBuildingComponent.h"
#include "GAS/InteractTag.h"
#include "Inventory/TSInventoryMasterComponent.h"
#include "System/ResourceControl/TSResourceItemInterface.h"
#include "Components/CapsuleComponent.h"
#include "Components/TextBlock.h"
#include "Components/WidgetComponent.h"
#include "Sound/Footstep/FootstepComponent.h"
#include "UI/TSPlayerUIDataControllerSystem.h"
#include "GameState/TSGameState.h"
#include "PingSystem/TSPingActor.h"
#include "Sound/Hit/HitComponent.h"
#include "System/Erosion/ErosionLightSourceSubActor.h"
#include "PingSystem/TSPingTypes.h"
#include "System/ResourceControl/TSResourceBaseActor.h"

// 로그 카테고리 정의 (이 파일 내에서만 사용)
DEFINE_LOG_CATEGORY_STATIC(LogCharacter, Log, All);

ATSCharacter::ATSCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm Component"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->TargetArmLength = 100.f;
	SpringArmComponent->bUsePawnControlRotation = true;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(SpringArmComponent);
	CameraComponent->bUsePawnControlRotation = false;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	
	// 컴포넌트 생성
	InventoryMasterComponent = CreateDefaultSubobject<UTSInventoryMasterComponent>(TEXT("InventoryComponent"));
	BuildingComponent = CreateDefaultSubobject<UTSBuildingComponent>(TEXT("BuildingComponent"));
	FootstepComponent = CreateDefaultSubobject<UFootstepComponent>(TEXT("FootstepComponent"));
	HitComponent = CreateDefaultSubobject<UHitComponent>(TEXT("HitComponent"));
	PlayerNameComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("PlayerNameComponent"));
	PlayerNameComponent->SetupAttachment(GetMesh());
	
	// 상호작용 위젯
	InteractionWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractionWidget"));
	InteractionWidget->SetupAttachment(RootComponent);
	InteractionWidget->SetWidgetSpace(EWidgetSpace::Screen);
	InteractionWidget->SetDrawSize(FVector2D(300.f, 60.f));
	InteractionWidget->SetVisibility(false);
	InteractionWidget->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

UAbilitySystemComponent* ATSCharacter::GetAbilitySystemComponent() const
{
	return ASC;
}

UTSAttributeSet* ATSCharacter::GetAttributeSet() const
{
	return Attributes;
}

void ATSCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	InitAbilitySystem();
	InitializeAbilities();
	if (!ASC)
	{
		return;
	}
	
	if (GetPlayerState())
	{
		ShowPlayerName(GetPlayerState()->GetPlayerName());
	}
	
	if (StaminaIncreaseEffectClass)
	{
		FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
		ContextHandle.AddSourceObject(this);
		FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(StaminaIncreaseEffectClass, 1, ContextHandle);
            
		if (SpecHandle.IsValid())
		{
			ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}
	
	if (ThirstDecayEffectClass)
	{
		FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
		ContextHandle.AddSourceObject(this);
		FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(ThirstDecayEffectClass, 1, ContextHandle);
            
		if (SpecHandle.IsValid())
		{
			ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}
	
	if (HungerDecayEffectClass)
	{
		FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
		ContextHandle.AddSourceObject(this);
		FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(HungerDecayEffectClass, 1, ContextHandle);
            
		if (SpecHandle.IsValid())
		{
			ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}
	
	if (HungerSpeedEffectClass)
	{
		FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
		ContextHandle.AddSourceObject(this);
		FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(HungerSpeedEffectClass, 1, ContextHandle);
            
		if (SpecHandle.IsValid())
		{
			ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}
	
	if (HungerHealthEffectClass)
	{
		FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
		ContextHandle.AddSourceObject(this);
		FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(HungerHealthEffectClass, 1, ContextHandle);
            
		if (SpecHandle.IsValid())
		{
			ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}
	if (TempHotEffectClass)
	{
		FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
		ContextHandle.AddSourceObject(this);
		FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(TempHotEffectClass, 1, ContextHandle);
            
		if (SpecHandle.IsValid())
		{
			ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}
	if (TempColdEffectClass)
	{
		FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
		ContextHandle.AddSourceObject(this);
		FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(TempColdEffectClass, 1, ContextHandle);
            
		if (SpecHandle.IsValid())
		{
			ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}
	if (LightEffectClass)
	{
		FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
		ContextHandle.AddSourceObject(this);
		FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(LightEffectClass, 1, ContextHandle);
            
		if (SpecHandle.IsValid())
		{
			ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}
	if (DarkEffectClass)
	{
		FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
		ContextHandle.AddSourceObject(this);
		FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(DarkEffectClass, 1, ContextHandle);
            
		if (SpecHandle.IsValid())
		{
			ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}
	
	// 빛 탐지를 위한 1초 타이머 시작
	GetWorld()->GetTimerManager().SetTimer(LightCheckTimerHandle,this,&ATSCharacter::CheckInLightSource,1.0f,true);
}

void ATSCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	InitAbilitySystem();

	if (GetPlayerState())
	{
		ShowPlayerName(GetPlayerState()->GetPlayerName());
	}
}

void ATSCharacter::InitAbilitySystem()
{
	ATSPlayerState* PS = GetPlayerState<ATSPlayerState>();
	if (!PS)
	{
		UE_LOG(LogTemp, Warning, TEXT("ATSPlayerState is NULL."));
		return;
	}

	ASC = PS->GetAbilitySystemComponent();
	if (!ASC)
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerState ASC is NULL."));
		return;
	}

	Attributes = PS->GetAttributeSet();
	if (!Attributes)
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerState Attributes is NULL."));
		return;
	}
	ASC->InitAbilityActorInfo(PS, this);

	ASC->GetGameplayAttributeValueChangeDelegate(UTSAttributeSet::GetMoveSpeedAttribute()).AddUObject(this, &ATSCharacter::OnMoveSpeedChanged);
	if (UCharacterMovementComponent* MoveComponent = GetCharacterMovement())
	{
		MoveComponent->MaxWalkSpeed = Attributes->GetMoveSpeed();
	}

	if (IsValid(ASC) && IsValid(Attributes))
	{
		ASC->GetGameplayAttributeValueChangeDelegate(
			UTSAttributeSet::GetMoveSpeedAttribute()
		).AddUObject(this, &ATSCharacter::OnMoveSpeedChanged);
		
		ASC->AddLooseGameplayTag(AbilityTags::TAG_Character_Type_Player);
		
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
		// ■ 추적: Hunger 변경 추적 (서버에서만 로그)
		ASC->GetGameplayAttributeValueChangeDelegate(
			UTSAttributeSet::GetHungerAttribute()
		).AddLambda([this](const FOnAttributeChangeData& Data)
		{
			if (HasAuthority())
			{
				UE_LOG(LogCharacter, Warning, TEXT("[서버] Hunger 변경: %.2f -> %.2f"), Data.OldValue, Data.NewValue);
			}
		});
		
		// ■ 추적: Thirst 변경 추적 (서버에서만 로그)
		ASC->GetGameplayAttributeValueChangeDelegate(
			UTSAttributeSet::GetThirstAttribute()
		).AddLambda([this](const FOnAttributeChangeData& Data)
		{
			if (HasAuthority())
			{
				UE_LOG(LogCharacter, Warning, TEXT("[서버] Thirst 변경: %.2f -> %.2f"), Data.OldValue, Data.NewValue);
			}
		});
		
		// ■ 추적: Health 변경 추적 (서버에서만 로그)
		ASC->GetGameplayAttributeValueChangeDelegate(
			UTSAttributeSet::GetHealthAttribute()
		).AddLambda([this](const FOnAttributeChangeData& Data)
		{
			if (HasAuthority())
			{
				UE_LOG(LogCharacter, Warning, TEXT("[서버] Health 변경: %.2f -> %.2f"), Data.OldValue, Data.NewValue);
			}
		});
		
		// ■ 추적: Sanity 변경 추적 (서버에서만 로그)
		ASC->GetGameplayAttributeValueChangeDelegate(
			UTSAttributeSet::GetSanityAttribute()
		).AddLambda([this](const FOnAttributeChangeData& Data)
		{
			if (HasAuthority())
			{
				UE_LOG(LogCharacter, Warning, TEXT("[서버] Sanity 변경: %.2f -> %.2f"), Data.OldValue, Data.NewValue);
			}
		});
		
		// ■ 추적: Temperature 변경 추적 (서버에서만 로그)
		ASC->GetGameplayAttributeValueChangeDelegate(
			UTSAttributeSet::GetTemperatureAttribute()
		).AddLambda([this](const FOnAttributeChangeData& Data)
		{
			if (HasAuthority())
			{
				UE_LOG(LogCharacter, Warning, TEXT("[서버] Temperature 변경: %.2f -> %.2f"), Data.OldValue, Data.NewValue);
			}
		});
		
		// ■ 추적: State.Status.Immune.Poison 태그 변화 추적
		ASC->RegisterGameplayTagEvent(
			AbilityTags::TAG_State_Status_Immune_Poison, 
			EGameplayTagEventType::NewOrRemoved
		).AddLambda([this](const FGameplayTag CallbackTag, int32 NewCount)
		{
			// NewCount > 0 : 태그가 적용됨
			// NewCount == 0 : 태그가 제거됨
			if (NewCount > 0)
			{
				UE_LOG(LogCharacter, Warning, TEXT("태그 적용: %s"), *CallbackTag.ToString());
			}
			else
			{
				UE_LOG(LogCharacter, Warning, TEXT("태그 제거: %s"), *CallbackTag.ToString());
			}
		});
		
		// ■ 추적: State.Status.Poison 태그 변화 추적
		ASC->RegisterGameplayTagEvent(
			AbilityTags::TAG_State_Status_Poison,
			EGameplayTagEventType::NewOrRemoved
		).AddLambda([this](const FGameplayTag CallbackTag, int32 NewCount)
		{
			if (NewCount > 0)
			{
				UE_LOG(LogCharacter, Warning, TEXT("태그 적용: %s"), *CallbackTag.ToString());
			}
			else
			{
				UE_LOG(LogCharacter, Warning, TEXT("태그 제거: %s"), *CallbackTag.ToString());
			}
		});
		
		// UI 컨트롤러 모델 시스템 초기화 요청		
		UTSPlayerUIDataControllerSystem* PUICS = UTSPlayerUIDataControllerSystem::Get(this);
		if (!IsValid(PUICS)) return;
		if (!IsValid(GetController())) return;
		APlayerController* PlayerController = Cast<APlayerController>(GetController());
		PUICS->InitControllerModel(PlayerController, ASC);
		PUICS->InitViewModel();
		
#endif
	}
}

void ATSCharacter::InitializeAbilities()
{
	//매니저를 통해 어빌리티 부여 하는 함수
	if (!HasAuthority() || !ASC)
	{
		return;
	}
	UTSAbilityManagerSubSystem* Manager = UTSAbilityManagerSubSystem::GetAbilityManager(this);
	if (!Manager)
	{
		return;
	}
	// 람다함수:태그 이름 받아서 어빌리티 부여하도록
	auto GiveByTag = [Manager, this](const FGameplayTag& Tag)
	{
		if (Tag.IsValid())
		{
			Manager->GiveAbilityByTag(ASC, Tag);
		}
	};
	
	// MOVE
	GiveByTag(AbilityTags::TAG_Ability_Move_JumpOrClimb.GetTag());
	GiveByTag(AbilityTags::TAG_Ability_Move_Roll.GetTag());
	GiveByTag(AbilityTags::TAG_Ability_Move_Sprint.GetTag());
	GiveByTag(AbilityTags::TAG_Ability_Move_Crouch.GetTag());

	// Interact
	GiveByTag(AbilityTags::TAG_Ability_Interact_Build.GetTag());
	GiveByTag(AbilityTags::TAG_Ability_Interact_Interact.GetTag());
	GiveByTag(AbilityTags::TAG_Ability_Interact_Ping.GetTag());
	GiveByTag(AbilityTags::TAG_Ability_Interact_Emote.GetTag());
	GiveByTag(AbilityTags::TAG_Ability_Interact_LeftClick.GetTag());
	GiveByTag(AbilityTags::TAG_Ability_Interact_RightClick.GetTag());

	// HotKey
	GiveByTag(AbilityTags::TAG_Ability_HotKey.GetTag());
	
	// 자원
	GiveByTag(InteractTag::INTERACTTAG_RESOURCE_STARTINTERACT.GetTag());
	
	GiveByTag(AbilityTags::TAG_Ability_HotKey.GetTag());
}

void ATSCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ATSCharacter, AnimType);
	DOREPLIFETIME(ATSCharacter, ReviveTargetCharacter);
	DOREPLIFETIME(ATSCharacter, bIsRescuing);
	DOREPLIFETIME(ATSCharacter, bIsDownedState);
	DOREPLIFETIME(ATSCharacter, bIsDeadState);
	DOREPLIFETIME(ATSCharacter, bIsClimbState);
	DOREPLIFETIME(ATSCharacter, bIsCrouching);
}

void ATSCharacter::BecomeDowned()
{
	if (!ASC)
	{
		return;
	}
	if (ASC)
	{
		if (DownedTagEffectClass)
		{
			FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
			ContextHandle.AddSourceObject(this);
			FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(DownedTagEffectClass, 1, ContextHandle);
            
			if (SpecHandle.IsValid())
			{
				ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			}
		}
	}
	if (ProneMoveSpeedEffectClass)
	{
		FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
		ContextHandle.AddSourceObject(this);
		FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(ProneMoveSpeedEffectClass, 1, ContextHandle);
            
		if (SpecHandle.IsValid())
		{
			ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}
	bIsDownedState = true;
	
	if (HasAuthority())
	{
		OnRep_IsDownedState();
		if (ATSGameState* GS = GetWorld()->GetGameState<ATSGameState>())
		{
			GS->CheckGameOver();
			GS->DecreaseSanityToAll(false);
		}
	}
}

bool ATSCharacter::IsDowned() const
{
	if (ASC)
	{
		return ASC->HasMatchingGameplayTag(AbilityTags::TAG_State_Status_Downed);
	}
	return false;
}

void ATSCharacter::OnRep_IsDownedState()
{
	if (IsLocallyControlled())
	{
		if (ATSPlayerController* TSController = Cast<ATSPlayerController>(Controller))
		{
			if (bIsDownedState)
			{
				TSController->ShowDownedUI();
			} else
			{
				TSController->HideDownedUI();
			}
		}
	}
}

void ATSCharacter::Die()
{
	if (bIsDeadState)
	{
		return;
	}
	if (ASC)
	{
		ASC->RemoveActiveEffectsWithGrantedTags(FGameplayTagContainer(AbilityTags::TAG_State_Status_Downed));
		if (DeadTagEffectClass)
		{
			FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
			ContextHandle.AddSourceObject(this);
			FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(DeadTagEffectClass, 1, ContextHandle);
            
			if (SpecHandle.IsValid())
			{
				ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			}
		}
	}
	bIsDownedState = false;
	bIsDeadState = true;
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		DisableInput(PC);
	}
	OnRep_IsDeadState();
	
	if (HasAuthority())
	{
		OnRep_IsDownedState();
		if (ATSGameState* GS = GetWorld()->GetGameState<ATSGameState>())
		{
			GS->CheckGameOver();
			GS->DecreaseSanityToAll(true);
		}
	}
}

bool ATSCharacter::IsDead() const
{
	if (ASC)
	{
		return ASC->HasMatchingGameplayTag(AbilityTags::TAG_State_Status_Dead);
	}
	return false;
}

void ATSCharacter::OnRep_IsDeadState()
{
	if (bIsDeadState)
	{
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
		GetMesh()->SetSimulatePhysics(true);
		if (GetCharacterMovement())
		{
			GetCharacterMovement()->StopMovementImmediately();
			GetCharacterMovement()->DisableMovement();
		}
	}
}

ATSCharacter* ATSCharacter::DetectReviveTarget()
{
	APlayerController* PC = Cast<APlayerController>(Controller);
	if (!IsValid(PC) || !PC->IsLocalController()) return nullptr;
	if (!IsValid(GEngine) || !IsValid(GEngine->GameViewport)) return nullptr;
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport) GEngine->GameViewport->GetViewportSize(ViewportSize);
	FVector2D Center = ViewportSize / 2.f;
	
	FVector TraceStart, Forward;
	UGameplayStatics::DeprojectScreenToWorld(PC, Center, TraceStart, Forward);
    
	FVector TraceEnd = TraceStart + Forward * 300.0f;
	FCollisionShape Shape = FCollisionShape::MakeSphere(40.0f);
	FCollisionObjectQueryParams ObjectParams;
	ObjectParams.AddObjectTypesToQuery(ECC_Pawn); 

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	FHitResult HitResult;
	bool bHit = GetWorld()->SweepSingleByObjectType(HitResult, TraceStart, TraceEnd, FQuat::Identity, ObjectParams, Shape, Params);

	if (bHit)
	{
		return Cast<ATSCharacter>(HitResult.GetActor());
	}
	return nullptr;
}

void ATSCharacter::Revive()
{
	if (IsDead())
	{
		return;
	}
	if (ASC)
	{
		ASC->RemoveActiveEffectsWithGrantedTags(FGameplayTagContainer(AbilityTags::TAG_State_Status_Downed));
	}
	if (UTSAttributeSet* AS = GetAttributeSet())
	{
		AS->SetHealth(50.0f);
	}
	bIsDownedState = false;
	
	if (HasAuthority())
	{
		OnRep_IsDownedState();
	}
}

bool ATSCharacter::IsRescueCharacter() const
{
	if (ASC)
	{
		return ASC->HasMatchingGameplayTag(AbilityTags::TAG_State_Status_Rescuing);
	}
	return false;
}

void ATSCharacter::ServerStartRevive_Implementation(ATSCharacter* Target)
{
	if (bIsRescuing)
	{
		return;
	}
	if (IsDowned() || IsDead())
	{
		return;
	}
	if (!IsValid(Target)) 
	{
		return;
	}
	float Distance = FVector::Dist(GetActorLocation(), Target->GetActorLocation());
	if (Distance > MaxReviveDistance)
	{
		return;
	}
	bool bTargetDown = Target->IsDowned();
	bool bTargetDead = Target->IsDead();

	if (!bTargetDown || bTargetDead)
	{
		return;
	}
	if (GetWorldTimerManager().IsTimerActive(ReviveTimerHandle))
	{
		GetWorldTimerManager().ClearTimer(ReviveTimerHandle);
	}
	ReviveTargetCharacter = Target;
	CurrentReviveTime = 0.f;
	bIsRescuing = true;
	if (ASC)
	{
		if (RescuingTagEffectClass)
		{
			FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
			ContextHandle.AddSourceObject(this);
			FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(RescuingTagEffectClass, 1, ContextHandle);
            
			if (SpecHandle.IsValid())
			{
				ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			}
		}
	}
	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->SetMovementMode(MOVE_None);
	}
	GetWorldTimerManager().SetTimer(ReviveTimerHandle, this, &ATSCharacter::OnReviveFinished, ReviveDuration, false);
}

void ATSCharacter::ServerStopRevive_Implementation()
{
	if (!bIsRescuing)
	{
		return;
	}
	GetWorldTimerManager().ClearTimer(ReviveTimerHandle);
	CurrentReviveTime = 0.f;
	ReviveTargetCharacter = nullptr;
	bIsRescuing = false;
	if (ASC)
	{
		ASC->RemoveActiveEffectsWithGrantedTags(FGameplayTagContainer(AbilityTags::TAG_State_Status_Rescuing));
	}
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	ClientForceStopRevive();
}

void ATSCharacter::ClientForceStopRevive_Implementation()
{
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
}

void ATSCharacter::OnReviveFinished()
{
	GetWorldTimerManager().ClearTimer(ReviveTimerHandle);
	CurrentReviveTime = 0.f;
	ATSCharacter* Target = ReviveTargetCharacter;
	ReviveTargetCharacter = nullptr;
	if (ASC)
	{
		ASC->RemoveActiveEffectsWithGrantedTags(FGameplayTagContainer(AbilityTags::TAG_State_Status_Rescuing));
	}
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	bIsRescuing = false;
	if (!IsValid(Target))
	{
		return;
	}
	if (!Target->IsDowned() || Target->IsDead())
	{
		return;
	}
	Target->Revive();
}

void ATSCharacter::TickReviveValidation()
{
	if (!HasAuthority())
	{
		return;
	}
	if (!IsValid(ReviveTargetCharacter))
	{
		ServerStopRevive();
		return;
	}
	if (ReviveTargetCharacter->IsDead() || !ReviveTargetCharacter->IsDowned())
	{
		ServerStopRevive();
		return;
	}
	float DistSq = FVector::DistSquared(GetActorLocation(), ReviveTargetCharacter->GetActorLocation());
	float MaxDistSq = MaxReviveDistance * MaxReviveDistance;
	if (DistSq > MaxDistSq)
	{
		ServerStopRevive();
		return;
	}
	if (ASC)
	{
		bool bHasMatchingDownedOrDeadTags = ASC->HasMatchingGameplayTag(AbilityTags::TAG_State_Status_Downed) || ASC->HasMatchingGameplayTag(AbilityTags::TAG_State_Status_Dead);
		if (bHasMatchingDownedOrDeadTags)
		{
			ServerStopRevive();
			return;
		}
	}
}

void ATSCharacter::OnRep_IsRescuing()
{
	if (bIsRescuing )
	{
		GetCharacterMovement()->SetMovementMode(MOVE_None);
	} else
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}
}

void ATSCharacter::CheckInLightSource()
{
	TArray<AActor*> LightActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(),AErosionLightSourceSubActor::StaticClass(), LightActors);
	const FGameplayTag InLightTag = AbilityTags::TAG_State_Status_InLightSourceRange;
	if (LightActors.Num() == 0)
	{
		if (ASC->HasMatchingGameplayTag(InLightTag))
		{
			ASC->RemoveActiveEffectsWithGrantedTags(FGameplayTagContainer(InLightTag));
		}
		return;
	}
	
	float NearestDistance = 0.0f;
	
	AActor* NearestActor = UGameplayStatics::FindNearestActor(GetActorLocation(),LightActors,NearestDistance);
	
	AErosionLightSourceSubActor* NearestLight = Cast<AErosionLightSourceSubActor>(NearestActor);
	
	const float LightRange = 5000.0f;
	
	if (NearestLight && NearestDistance <= LightRange)
	{
		if (!ASC -> HasMatchingGameplayTag(InLightTag))
		{
			if (InLightTagEffectClass)
			{
				FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
				ContextHandle.AddSourceObject(this);
				FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(InLightTagEffectClass, 1, ContextHandle);
            
				if (SpecHandle.IsValid())
				{
					ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
				}
			}
		}
	} else
	{
		if (ASC->HasMatchingGameplayTag(InLightTag))
		{
			ASC->RemoveActiveEffectsWithGrantedTags(FGameplayTagContainer(InLightTag));
		}
	}
}

void ATSCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (SpringArmComponent)
	{
		FVector StartLocation = SpringArmComponent->GetRelativeLocation();
		StartLocation.Y = 0.0f;
		SpringArmComponent->SetRelativeLocation(StartLocation);
		
		FVector StartOffset = SpringArmComponent->SocketOffset;
		StartOffset.Y = 40.0f;
		SpringArmComponent->SocketOffset = StartOffset;
		bIsRightShoulder= true;
	}
	APlayerController* PlayerController = Cast<APlayerController>(Controller);
	if (PlayerController && PlayerController->IsLocalController())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			if (InputDataAsset)
			{
				if (InputDataAsset->DefaultInputMappingContext)
				{
					Subsystem->AddMappingContext(InputDataAsset->DefaultInputMappingContext, 0);
				}
				if (InputDataAsset->HotKeyInputMappingContext)
				{
					Subsystem->AddMappingContext(InputDataAsset->HotKeyInputMappingContext, 1);
				}
				if (InputDataAsset->InteractionInputMappingContext)
				{
					Subsystem->AddMappingContext(InputDataAsset->InteractionInputMappingContext, 1);
				}
			}
		}
	}
}

void ATSCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D Input = Value.Get<FVector2D>();
	
	if (Input.SizeSquared() > 0.01f && ASC)
	{
		FGameplayTagContainer CancelTags;
		CancelTags.AddTag(AbilityTags::TAG_Ability_Interact_Emote);
		ASC->CancelAbilities(&CancelTags);
	}
	if (IsClimbing())
	{
		const FVector RightDirection = FVector::CrossProduct(CurrentWallNormal, FVector::UpVector).GetSafeNormal();
		const FVector ForwardDirection = FVector::CrossProduct(RightDirection,CurrentWallNormal).GetSafeNormal();
		
		if (!FMath::IsNearlyZero(Input.Y))
		{
			AddMovementInput(ForwardDirection, Input.Y);
		}
		if (!FMath::IsNearlyZero(Input.X))
		{
			AddMovementInput(RightDirection, Input.X);
		}
		return;
	}
	const FRotator ControlRot = Controller->GetControlRotation();
	const FRotator YawRot(0.f, ControlRot.Yaw, 0.f);
	const FVector Forward = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
	const FVector Right = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);

	if (!FMath::IsNearlyZero(Input.Y))
	{
		AddMovementInput(Forward, Input.Y);
	}

	if (!FMath::IsNearlyZero(Input.X))
	{
		AddMovementInput(Right, Input.X);
	}
}

void ATSCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ATSCharacter::ShoulderSwitch(const struct FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("q pressed"));
	if (!SpringArmComponent) return;
	bIsRightShoulder = !bIsRightShoulder;
	FVector NewLocation = SpringArmComponent->GetRelativeLocation();
	if (bIsRightShoulder)
	{
		NewLocation.Y = RightShoulderOffset;
	} else
	{
		NewLocation.Y = LeftShoulderOffset;
	}
	SpringArmComponent->SocketOffset = NewLocation;
}

void ATSCharacter::OnJumpOrClimbStarted(const struct FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("SpaceBar pressed"));
	const FGameplayTag JumpOrClimbTag = AbilityTags::TAG_Ability_Move_JumpOrClimb.GetTag();

	if (ASC && JumpOrClimbTag.IsValid())
	{
		ASC->TryActivateAbilitiesByTag(JumpOrClimbTag.GetSingleTagContainer(), true);
	}
}

void ATSCharacter::OnJumpOrClimbCompleted(const struct FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("SpaceBar end"));
	const FGameplayTag JumpOrClimbTag = AbilityTags::TAG_Ability_Move_JumpOrClimb.GetTag();
	if (ASC && JumpOrClimbTag.IsValid())
	{
		FGameplayTagContainer WithTags;
		WithTags.AddTag(JumpOrClimbTag);
		ASC->CancelAbilities(&WithTags);
	}
}

void ATSCharacter::OnRoll(const struct FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("ctrl pressed"));
	const FGameplayTag RollTag = AbilityTags::TAG_Ability_Move_Roll.GetTag();
	if (ASC && RollTag.IsValid())
	{
		ASC->TryActivateAbilitiesByTag(RollTag.GetSingleTagContainer(), true);
	}
}

void ATSCharacter::OnCrouch(const struct FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("c pressed"));
	if (!ASC) return;
	const FGameplayTag CrouchTag = AbilityTags::TAG_Ability_Move_Crouch.GetTag();
	for (const FGameplayAbilitySpec& Spec : ASC->GetActivatableAbilities())
	{
		const bool bMatch = Spec.DynamicAbilityTags.HasTag(CrouchTag) || (Spec.Ability && Spec.Ability->AbilityTags.HasTag(CrouchTag));
		if (bMatch)
		{
			if (Spec.IsActive())
			{
				ASC->CancelAbility(Spec.Ability);
			} else
			{
				ASC->TryActivateAbility(Spec.Handle);
			}
			return;
		}
	}
}

void ATSCharacter::OnSprintStarted(const struct FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("shift pressed"));
	if (!ASC) return;
	const FGameplayTag SprintTag = AbilityTags::TAG_Ability_Move_Sprint.GetTag(); 
	if (ASC && SprintTag.IsValid())
	{
		ASC->TryActivateAbilitiesByTag(SprintTag.GetSingleTagContainer(), /*bAllowRemoteActivation=*/true);
	}
}

void ATSCharacter::OnSprintCompleted(const struct FInputActionValue& Value)
{
	// Crouch 상태에서 Sprint 누르면 TAG 삭제하도록
	UE_LOG(LogTemp, Log, TEXT("shift end"));
	const FGameplayTag SprintTag = AbilityTags::TAG_Ability_Move_Sprint.GetTag();
	if (ASC && SprintTag.IsValid())
	{
		FGameplayTagContainer WithTags;
		WithTags.AddTag(SprintTag);
		ASC->CancelAbilities(&WithTags);
	}
}

void ATSCharacter::OnOpenBag(const struct FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("i pressed Open Bag"));
	// 빌딩 모드인지 확인
	if (BuildingComponent && BuildingComponent->IsBuildingMode())
	{
		return;
	}
	if (!IsLocallyControlled()) return;
	if (ATSPlayerController* TSController = Cast<ATSPlayerController>(Controller))
	{
		TSController -> ToggleInventory();
	}
}

void ATSCharacter::OnBuild(const struct FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("b pressed"));
	// 빌딩 모드인지 확인
	if (BuildingComponent && BuildingComponent->IsBuildingMode())
	{
		return;
	}
	// 빌딩 위젯 띄우기
	ATSPlayerController* PC = Cast<ATSPlayerController>(GetController());
	if (PC)
	{
		PC->ToggleContentsWidget(EContentWidgetIndex::BuildingMode);
	}
	// const FGameplayTag BuildTag = AbilityTags::TAG_Ability_Interact_Build.GetTag();
	// if (ASC && BuildTag.IsValid())
	// {
	// 	ASC->TryActivateAbilitiesByTag(BuildTag.GetSingleTagContainer(), /*bAllowRemoteActivation=*/true);
	// }
}

void ATSCharacter::OnInteract(const struct FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("e pressed"));
	// 빌딩 모드인지 확인
	if (BuildingComponent && BuildingComponent->IsBuildingMode())
	{
		return;
	}
	if (IsDowned() || IsDead())
	{
		return;
	}
	ATSCharacter* ReviveTarget = DetectReviveTarget();
    
	if (ReviveTarget)
	{
		ServerStartRevive(ReviveTarget);
		bIsInteracting = true;
		return; 
		
	}
	LineTrace();
	if (!IsValid(CurrentHitActor.Get()))
	{
		return;
	}
	if (CurrentHitActor->Implements<UIInteraction>())
	{
		IIInteraction* InteractionInterface = Cast<IIInteraction>(CurrentHitActor);
		if (InteractionInterface && InteractionInterface->CanInteract(this))
		{
			if (InteractionInterface->RunOnServer())
			{
				ServerInteract(CurrentHitActor.Get());
			}
			else
			{
				InteractionInterface->Interact(this);
			}
		}
	}
	if (CurrentHitActor->Implements<UTSResourceItemInterface>() && IsValid(ASC))
	{
		FGameplayTagContainer WithTags;
		WithTags.AddTag(InteractTag::INTERACTTAG_RESOURCE_STARTINTERACT);
		ASC->TryActivateAbilitiesByTag(WithTags);
	}
}

void ATSCharacter::OnStopInteract(const struct FInputActionValue& Value)
{
	if (!bIsInteracting)
	{
		return;
	}
	bIsInteracting = false;
	ServerStopRevive();
	ServerSendStopInteractEvent();
}

void ATSCharacter::OnLeftClick(const struct FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("l-click pressed"));
	// 빌딩 모드인지 확인
	if (BuildingComponent && BuildingComponent->IsBuildingMode())
	{
		BuildingComponent->ConfirmPlacement();
		return;
	}
	// 일반 공격	
	const FGameplayTag LeftClickTag = AbilityTags::TAG_Ability_Interact_LeftClick.GetTag();
	if (ASC && LeftClickTag.IsValid())
	{
		ASC->TryActivateAbilitiesByTag(LeftClickTag.GetSingleTagContainer(), /*bAllowRemoteActivation=*/true);
	}
}

void ATSCharacter::OnRightClick(const struct FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("r-click pressed"));
	// 빌딩 모드인지 확인
	if (BuildingComponent && BuildingComponent->IsBuildingMode())
	{
		return;
	}
	ServerSendUseItemEvent();
	const FGameplayTag RightClickTag = AbilityTags::TAG_Ability_Interact_RightClick.GetTag();
	if (ASC && RightClickTag.IsValid())
	{
		ASC->TryActivateAbilitiesByTag(RightClickTag.GetSingleTagContainer(), /*bAllowRemoteActivation=*/true);
	}
}

void ATSCharacter::OnPingStarted(const struct FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("wheel pressed"));
	// 빌딩 모드인지 확인
	if (BuildingComponent && BuildingComponent->IsBuildingMode())
	{
		return;
	}
	ATSPlayerController* PC = Cast<ATSPlayerController>(GetController());
	if (PC)
	{
		PC->ShowPingUI();
	}
}

void ATSCharacter::OnPingCompleted(const struct FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("Wheel Click Completed"));
	ATSPlayerController* PC = Cast<ATSPlayerController>(GetController());
	if (!PC)
	{
		return;
	}
	ETSPingType NowPingType = PC->HidePingUI();
	if (NowPingType == ETSPingType::NONE)
	{
		return;
	}
	FVector SpawnLocation = FVector::ZeroVector;
	if (!IsValid(GEngine) || !IsValid(GEngine->GameViewport)) return;
	FVector2D ViewPortSize = FVector2D::ZeroVector;
	GEngine->GameViewport->GetViewportSize(ViewPortSize);
	const FVector2D ViewportCenter = ViewPortSize / 2.f;
		
	FVector TraceStart;
	FVector Forward;
	bool bDeprojectSuccess = UGameplayStatics::DeprojectScreenToWorld(
		PC,
		ViewportCenter, 
		TraceStart, 
		Forward
		);
	if (!bDeprojectSuccess)
	{
		return;
	}
	const FVector TraceEnd = TraceStart + (Forward * 50000.0f);
	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
		
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, Params);
	if (bHit)
	{
		SpawnLocation = HitResult.Location;
	} else
	{
		SpawnLocation = TraceEnd;
	}
	
	if (PC)
	{
		PC->PingSuccess();
	}
	ServerSpawnPing(NowPingType, SpawnLocation);
}

void ATSCharacter::OnEmoteStarted(const struct FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT(" T (Emote) pressed"));
	if (BuildingComponent && BuildingComponent->IsBuildingMode())
	{
		return;
	}
	ATSPlayerController* PC = Cast<ATSPlayerController>(GetController());
	if (PC)
	{
		PC->ShowEmoteUI();
	}
}

void ATSCharacter::OnEmoteCompleted(const struct FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT(" T (Emote) end"));
	ATSPlayerController* PC = Cast<ATSPlayerController>(GetController());
	if (PC && ASC)
	{
		ETSEmoteType NowEmoteType = PC->HideEmoteUI();
		if (NowEmoteType == ETSEmoteType::NONE)
		{
			return;
		}
		FGameplayEventData Payload;
		Payload.EventMagnitude = (float) NowEmoteType;
		Payload.Instigator = this;
		Payload.Target = this;
		FGameplayTag EventTag =  AbilityTags::TAG_Event_Play_Emote;
		if (IsLocallyControlled())
		{
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, EventTag, Payload);
		}
		ServerPlayEmote(NowEmoteType);	
	}
}

void ATSCharacter::OnWheelScroll(const struct FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("wheel scroll pressed"));
	// 빌딩 모드인지 확인
	if (BuildingComponent && BuildingComponent->IsBuildingMode())
	{
		BuildingComponent->ServerRotateBuilding(Value.Get<float>());
		return;
	}
	const float WheelValue = Value.Get<float>() * -10.f;
	const float NewTargetArmLength = FMath::Clamp(SpringArmComponent->TargetArmLength + WheelValue, 100.f, 450.f);
	SpringArmComponent->TargetArmLength = NewTargetArmLength;
}

void ATSCharacter::OnEsc(const struct FInputActionValue& Vaule)
{	
	UE_LOG(LogTemp, Log, TEXT("ESC pressed"));
	// 빌딩 모드인지 확인
	if (BuildingComponent && BuildingComponent->IsBuildingMode())
	{
		BuildingComponent->ServerEndBuildingMode();
		return;
	}
	// ESC 키 동작 (열린 위젯 닫기 or 설정창 열기)
	ATSPlayerController* PC = Cast<ATSPlayerController>(GetController());
	if (PC)
	{
		PC->HandleEscapeKey();
	}
}

void ATSCharacter::OnHotKey1(const struct FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("1 pressed"));
	ServerSendHotKeyEvent(0);
}

void ATSCharacter::OnHotKey2(const struct FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("2 pressed"));
	ServerSendHotKeyEvent(1);
}

void ATSCharacter::OnHotKey3(const struct FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("3 pressed"));
	ServerSendHotKeyEvent(2);
}

void ATSCharacter::OnHotKey4(const struct FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("4 pressed"));
	ServerSendHotKeyEvent(3);
}

void ATSCharacter::OnHotKey5(const struct FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("5 pressed"));
	ServerSendHotKeyEvent(4);
}

void ATSCharacter::OnHotKey6(const struct FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("6 pressed"));
	ServerSendHotKeyEvent(5);
}

void ATSCharacter::OnHotKey7(const struct FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("7 pressed"));
	ServerSendHotKeyEvent(6);
}

void ATSCharacter::OnHotKey8(const struct FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("8 pressed"));
	ServerSendHotKeyEvent(7);
}

void ATSCharacter::OnHotKey9(const struct FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("9 pressed"));
	ServerSendHotKeyEvent(8);
}

void ATSCharacter::OnHotKey0(const struct FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("0 pressed"));
	ServerSendHotKeyEvent(9);
}

void ATSCharacter::OnMoveSpeedChanged(const FOnAttributeChangeData& Data)
{
	GetCharacterMovement()->MaxWalkSpeed = Data.NewValue;
	GetCharacterMovement()->MaxWalkSpeedCrouched = Data.NewValue;
}

void ATSCharacter::Landed(const FHitResult& Hit)
{
	float FallingSpeed_Z = GetVelocity().Z;
	Super::Landed(Hit);
	if (FootstepComponent)
	{
		FootstepComponent->PlayFootstepSoundFromHit(Hit);
	}
	if (!HasAuthority() || !ASC )
	{
		return;
	}
	float FallSpeed = FallingSpeed_Z * (-1.0);
	float StandSpeed = 1000.0f;
	if ( FallSpeed < StandSpeed)
	{
		return;
	}
	float FallDamage = (FallSpeed - StandSpeed) * 0.1;
	
	if (FallDamageEffectClass && FallDamage > 0.0f)
	{
		FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
		ContextHandle.AddSourceObject(this);
		FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(FallDamageEffectClass, 1, ContextHandle);
            
		if (SpecHandle.IsValid())
		{
			FGameplayTag DamageTag = AbilityTags::TAG_Data_Damage_Fall;
			SpecHandle.Data.Get()->SetSetByCallerMagnitude(DamageTag, -FallDamage);
			ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}
}

void ATSCharacter::LineTrace()
{
	APlayerController* PC = Cast<APlayerController>(Controller);
	if (!IsValid(PC) || !PC->IsLocalController()) return;
	if (!IsValid(GEngine) || !IsValid(GEngine->GameViewport)) return;
	FVector2D ViewPortSize = FVector2D::ZeroVector;
	GEngine->GameViewport->GetViewportSize(ViewPortSize);
	const FVector2D ViewportCenter = ViewPortSize / 2.f;
	FVector TraceStart;
	FVector Forward;
	bool DeprojectScreenToWorld= UGameplayStatics::DeprojectScreenToWorld(GetWorld()->GetFirstPlayerController(), ViewportCenter, TraceStart, Forward);
	if (!DeprojectScreenToWorld) return;
	const FVector TraceEnd = TraceStart + Forward * TraceLength;
	FHitResult HitResult;
	    
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	
	// GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, TraceChannel);
	if (!GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Pawn, Params))
	{
		GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, TraceChannel, Params);
	}
	LastHitActor = CurrentHitActor;
	CurrentHitActor = HitResult.GetActor();
	
	// 같은 액터 처리
	if (CurrentHitActor == LastHitActor)
	{
		// 같은 액터지만 상호작용 불가능으로 바뀐 경우 처리
		if (IsValid(CurrentHitActor.Get()))
		{
			if (CurrentHitActor->Implements<UIInteraction>())
			{
				IIInteraction* InteractionInterface = Cast<IIInteraction>(CurrentHitActor);
				if (InteractionInterface && !InteractionInterface->CanInteract(this))
				{
					InteractionInterface->HideInteractionWidget();
					if (bIsInteracting)
					{
						OnReticleInteractionEnd.Broadcast();
						bIsInteracting = false;
					}
				}
			}
		}
		return;
	}
	// 이전 액터 처리
	if (IsValid(LastHitActor.Get()))
	{
		if (LastHitActor->Implements<UIInteraction>())
		{
			IIInteraction* InteractionInterface = Cast<IIInteraction>(LastHitActor);
			if (InteractionInterface && InteractionInterface->CanInteract(this))
			{
				InteractionInterface->HideInteractionWidget();
				OnReticleInteractionEnd.Broadcast();
			}
		}
		// 클라이밍 레티클 상호작용 끝내기
		else if(LastHitActor->ActorHasTag("Climbable"))
		{
			OnReticleInteractionEnd.Broadcast();
		}
	}
	// 현재 액터 처리
	if (IsValid(CurrentHitActor.Get()))
	{
		if (CurrentHitActor->Implements<UIInteraction>())
		{
			IIInteraction* InteractionInterface = Cast<IIInteraction>(CurrentHitActor);
			if (InteractionInterface && InteractionInterface->CanInteract(this))
			{
				InteractionInterface->ShowInteractionWidget(this);
				OnReticleInteractionBegin.Broadcast();
			}
			else
			{
				InteractionInterface->HideInteractionWidget();
			}
		}
		// 클라이밍 레티클 상호작용 시작
		else if(CurrentHitActor->ActorHasTag("Climbable"))
		{
			OnReticleInteractionBegin.Broadcast();
		}
	}
}

bool ATSCharacter::IsClimbing()
{
	return bIsClimbState;
}

void ATSCharacter::ShowInteractionWidget(ATSCharacter* InstigatorCharacter)
{	
	if (!InteractionWidget)
	{
		return;
	}
	InteractionWidget->SetVisibility(true);
	if (CanInteract(InstigatorCharacter))
	{
		SetInteractionText(InteractionText);
	}
}

void ATSCharacter::HideInteractionWidget()
{
	if (!InteractionWidget)
	{
		return;
	}
	InteractionWidget->SetVisibility(false);
}

void ATSCharacter::SetInteractionText(FText WidgetText)
{
	UUserWidget* Widget = InteractionWidget->GetWidget();
	if (Widget)
	{
		UTextBlock* TextBlock = Cast<UTextBlock>(Widget->GetWidgetFromName(TEXT("InteractionText")));
		if (TextBlock)
		{
			TextBlock->SetText(WidgetText);
		}
	}
}

bool ATSCharacter::CanInteract(ATSCharacter* InstigatorCharacter)
{
	if (!IsValid(InstigatorCharacter))
	{
		return false;
	}
	if (!IsValid(InstigatorCharacter->ASC) || !IsValid(ASC))
	{
		return false;
	}
	// 상호작용 실행할 플레이어 상태 확인
	if (InstigatorCharacter->ASC->HasMatchingGameplayTag(AbilityTags::TAG_State_Status_Downed)
		|| InstigatorCharacter->ASC->HasMatchingGameplayTag(AbilityTags::TAG_State_Status_Dead))
	{
		return false;
	}
	// 상호작용 대상 플레이어 상태 확인
	if (ASC->HasMatchingGameplayTag(AbilityTags::TAG_State_Status_Downed))
	{
		return true;
	}
	return false;
}

void ATSCharacter::Interact(ATSCharacter* InstigatorCharacter)
{
}

bool ATSCharacter::RunOnServer()
{
	return false;
}

void ATSCharacter::ServerSpawnPing_Implementation(ETSPingType PingType, FVector Location)
{
	if ( !PingActorClass)
	{
		return;
	}
	FTransform SpawnPingTransform(FRotator::ZeroRotator, Location);
	ATSPingActor* Ping = GetWorld()->SpawnActorDeferred<ATSPingActor>(PingActorClass, SpawnPingTransform, this, this,ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	if (Ping)
	{
		Ping->PingType = PingType;
		UGameplayStatics::FinishSpawningActor(Ping, SpawnPingTransform);
	}
}

void ATSCharacter::ServerPlayEmote_Implementation(ETSEmoteType EmoteType)
{
	if (!ASC)
	{
		return;
	}
	FGameplayEventData Payload;
	Payload.EventMagnitude = (float) EmoteType;
	Payload.Instigator = this;
	Payload.Target = this;
	
	FGameplayTag EventTag =  AbilityTags::TAG_Event_Play_Emote;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, EventTag, Payload);
	
}

void ATSCharacter::ServerSendHotKeyEvent_Implementation(int HotKeyIndex)
{
	// 빌딩 모드인지 확인
	if (BuildingComponent && BuildingComponent->IsBuildingMode())
	{
		return;
	}
	if (!ASC) return;
	const FGameplayTag EventTag = AbilityTags::TAG_Input_HotKey.GetTag();
	FGameplayEventData EventData;
	EventData.EventTag = EventTag;
	EventData.EventMagnitude = static_cast<float>(HotKeyIndex);

	EventData.Instigator = this;
	EventData.Target = this;
	
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this/*Actor*/, EventTag, EventData/*Payload*/);
}

bool ATSCharacter::ServerSendHotKeyEvent_Validate(int HotKeyIndex)
{
	return HotKeyIndex >= 0 && HotKeyIndex < 10;
}

void ATSCharacter::ServerSendUseItemEvent_Implementation()
{
	// BP_GA_Hotkey에서 WaitGameplayEvent로 받아서 UseItem 실행
	if (!ASC) return;
	const FGameplayTag EventTag = FGameplayTag::RequestGameplayTag(FName("Ability.Interact.RightClick"));
	FGameplayEventData EventData;
	EventData.EventTag = EventTag;

	EventData.Instigator = this;
	EventData.Target = this;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this/*Actor*/, EventTag, EventData/*Payload*/);
}

bool ATSCharacter::ServerSendUseItemEvent_Validate()
{
	return IsValid(ASC);
}

void ATSCharacter::ServerInteract_Implementation(AActor* TargetActor)
{
	if (!IsValid(TargetActor))
	{
		return;
	}
	if (TargetActor->Implements<UIInteraction>())
	{
		IIInteraction* InteractionInterface = Cast<IIInteraction>(TargetActor);
		if (InteractionInterface && InteractionInterface->CanInteract(this))
		{
			Multicast_PlayPickUpMontage();
			InteractionInterface->Interact(this);
		}
	}
}

bool ATSCharacter::ServerInteract_Validate(AActor* TargetActor)
{
	return IsValid(TargetActor);
}

void ATSCharacter::ServerSendStopInteractEvent_Implementation()
{
	if (IsValid(ASC))
	{
		FGameplayEventData EventData;
		ASC->HandleGameplayEvent(InteractTag::INTERACTTAG_STOPINTERACT, &EventData);
	}
}

void ATSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (HasAuthority() && bIsRescuing)
	{
		TickReviveValidation(); // 구조 중이면 -? 매 프레임 거리와 상태 체크
	}
	// ■ WASD 태그 관리
	//[S]=====================================================================================
	if (ASC && GetCharacterMovement())
	{
		// 이동 상태 태그 정의
		const FGameplayTag WASDTag = AbilityTags::TAG_State_Move_WASD.GetTag();
		
		// 태그 유효성 확인
		if (!WASDTag.IsValid())
		{
			UE_LOG(LogTemp, Error, TEXT("[WASD] TAG_State_Move_WASD is INVALID! 태그가 등록되지 않았습니다!"));
			LineTrace(); // 디버깅용 레이 트레이스
			return;
		}
		
		// 현재 캐릭터가 해당 태그를 가지고 있는지 확인
		bool bHasTag = ASC->HasMatchingGameplayTag(WASDTag);
		
		// 실제 이동 속도로 이동 여부 판단
		FVector Velocity = GetVelocity();
		float Speed2D = Velocity.Size2D();	// Z축 제외, 수평 속도만 계산
		bool bIsMoving = Speed2D > 1.0f;	// 1.0f 이상이면 이동 중으로 간주
		
		// 매 프레임 상태 로그 (디버깅용)
		// UE_LOG(LogTemp, Warning, TEXT("[WASD Debug] Speed: %.2f | IsMoving: %s | HasTag: %s | ASC Valid: %s"), 
		// 	Speed2D, 
		// 	bIsMoving ? TEXT("YES") : TEXT("NO"),
		// 	bHasTag ? TEXT("YES") : TEXT("NO"),
		// 	ASC ? TEXT("YES") : TEXT("NO"));
		
		if (bIsMoving && !bHasTag)
		{// 이동 중이고 태그가 없으면 태그 추가
			ASC->AddLooseGameplayTag(WASDTag);
			//UE_LOG(LogTemp, Error, TEXT("[WASD] ★★★ 태그 추가됨! Speed: %.2f ★★★"), Speed2D);
			
			// 태그가 실제로 추가되었는지 재확인
			bool bCheckTag = ASC->HasMatchingGameplayTag(WASDTag);
			//UE_LOG(LogTemp, Error, TEXT("[WASD] 추가 후 재확인: %s"), bCheckTag ? TEXT("성공") : TEXT("실패!"));
		}
		else if (!bIsMoving && bHasTag)
		{// 이동하지 않는데 태그가 있으면 제거
			ASC->RemoveLooseGameplayTag(WASDTag);
			//UE_LOG(LogTemp, Error, TEXT("[WASD] ★★★ 태그 제거됨! ★★★"));
		}
	}
	else
	{// ASC 또는 CharacterMovement가 nullptr이면 에러 로그 출력
		//UE_LOG(LogTemp, Error, TEXT("[WASD] ASC 또는 CharacterMovement가 nullptr!"));
	}
	//[E]=====================================================================================
	
	LineTrace();
}

void ATSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (!InputDataAsset) return;

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(InputDataAsset->MoveAction, ETriggerEvent::Triggered, this,
		                                   &ATSCharacter::Move);
		EnhancedInputComponent->BindAction(InputDataAsset->LookAction, ETriggerEvent::Triggered, this,
		                                   &ATSCharacter::Look);
		EnhancedInputComponent->BindAction(InputDataAsset->ShoulderSwitchAction, ETriggerEvent::Started, this,
		                                   &ATSCharacter::ShoulderSwitch);

		EnhancedInputComponent->BindAction(InputDataAsset->JumpOrClimbAction, ETriggerEvent::Started, this,
		                                   &ATSCharacter::OnJumpOrClimbStarted);
		EnhancedInputComponent->BindAction(InputDataAsset->JumpOrClimbAction, ETriggerEvent::Completed, this,
										   &ATSCharacter::OnJumpOrClimbCompleted);
		EnhancedInputComponent->BindAction(InputDataAsset->RollAction, ETriggerEvent::Started, this,
		                                   &ATSCharacter::OnRoll);
		EnhancedInputComponent->BindAction(InputDataAsset->CrouchAction, ETriggerEvent::Started, this,
										   &ATSCharacter::OnCrouch);
		EnhancedInputComponent->BindAction(InputDataAsset->OpenBagAction, ETriggerEvent::Started, this,
									   &ATSCharacter::OnOpenBag);
		EnhancedInputComponent->BindAction(InputDataAsset->BuildAction, ETriggerEvent::Started, this,
		                                   &ATSCharacter::OnBuild);
		EnhancedInputComponent->BindAction(InputDataAsset->InteractAction, ETriggerEvent::Started, this,
		                                   &ATSCharacter::OnInteract);
		
		EnhancedInputComponent->BindAction(InputDataAsset->InteractAction, ETriggerEvent::Canceled, this,
										   &ATSCharacter::OnStopInteract);
		
		EnhancedInputComponent->BindAction(InputDataAsset->InteractAction, ETriggerEvent::Completed, this,
										   &ATSCharacter::OnStopInteract);
		
		EnhancedInputComponent->BindAction(InputDataAsset->PingAction, ETriggerEvent::Started, this,
		                                   &ATSCharacter::OnPingStarted);
		EnhancedInputComponent->BindAction(InputDataAsset->PingAction, ETriggerEvent::Completed, this,
										   &ATSCharacter::OnPingCompleted);
		EnhancedInputComponent->BindAction(InputDataAsset->EmoteAction, ETriggerEvent::Started, this,
										   &ATSCharacter::OnEmoteStarted);
		EnhancedInputComponent->BindAction(InputDataAsset->EmoteAction, ETriggerEvent::Completed, this,
										   &ATSCharacter::OnEmoteCompleted);
		EnhancedInputComponent->BindAction(InputDataAsset->WheelScrollAction, ETriggerEvent::Triggered, this,
		                                   &ATSCharacter::OnWheelScroll);
		EnhancedInputComponent->BindAction(InputDataAsset->LeftClickAction, ETriggerEvent::Started, this,
		                                   &ATSCharacter::OnLeftClick);
		EnhancedInputComponent->BindAction(InputDataAsset->RightClickAction, ETriggerEvent::Started, this,
		                                   &ATSCharacter::OnRightClick);
		EnhancedInputComponent->BindAction(InputDataAsset->SprintAction, ETriggerEvent::Started, this,
		                                   &ATSCharacter::OnSprintStarted);
		EnhancedInputComponent->BindAction(InputDataAsset->SprintAction, ETriggerEvent::Completed, this,
		                                   &ATSCharacter::OnSprintCompleted);
		EnhancedInputComponent->BindAction(InputDataAsset->EscAction, ETriggerEvent::Started, this,
										   &ATSCharacter::OnEsc);

		EnhancedInputComponent->BindAction(InputDataAsset->HotKey1Action, ETriggerEvent::Started, this,
		                                   &ATSCharacter::OnHotKey1);
		EnhancedInputComponent->BindAction(InputDataAsset->HotKey2Action, ETriggerEvent::Started, this,
		                                   &ATSCharacter::OnHotKey2);
		EnhancedInputComponent->BindAction(InputDataAsset->HotKey3Action, ETriggerEvent::Started, this,
		                                   &ATSCharacter::OnHotKey3);
		EnhancedInputComponent->BindAction(InputDataAsset->HotKey4Action, ETriggerEvent::Started, this,
		                                   &ATSCharacter::OnHotKey4);
		EnhancedInputComponent->BindAction(InputDataAsset->HotKey5Action, ETriggerEvent::Started, this,
		                                   &ATSCharacter::OnHotKey5);
		EnhancedInputComponent->BindAction(InputDataAsset->HotKey6Action, ETriggerEvent::Started, this,
		                                   &ATSCharacter::OnHotKey6);
		EnhancedInputComponent->BindAction(InputDataAsset->HotKey7Action, ETriggerEvent::Started, this,
		                                   &ATSCharacter::OnHotKey7);
		EnhancedInputComponent->BindAction(InputDataAsset->HotKey8Action, ETriggerEvent::Started, this,
		                                   &ATSCharacter::OnHotKey8);
		EnhancedInputComponent->BindAction(InputDataAsset->HotKey9Action, ETriggerEvent::Started, this,
		                                   &ATSCharacter::OnHotKey9);
		EnhancedInputComponent->BindAction(InputDataAsset->HotKey0Action, ETriggerEvent::Started, this,
		                                   &ATSCharacter::OnHotKey0);
	}
}

#pragma region Multicast_ConsumeMontage
void ATSCharacter::Multicast_PlayConsumeMontage_Implementation(
	UAnimMontage* Montage, float PlayRate, float ServerStartTime)
{
	if (!Montage)
	{
		return;
	}
	
	// 몽타주 재생 전 유효성 체크 강화
	if (!Montage->IsValidLowLevel())
	{
		UE_LOG(LogCharacter, Error, TEXT("[Multicast] 유효하지 않은 몽타주!"));
		return;
	}
	
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!AnimInstance)
	{
		return;
	}
	
	// 애니메이션 인스턴스 상태 확인
	if (!AnimInstance->IsValidLowLevel())
	{
		UE_LOG(LogCharacter, Error, TEXT("[Multicast] AnimInstance 상태 이상!"));
		return;
	}
	
	// 이미 재생 중이면 스킵
	// if (AnimInstance->Montage_IsPlaying(Montage))
	// {
	// 	return;
	// }
	// 이미 재생 중이면 정지 후 재시작
	if (AnimInstance->Montage_IsPlaying(Montage))
	{
		AnimInstance->Montage_Stop(0.0f, Montage);
	}
	
	//=======================================================================
	// 네트워크 지연 보정 로직 추가
	//=======================================================================
	float StartPosition = 0.0f;
	
	// 서버가 아닌 경우에만 시간 차이 계산
	if (!HasAuthority())
	{
		// 1. 현재 클라이언트 로컬 시간
		float LocalTime = GetWorld()->GetTimeSeconds();
		
		// 2. 서버가 시작한 시점부터 얼마나 지났는지 계산
		float Elapsed = FMath::Max(LocalTime - ServerStartTime, 0.0f);
		
		// 3. 몽타주 길이 확인
		float MontageLength = Montage->GetPlayLength();
		
		// 4. 이미 몽타주가 끝난 시점이라면 재생하지 않음
		if (Elapsed >= MontageLength)
		{
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
			UE_LOG(LogCharacter, Warning, TEXT("[Client] 몽타주 시작 지연 과다 (경과=%.2fs, 길이=%.2fs) - 재생 스킵"),
				Elapsed, MontageLength);
			return; // 재생 스킵
#endif
		}
		
		// 5. 지연된 시간만큼 건너뛰기
		StartPosition = Elapsed;

#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
		UE_LOG(LogCharacter, Log, TEXT("[Client] 네트워크 지연 보정: %.2f초 건너뛰고 재생 (전체 %.2fs)"),
			StartPosition, MontageLength);
#endif
	}
	
	//=======================================================================
	// 몽타주 재생 (시작 위치 적용)
	//=======================================================================
	float PlayedLength = AnimInstance->Montage_Play(
		Montage,
		PlayRate,
		EMontagePlayReturnType::MontageLength,
		StartPosition // 지연된 시간만큼 건너뜀
	);
	
	if (PlayedLength > 0.f)
	{
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
		UE_LOG(LogCharacter, Log, TEXT("[Multicast] 몽타주 재생: %s (Role: %s, StartPos: %.2fs)"),
			*Montage->GetName(),
			HasAuthority() ? TEXT("Server") : TEXT("Client"),
			StartPosition);
#endif
	}
	else
	{
		UE_LOG(LogCharacter, Warning, TEXT("[Multicast] 몽타주 재생 실패: %s"), *Montage->GetName());
	}
}

void ATSCharacter::Multicast_StopConsumeMontage_Implementation(UAnimMontage* Montage)
{
	if (!Montage)
	{
		return;
	}
	
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!AnimInstance)
	{
		return;
	}
	
	if (AnimInstance->Montage_IsPlaying(Montage))
	{
		AnimInstance->Montage_Stop(0.2f, Montage);
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
		UE_LOG(LogCharacter, Log, TEXT("[Multicast] 몽타주 정지: %s (Role: %s)"),
			*Montage->GetName(), HasAuthority() ? TEXT("Server") : TEXT("Client"));
#endif
	}
}
#pragma endregion

void ATSCharacter::Multicast_PlayPickUpMontage_Implementation()
{
	if (!PickUpMontage)
	{
		return;
	}
	PlayAnimMontage(PickUpMontage);
}