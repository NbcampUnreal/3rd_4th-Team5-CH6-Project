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
DEFINE_LOG_CATEGORY_STATIC(LogTSCharacter, Log, All);

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
	bUseControllerRotationYaw = true; //카메라 시야랑 캐릭터 눈이랑 맞아야할걸? << 기획파트한테 물어봐야함
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
	InitializeAbilities(); //어빌리티 부여
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
	
	// 빛 탐지를 위한 0.1초 타이머 시작
	GetWorld()->GetTimerManager().SetTimer(LightCheckTimerHandle,this,&ATSCharacter::CheckInLightSource,0.1f,true);
	
	//테스트 코드 GE_TempHot,GE_TempCold 잘 되는지 테스트 하는 용도
	// 이 GE는 추후 삭제 예정 (체온 원하는 값으로 override 할 수 있는 GE)
	// 현재 값 : 30으로 되어있음. 체온에 관한 아이템 적용 되면 삭제할 예정
	if (TempTESTClass)
	{
		FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
		ContextHandle.AddSourceObject(this);
		FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(TempTESTClass, 1, ContextHandle);
		if (SpecHandle.IsValid())
		{
			ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}
	// 여기까지 체온 테스트 코드
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
				UE_LOG(LogTSCharacter, Warning, TEXT("[서버] Hunger 변경: %.2f -> %.2f"), Data.OldValue, Data.NewValue);
			}
		});
		
		// ■ 추적: Thirst 변경 추적 (서버에서만 로그)
		ASC->GetGameplayAttributeValueChangeDelegate(
			UTSAttributeSet::GetThirstAttribute()
		).AddLambda([this](const FOnAttributeChangeData& Data)
		{
			if (HasAuthority())
			{
				UE_LOG(LogTSCharacter, Warning, TEXT("[서버] Thirst 변경: %.2f -> %.2f"), Data.OldValue, Data.NewValue);
			}
		});
		
		// ■ 추적: Health 변경 추적 (서버에서만 로그)
		ASC->GetGameplayAttributeValueChangeDelegate(
			UTSAttributeSet::GetHealthAttribute()
		).AddLambda([this](const FOnAttributeChangeData& Data)
		{
			if (HasAuthority())
			{
				UE_LOG(LogTSCharacter, Warning, TEXT("[서버] Health 변경: %.2f -> %.2f"), Data.OldValue, Data.NewValue);
			}
		});
		
		// ■ 추적: Sanity 변경 추적 (서버에서만 로그)
		ASC->GetGameplayAttributeValueChangeDelegate(
			UTSAttributeSet::GetSanityAttribute()
		).AddLambda([this](const FOnAttributeChangeData& Data)
		{
			if (HasAuthority())
			{
				UE_LOG(LogTSCharacter, Warning, TEXT("[서버] Sanity 변경: %.2f -> %.2f"), Data.OldValue, Data.NewValue);
			}
		});
		
		// ■ 추적: Temperature 변경 추적 (서버에서만 로그)
		ASC->GetGameplayAttributeValueChangeDelegate(
			UTSAttributeSet::GetTemperatureAttribute()
		).AddLambda([this](const FOnAttributeChangeData& Data)
		{
			if (HasAuthority())
			{
				UE_LOG(LogTSCharacter, Warning, TEXT("[서버] Temperature 변경: %.2f -> %.2f"), Data.OldValue, Data.NewValue);
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
				UE_LOG(LogTSCharacter, Warning, TEXT("태그 적용: %s"), *CallbackTag.ToString());
			}
			else
			{
				UE_LOG(LogTSCharacter, Warning, TEXT("태그 제거: %s"), *CallbackTag.ToString());
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
				UE_LOG(LogTSCharacter, Warning, TEXT("태그 적용: %s"), *CallbackTag.ToString());
			}
			else
			{
				UE_LOG(LogTSCharacter, Warning, TEXT("태그 제거: %s"), *CallbackTag.ToString());
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
	
	// Hit 나중에 네이티브로 바꾸셈
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
}

void ATSCharacter::BecomeDowned()
{
	if (!ASC)
	{
		return;
	}
	// 1. 기존 행동 취소 및 Downed 태그 부착
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

	// 2. 이동속도 감소 GE 적용
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
	
	// 3. DownedHealth 감소 GE 적용
	if (DownedEffectClass)
	{
		FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
		ContextHandle.AddSourceObject(this);
		FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(DownedEffectClass, 1, ContextHandle);
            
		if (SpecHandle.IsValid())
		{
			ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}
	// 4. 클라이언트 애니메이션 동기화
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
		return; // 이미 죽었으면 return
	}

	// 1. 태그 Downed -> Dead 교체
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
	// 2. 상태 변수 업데이트 
	bIsDownedState = false;
	bIsDeadState = true;
	// 3. 입력 차단
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		DisableInput(PC);
	}
	// 4. 서버에서도 Ragdoll 물리 적용
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
		// 1. 캡슐 콜리전 끄기 (시체 통과 가능하게)
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		// 2. 메쉬 물리 켜기 (철퍼덕)
		GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
		GetMesh()->SetSimulatePhysics(true);
		// 3. 이동 컴포넌트 정지
		if (GetCharacterMovement())
		{
			GetCharacterMovement()->StopMovementImmediately();
			GetCharacterMovement()->DisableMovement();
		}
	}
}

ATSCharacter* ATSCharacter::DetectReviveTarget()
{
	// ---------------Downed Character 탐지용 트레이스---------------
	
	// 오직 로컬 플레이어 컨트롤러에서만 실행
	APlayerController* PC = Cast<APlayerController>(Controller);
	if (!IsValid(PC) || !PC->IsLocalController()) return nullptr;
	
	// 화면 중앙 위치 가져오기
	if (!IsValid(GEngine) || !IsValid(GEngine->GameViewport)) return nullptr;
	
	// 화면 중앙 좌표 계산
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
    
	// 디버그
	//if (bLineTraceDebugDraw && bHit)
	//{
	//	DrawDebugCapsule(GetWorld(), HitResult.Location, 50.0f, 40.0f, FQuat::Identity, FColor::Red, false, 1.0f);
	//}

	if (bHit)
	{
		return Cast<ATSCharacter>(HitResult.GetActor());
	}
	return nullptr;
	
}

void ATSCharacter::Revive() // Downed된 친구가 부활하는 함수
{
	if (IsDead())
	{
		return;
	}
	// 1. Downed 태그 제거
	if (ASC)
	{
		ASC->RemoveActiveEffectsWithGrantedTags(FGameplayTagContainer(AbilityTags::TAG_State_Status_Downed));
	}
	// 2. Health 50 회복, DownedHealth 100 초기화
	if (UTSAttributeSet* AS = GetAttributeSet())
	{
		AS->SetHealth(50.0f);
		AS->SetDownedHealth(AS->GetMaxDownedHealth());
	}
	// 3. 변수 초기화
	bIsDownedState = false;
	
	if (HasAuthority())
	{
		OnRep_IsDownedState();
	}
}

bool ATSCharacter::IsRescueCharacter() const
{
	// 구조 중 상태 확인
	if (ASC)
	{
		return ASC->HasMatchingGameplayTag(AbilityTags::TAG_State_Status_Rescuing);
	}
	return false;
}

void ATSCharacter::ServerStartRevive_Implementation(ATSCharacter* Target)
{
	// 1. 구조자 상태, 타겟 상태, 거리 확인, 대상 탐지
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
	// 2. 타이머 중복 방지 (정리)
	if (GetWorldTimerManager().IsTimerActive(ReviveTimerHandle))
	{
		GetWorldTimerManager().ClearTimer(ReviveTimerHandle);
	}
	
	// 3. 소생 상태 설정
	ReviveTargetCharacter = Target; // 소생 대상을 변수에 저장
	CurrentReviveTime = 0.f;
	bIsRescuing = true;
	
	// 4. Rescue 태그 부착
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
	
	// 5. 내 움직임 봉인 // 구조 중 이동 불가
	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->SetMovementMode(MOVE_None);
	}
    
	// 6. 타이머 시작 (5초)
	GetWorldTimerManager().SetTimer(ReviveTimerHandle, this, &ATSCharacter::OnReviveFinished, ReviveDuration, false);
}

void ATSCharacter::ServerStopRevive_Implementation()
{
	// 1. 타이머 정리
	GetWorldTimerManager().ClearTimer(ReviveTimerHandle);
	CurrentReviveTime = 0.f;
	
	// 2. 변수 초기화
	ReviveTargetCharacter = nullptr;
	bIsRescuing = false;
	
	// 2. Rescue 태그 제거
	if (ASC)
	{
		ASC->RemoveActiveEffectsWithGrantedTags(FGameplayTagContainer(AbilityTags::TAG_State_Status_Rescuing));
	}
	
	// 4. 이동상태 복구
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	
	// 5. 클라이언트 동기화
	ClientForceStopRevive();
}

void ATSCharacter::ClientForceStopRevive_Implementation()
{
	GetCharacterMovement()->SetMovementMode(MOVE_Walking); // 소생 강제 해제 -> 움직임 복구
}

void ATSCharacter::OnReviveFinished()
{
	// 1. 타이머 정리
	GetWorldTimerManager().ClearTimer(ReviveTimerHandle);
	CurrentReviveTime = 0.f;
	// 2. 타겟 정리 및 변수 초기화
	ATSCharacter* Target = ReviveTargetCharacter;
	ReviveTargetCharacter = nullptr;
	// 3. 태그 제거
	if (ASC)
	{
		ASC->RemoveActiveEffectsWithGrantedTags(FGameplayTagContainer(AbilityTags::TAG_State_Status_Rescuing));
	}
	// 4. 이동상태 복구
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	bIsRescuing = false;
	// 5. 살려주기 + 안전성 검사
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
	// 1. 대상이 사라짐 -> 취소
	if (!IsValid(ReviveTargetCharacter))
	{
		ServerStopRevive();
		return;
	}

	// 2. 대상이 그 사이에 죽어버렸거나, 이미 일어남 -> 취소
	if (ReviveTargetCharacter->IsDead() || !ReviveTargetCharacter->IsDowned())
	{
		ServerStopRevive();
		return;
	}

	// 3. 거리 계산 (기어가서 멀어짐 방지)
	float DistSq = FVector::DistSquared(GetActorLocation(), ReviveTargetCharacter->GetActorLocation());
	float MaxDistSq = MaxReviveDistance * MaxReviveDistance; // sqrt 말고 곱셈으로..
	if (DistSq > MaxDistSq)
	{
		ServerStopRevive();
		return;
	}
	
	// 4. 살리는 중간에 기절하거나 죽으면 소생 취소
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
	// 빛구역인지 확인하자
	TArray<AActor*> LightActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(),AErosionLightSourceSubActor::StaticClass(), LightActors);
	const FGameplayTag InLightTag = AbilityTags::TAG_State_Status_InLightSourceRange;
	if (LightActors.Num() == 0)
	{
		if (ASC->HasMatchingGameplayTag(InLightTag)) //주변에 빛 액터 없는데 라이트 태그 갖고있으면 없애기
		{
			ASC->RemoveActiveEffectsWithGrantedTags(FGameplayTagContainer(InLightTag));
		}
		return;
	}
	
	float NearestDistance = 0.0f;
	
	AActor* NearestActor = UGameplayStatics::FindNearestActor(GetActorLocation(),LightActors,NearestDistance);
	
	AErosionLightSourceSubActor* NearestLight = Cast<AErosionLightSourceSubActor>(NearestActor);
	
	const float LightRange = 5000.0f; // 50 미터?
	
	if (NearestLight && NearestDistance <= LightRange)
	{
		// 빛도 있고 가까우면
		// 이건 빛구역이다 -> 태그 넣기
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
		// 어둠 구역이니까 라이트 태그 떼기
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
		BaseSpringArmSocketOffset = SpringArmComponent -> SocketOffset;
		SpringArmBaseLocation = SpringArmComponent->GetRelativeLocation();
		SpringArmRightLocation = SpringArmBaseLocation + FVector(0.f, RightShoulderOffset, 0.f); // +40
		SpringArmLeftLocation = SpringArmBaseLocation + FVector(0.f, LeftShoulderOffset, 0.f); // -80

		SpringArmComponent->SetRelativeLocation(SpringArmRightLocation);
		bIsRightShoulder = true;
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

void ATSCharacter::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	OriginalCrouchHeightAdjust = ScaledHalfHeightAdjust; // 캡슐 줄어든 크기 저장해서 이만큼 카메라 들어올리기
}

void ATSCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	OriginalCrouchHeightAdjust = ScaledHalfHeightAdjust; // 캡슐이 커진 크기 저장
}

void ATSCharacter::UpdateCrouchCamera()
{
	if (IsLocallyControlled())
	{
		if (SpringArmComponent)
		{
			UAnimInstance* Anim = GetMesh()->GetAnimInstance();
			if (Anim)
			{
				float Alpha = Anim->GetCurveValue(CrouchCurveName);
				Alpha = FMath::Clamp(Alpha, 0.f, 1.f); //애니메이션 커브값 가져오기
				
				const float TargetZ=FMath::Lerp(StandCameraZ,CrouchCameraZ,Alpha);
				const float Adjust = OriginalCrouchHeightAdjust;
				// crouched 상태면 캡슐 낮아졌으니 카메라를 위로 보정
				// 반대면 캡슐이 높아졌으니 카메라를 아래로 보정
				const float JumpCancel = bIsCrouched ? (Adjust * (1.f - Alpha)) : (-Adjust * Alpha);
				
				FVector NewOffset = BaseSpringArmSocketOffset;
				NewOffset.Z += (TargetZ + JumpCancel);
				
				SpringArmComponent->SocketOffset = NewOffset;
			}
		}
	}
}

void ATSCharacter::UnlockCrouchToggle()
{
	bCrouchToggleLocked = false;
}

void ATSCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D Input = Value.Get<FVector2D>();
	
	if (Input.SizeSquared() > 0.01f && ASC)
	{
		FGameplayTagContainer CancelTags;
		CancelTags.AddTag(AbilityTags::TAG_Ability_Interact_Emote);
		//감정표현 취소
		ASC->CancelAbilities(&CancelTags);
	}
	
	// 클라이밍 이동 로직 Cross Product 언리얼에선 왼손 규칙 씀
	if (IsClimbing())
	{
		//오른쪽 이동 방향 구하기 (cross product 외적 -> 벽의 법선과 월드 위쪽을 섞으면 오른쪽방향 나온다고 함........)
		const FVector RightDirection = FVector::CrossProduct(CurrentWallNormal, FVector::UpVector).GetSafeNormal();
		// 위쪽 이동 방향 구하기 (cross product 외적 -> 구한 오른쪽과 법선을 다시 섞으면 위쪽 방향이 나온다고 함..)
		const FVector ForwardDirection = FVector::CrossProduct(RightDirection,CurrentWallNormal).GetSafeNormal();
		
		if (!FMath::IsNearlyZero(Input.Y)) // w s 키 
		{
			AddMovementInput(ForwardDirection, Input.Y);
		}
		if (!FMath::IsNearlyZero(Input.X)) // a d 키
		{
			AddMovementInput(RightDirection, Input.X);
		}
		return;
	}
	//기본 이동 로직 (걷기) wasd
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
	if (bIsSwitchingShoulder) return; //이미 전환중이면 무시
	
	bIsSwitchingShoulder = true; 
	ShoulderSwitchElapsed = 0.f;
	
	ShoulderStartOffset = SpringArmComponent->GetRelativeLocation();
	ShoulderTargetOffset = bIsRightShoulder ? SpringArmLeftLocation : SpringArmRightLocation;
	bIsRightShoulder = !bIsRightShoulder;
}

//---- Non GAS

//GAS
void ATSCharacter::OnJumpOrClimbStarted(const struct FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("SpaceBar pressed"));
	const FGameplayTag JumpOrClimbTag = AbilityTags::TAG_Ability_Move_JumpOrClimb.GetTag();
	const FGameplayTag CrouchStateTag = AbilityTags::TAG_State_Move_Crouch.GetTag();

	// 앉아있으면 먼저 GA_Crouch 취소
	if (ASC->HasMatchingGameplayTag(CrouchStateTag))
	{
		const FGameplayTag CrouchAbilityTag = AbilityTags::TAG_Ability_Move_Crouch.GetTag();
		FGameplayTagContainer CancelTags;
		CancelTags.AddTag(CrouchAbilityTag);
		ASC->CancelAbilities(&CancelTags);
	}

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
	//연타방지
	if (bCrouchToggleLocked) return;
	bCrouchToggleLocked = true;
	const FGameplayTag CrouchTag = AbilityTags::TAG_Ability_Move_Crouch.GetTag();
	const FGameplayTag CrouchStateTag = AbilityTags::TAG_State_Move_Crouch.GetTag();
	
	if ( ASC->HasMatchingGameplayTag(CrouchStateTag))
	{
		FGameplayTagContainer WithTags;
		WithTags.AddTag(CrouchTag);
		ASC->CancelAbilities(&WithTags);
	}
	else
	{
		if (ASC && CrouchTag.IsValid())
		{
			ASC->TryActivateAbilitiesByTag(CrouchTag.GetSingleTagContainer(), /*bAllowRemoteActivation=*/true);
		}
	}
	const float Delay = ASC->HasMatchingGameplayTag(CrouchStateTag) ? 0.62f : 0.65f;
	
	GetWorld()->GetTimerManager().SetTimer(CrouchTimerHandle, this, &ATSCharacter::UnlockCrouchToggle, Delay, false);
}

void ATSCharacter::OnSprintStarted(const struct FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("shift pressed"));
	if (!ASC) return;
	const FGameplayTag CrouchStateTag = AbilityTags::TAG_State_Move_Crouch.GetTag();
	const FGameplayTag CrouchAbilityTag = AbilityTags::TAG_Ability_Move_Crouch.GetTag(); 
	const FGameplayTag SprintTag = AbilityTags::TAG_Ability_Move_Sprint.GetTag(); 

	
	if (ASC->HasMatchingGameplayTag(CrouchStateTag))
	{
		// 앉기 어빌리티 취소 -> GA_Crouch 종료 -> EndAbility 호출
		FGameplayTagContainer CancelTags;
		CancelTags.AddTag(CrouchAbilityTag);
		ASC->CancelAbilities(&CancelTags);
		
		return;
	}
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
	
	// 1. 내가 다운 || 죽음이면 못함
	if (IsDowned() || IsDead())
	{
		return;
	}
	ATSCharacter* ReviveTarget = DetectReviveTarget();
    
	if (ReviveTarget)
	{
		// 친구를 찾았고 + 기절 상태라면?
		ServerStartRevive(ReviveTarget);
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
	//if (NowPingType == ETSPingType::LOCATION || NowPingType == ETSPingType::HELP)
	//{
		// 내 위치 || 도움 -> 캐릭터 위치에 핑 찍기
		//SpawnLocation = GetActorLocation();
		
	//} else
	//{
	// 위험 || 발견 알림이면 라인 트레이스 -> 힛 부분에 찍기
	// 라인트레이스 쏘고 힛 부분 or 끝부분에 핑 찍기
	if (!IsValid(GEngine) || !IsValid(GEngine->GameViewport)) return;
	FVector2D ViewPortSize = FVector2D::ZeroVector;
	GEngine->GameViewport->GetViewportSize(ViewPortSize);
	const FVector2D ViewportCenter = ViewPortSize / 2.f;
		
	FVector TraceStart;
	FVector Forward;
	bool bDeprojectSuccess = UGameplayStatics::DeprojectScreenToWorld(
		PC,              // GetFirstPlayerController 대신 PC 사용 (더 안전)
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
		
	//}
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
	ServerSendHotKeyEvent(0); //1번키 = 0번 슬롯!!!
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
	ServerSendHotKeyEvent(9); // 0번키 = 9번 슬롯 !!!!!!!!
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
	// 착지 사운드
	if (FootstepComponent)
	{
		FootstepComponent->PlayFootstepSoundFromHit(Hit);
	}
	if (!HasAuthority() || !ASC )
	{
		return;
	}
	// v=g*t니까 낙하속도가 1000이면 대충 1초 지났을 때부터 데미지 받음
	// h=1/2 * g * t^2이니까 0.5*10(대충)*1*1 =5미터부터 데미지 받음 -> 너무 빡세면 낙하 기준 속도 올리면 됨 
	float FallSpeed = FallingSpeed_Z * (-1.0); //낙하속도는 음수라 -1 곱하기 
	float StandSpeed = 1000.0f; // 데미지 판정 기준 : 속도 1000 이상일때만 데미지 닳도록
	if ( FallSpeed < StandSpeed)
	{
		return; // 살살 떨어졌으니 데미지 없음
	}
	float FallDamage = (FallSpeed - StandSpeed) * 0.1; //데미지 공식 아직 미정
	
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
	// 오직 로컬 플레이어 컨트롤러에서만 실행
	APlayerController* PC = Cast<APlayerController>(Controller);
	if (!IsValid(PC) || !PC->IsLocalController()) return;
	
	// 화면 중앙 위치 가져오기
	if (!IsValid(GEngine) || !IsValid(GEngine->GameViewport)) return;
	
	FVector2D ViewPortSize = FVector2D::ZeroVector;
	GEngine->GameViewport->GetViewportSize(ViewPortSize);
	const FVector2D ViewportCenter = ViewPortSize / 2.f;
	
	FVector TraceStart; // 시작 점 
	FVector Forward;
	
	// "화면 중앙 픽셀"이 실제 3D 세계에서 어느 방향을 가리키는가? -> 성공 여부 반환
	bool DeprojectScreenToWorld= UGameplayStatics::DeprojectScreenToWorld(GetWorld()->GetFirstPlayerController(), ViewportCenter, TraceStart, Forward);
	if (!DeprojectScreenToWorld) return;
	
	// 끝 위치
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
	
	// 같은 걸 바라보면 아무것도 하지 않음.
	if (CurrentHitActor == LastHitActor)
	{
		if (IsValid(CurrentHitActor.Get()))
		{
			if (CurrentHitActor->Implements<UIInteraction>())
			{
				IIInteraction* InteractionInterface = Cast<IIInteraction>(CurrentHitActor);
				if (InteractionInterface && !InteractionInterface->CanInteract(this))
				{
					InteractionInterface->HideInteractionWidget();
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
			if (InteractionInterface)
			{
				InteractionInterface->HideInteractionWidget();
				OnReticleInteractionEnd.Broadcast();
			}
		}
		// 자원원천, 클라이밍 레티클 상호작용 끝내기
		else if(LastHitActor->IsA(ATSResourceBaseActor::StaticClass()) || LastHitActor->ActorHasTag("Climbable"))
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
		// 자원원천, 클라이밍 레티클 상호작용 시작
		else if(CurrentHitActor->IsA(ATSResourceBaseActor::StaticClass()) || CurrentHitActor->ActorHasTag("Climbable"))
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
	if (IsValid(ASC) && ASC->HasMatchingGameplayTag(AbilityTags::TAG_State_Status_Downed))
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

	//페이로드 데이터를 사용하여 해당 액터에 대한 능력을 트리거하는데 사용하는 함수. 저는 페이로드라고 안쓰고 EventData로 사용함
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
	if (bIsSwitchingShoulder && SpringArmComponent)
	{
		ShoulderSwitchElapsed += DeltaTime;

		if (ShoulderSwitchElapsed > ShoulderSwitchDuration)
		{
			ShoulderSwitchElapsed = ShoulderSwitchDuration;
		}
		float Alpha = ShoulderSwitchElapsed / ShoulderSwitchDuration;
		
		const FVector NewLoc = FMath::Lerp(ShoulderStartOffset, ShoulderTargetOffset, Alpha);
		SpringArmComponent->SetRelativeLocation(NewLoc);

		if (ShoulderSwitchElapsed >= ShoulderSwitchDuration)
		{
			bIsSwitchingShoulder = false;
		}
	}
	
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
	UpdateCrouchCamera();
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
		//LyingDown 삭제
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
		UE_LOG(LogTemp, Error, TEXT("[Multicast] 유효하지 않은 몽타주!"));
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
		UE_LOG(LogTemp, Error, TEXT("[Multicast] AnimInstance 상태 이상!"));
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
			UE_LOG(LogTSCharacter, Warning, TEXT("[Client] 몽타주 시작 지연 과다 (경과=%.2fs, 길이=%.2fs) - 재생 스킵"),
				Elapsed, MontageLength);
			return; // 재생 스킵
#endif
		}
		
		// 5. 지연된 시간만큼 건너뛰기
		StartPosition = Elapsed;

#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
		UE_LOG(LogTSCharacter, Log, TEXT("[Client] 네트워크 지연 보정: %.2f초 건너뛰고 재생 (전체 %.2fs)"),
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
		UE_LOG(LogTemp, Log, TEXT("[Multicast] 몽타주 재생: %s (Role: %s, StartPos: %.2fs)"),
			*Montage->GetName(),
			HasAuthority() ? TEXT("Server") : TEXT("Client"),
			StartPosition);
#endif
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[Multicast] 몽타주 재생 실패: %s"), *Montage->GetName());
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
		UE_LOG(LogTSCharacter, Log, TEXT("[Multicast] 몽타주 정지: %s (Role: %s)"),
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