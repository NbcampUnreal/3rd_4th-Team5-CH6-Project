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
#include "Kismet/KismetMathLibrary.h"
#include "GAS/AttributeSet/TSAttributeSet.h"
#include "Item/Interface/IInteraction.h"
#include "GameplayTags/AbilityGameplayTags.h"
// 테스트 코드

#include "Components/InstancedStaticMeshComponent.h"
#include "Item/System/WorldItemInstanceSubsystem.h"
#include "Item/System/WorldItemPoolSubsystem.h"
#include "Item/WorldItem.h"
#include "Item/System/TSItemPoolActor.h"
// 테스트 코드

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
}

void ATSCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	InitAbilitySystem();
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


	if (IsValid(ASC) && IsValid(Attributes))
	{
		ASC->GetGameplayAttributeValueChangeDelegate(
			UTSAttributeSet::GetMoveSpeedAttribute()
		).AddUObject(this, &ATSCharacter::OnMoveSpeedChanged);
		
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
		// ■ 추적: Hunger 변경 추적 (서버에서만 로그)
		ASC->GetGameplayAttributeValueChangeDelegate(
			UTSAttributeSet::GetHungerAttribute()
		).AddLambda([this](const FOnAttributeChangeData& Data)
		{
			if (HasAuthority())
			{
				UE_LOG(LogTemp, Warning, TEXT("[서버] Hunger 변경: %.2f -> %.2f"), Data.OldValue, Data.NewValue);
			}
		});
		
		// ■ 추적: Thirst 변경 추적 (서버에서만 로그)
		ASC->GetGameplayAttributeValueChangeDelegate(
			UTSAttributeSet::GetThirstAttribute()
		).AddLambda([this](const FOnAttributeChangeData& Data)
		{
			if (HasAuthority())
			{
				UE_LOG(LogTemp, Warning, TEXT("[서버] Thirst 변경: %.2f -> %.2f"), Data.OldValue, Data.NewValue);
			}
		});
		
		// ■ 추적: Health 변경 추적 (서버에서만 로그)
		ASC->GetGameplayAttributeValueChangeDelegate(
			UTSAttributeSet::GetHealthAttribute()
		).AddLambda([this](const FOnAttributeChangeData& Data)
		{
			if (HasAuthority())
			{
				UE_LOG(LogTemp, Warning, TEXT("[서버] Health 변경: %.2f -> %.2f"), Data.OldValue, Data.NewValue);
			}
		});
		
		// ■ 추적: Sanity 변경 추적 (서버에서만 로그)
		ASC->GetGameplayAttributeValueChangeDelegate(
			UTSAttributeSet::GetSanityAttribute()
		).AddLambda([this](const FOnAttributeChangeData& Data)
		{
			if (HasAuthority())
			{
				UE_LOG(LogTemp, Warning, TEXT("[서버] Sanity 변경: %.2f -> %.2f"), Data.OldValue, Data.NewValue);
			}
		});
		
		// ■ 추적: Temperature 변경 추적 (서버에서만 로그)
		ASC->GetGameplayAttributeValueChangeDelegate(
			UTSAttributeSet::GetTemperatureAttribute()
		).AddLambda([this](const FOnAttributeChangeData& Data)
		{
			if (HasAuthority())
			{
				UE_LOG(LogTemp, Warning, TEXT("[서버] Temperature 변경: %.2f -> %.2f"), Data.OldValue, Data.NewValue);
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
				UE_LOG(LogTemp, Warning, TEXT("태그 적용: %s"), *CallbackTag.ToString());
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("태그 제거: %s"), *CallbackTag.ToString());
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
				UE_LOG(LogTemp, Warning, TEXT("태그 적용: %s"), *CallbackTag.ToString());
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("태그 제거: %s"), *CallbackTag.ToString());
			}
		});
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
	GiveByTag(AbilityTags::TAG_Ability_Interact_LeftClick.GetTag());
	GiveByTag(AbilityTags::TAG_Ability_Interact_RightClick.GetTag());

	// HotKey
	GiveByTag(AbilityTags::TAG_Ability_HotKey.GetTag());
	
}

void ATSCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (SpringArmComponent)
	{
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
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	// TODO: 독 상태이상 디버깅 용도 - 테스트 종료시 반드시 제거
	// 아직은 테스트 중이라 적용 상태로 올림
	// 초기 독 상태 태그 추가
	if (!ASC) return;
	ASC->AddLooseGameplayTag(AbilityTags::TAG_State_Status_Poison);
	UE_LOG(LogTemp, Error, TEXT("초기 독 상태 태그 적용!"));
#endif
}

void ATSCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D Input = Value.Get<FVector2D>();
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
		AddMovementInput(Forward, Input.Y);

	if (!FMath::IsNearlyZero(Input.X))
		AddMovementInput(Right, Input.X);
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

//LyingDown 삭제 

void ATSCharacter::OnOpenBag(const struct FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("i pressed Open Bag"));
	if (!IsLocallyControlled()) return;
	if (ATSPlayerController* TSController = Cast<ATSPlayerController>(Controller))
	{
		TSController -> ToggleInventory();
	}
}

void ATSCharacter::OnBuild(const struct FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("b pressed"));
	const FGameplayTag BuildTag = AbilityTags::TAG_Ability_Interact_Build.GetTag();
	if (ASC && BuildTag.IsValid())
	{
		ASC->TryActivateAbilitiesByTag(BuildTag.GetSingleTagContainer(), /*bAllowRemoteActivation=*/true);
	}
}

void ATSCharacter::OnInteract(const struct FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("e pressed"));

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
}

void ATSCharacter::OnLeftClick(const struct FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("l-click pressed"));
	const FGameplayTag LeftClickTag = AbilityTags::TAG_Ability_Interact_LeftClick.GetTag();
	if (ASC && LeftClickTag.IsValid())
	{
		ASC->TryActivateAbilitiesByTag(LeftClickTag.GetSingleTagContainer(), /*bAllowRemoteActivation=*/true);
	}
}

void ATSCharacter::OnRightClick(const struct FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("r-click pressed"));
	ServerSendUseItemEvent();
	const FGameplayTag RightClickTag = AbilityTags::TAG_Ability_Interact_RightClick.GetTag();
	if (ASC && RightClickTag.IsValid())
	{
		ASC->TryActivateAbilitiesByTag(RightClickTag.GetSingleTagContainer(), /*bAllowRemoteActivation=*/true);
	}
}

void ATSCharacter::OnPing(const struct FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("wheel pressed"));
	const FGameplayTag PingTag = AbilityTags::TAG_Ability_Interact_Ping.GetTag();
	if (ASC && PingTag.IsValid())
	{
		ASC->TryActivateAbilitiesByTag(PingTag.GetSingleTagContainer(), /*bAllowRemoteActivation=*/true);
	}
}

void ATSCharacter::OnWheelScroll(const struct FInputActionValue& Vaule)
{
	UE_LOG(LogTemp, Log, TEXT("wheel scroll pressed"));
	// 가스 안쓸거임
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
	
	GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, TraceChannel);
	LastHitActor = CurrentHitActor;
	CurrentHitActor = HitResult.GetActor();
	
	GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_GameTraceChannel1);
	DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Green, false, 1.f, 0, 1.f);
	if (HitResult.bBlockingHit)
	{
		DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 8.f, 12, FColor::Red, false, 1.f);
	}
	// 같은 걸 바라보면 아무것도 하지 않음.
	if (CurrentHitActor == LastHitActor) return;

	// 이전 액터 처리
	if (IsValid(LastHitActor.Get()))
	{
		if (LastHitActor->Implements<UIInteraction>())
		{
			IIInteraction* InteractionInterface = Cast<IIInteraction>(LastHitActor);
			if (InteractionInterface)
			{
				InteractionInterface->HideInteractionWidget();
			}
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
			}
		}
	}
}

bool ATSCharacter::IsClimbing()
{
	//State.Move.Climb 태그가 있으면 -> 클라이밍 중
	return ASC->HasMatchingGameplayTag(AbilityTags::TAG_State_Move_Climb.GetTag());
}

void ATSCharacter::ServerSendHotKeyEvent_Implementation(int HotKeyIndex)
{
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
			InteractionInterface->Interact(this);
		}
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
		//LyingDown 삭제
		EnhancedInputComponent->BindAction(InputDataAsset->OpenBagAction, ETriggerEvent::Started, this,
									   &ATSCharacter::OnOpenBag);
		EnhancedInputComponent->BindAction(InputDataAsset->BuildAction, ETriggerEvent::Started, this,
		                                   &ATSCharacter::OnBuild);
		EnhancedInputComponent->BindAction(InputDataAsset->InteractAction, ETriggerEvent::Started, this,
		                                   &ATSCharacter::OnInteract);
		EnhancedInputComponent->BindAction(InputDataAsset->PingAction, ETriggerEvent::Started, this,
		                                   &ATSCharacter::OnPing);
		EnhancedInputComponent->BindAction(InputDataAsset->WheelScrollAction, ETriggerEvent::Started, this,
		                                   &ATSCharacter::OnWheelScroll);
		EnhancedInputComponent->BindAction(InputDataAsset->LeftClickAction, ETriggerEvent::Started, this,
		                                   &ATSCharacter::OnLeftClick);
		EnhancedInputComponent->BindAction(InputDataAsset->RightClickAction, ETriggerEvent::Started, this,
		                                   &ATSCharacter::OnRightClick);
		EnhancedInputComponent->BindAction(InputDataAsset->SprintAction, ETriggerEvent::Started, this,
		                                   &ATSCharacter::OnSprintStarted);
		EnhancedInputComponent->BindAction(InputDataAsset->SprintAction, ETriggerEvent::Completed, this,
		                                   &ATSCharacter::OnSprintCompleted);

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
		
		// 테스트 코드
		// [TEST] 디버그 키 바인딩 추가
		UE_LOG(LogTemp, Warning, TEXT("[Input Info] Trying to bind Debug Actions..."));
		if (InputDataAsset->DebugDropAction)
		{
			EnhancedInputComponent->BindAction(InputDataAsset->DebugDropAction, ETriggerEvent::Started, this, &ATSCharacter::OnDebugDrop);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[Input Error] DebugDropAction is NULL in DataAsset!"));
		}
       
		if (InputDataAsset->DebugRemoveAction)
		{
			EnhancedInputComponent->BindAction(InputDataAsset->DebugRemoveAction, ETriggerEvent::Started, this, &ATSCharacter::OnDebugRemove);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[Input Error] DebugRemoveAction is NULL in DataAsset!"));
		}
		// 테스트 코드
	}
}


// 테스트 코드

// === [TEST] 입력 핸들러 ===
void ATSCharacter::OnDebugDrop(const FInputActionValue& Value)
{
    Server_DebugDropItem();
}

void ATSCharacter::OnDebugRemove(const FInputActionValue& Value)
{
    Server_DebugRemoveItem();
}

// === [TEST] 서버: 아이템 드랍 ===
void ATSCharacter::Server_DebugDropItem_Implementation()
{
    UWorld* World = GetWorld();
    if (!World) return;

    auto* PoolSys = World->GetSubsystem<UWorldItemPoolSubsystem>();
    if (!PoolSys)
    {
        UE_LOG(LogTemp, Warning, TEXT("WorldItemPoolSubsystem Not Found"));
        return;
    }

    // 테스트 아이템 데이터 (ID: 1번 아이템)
    FSlotStructMaster TestItem;
    TestItem.ItemData.StaticDataID = 2; 
    // 생성 시간 초기화
    TestItem.ItemData.CreationServerTime = World->GetTimeSeconds();

    // 캐릭터 앞 1.5m, 바닥 쪽으로 위치 설정
    FVector Forward = GetActorForwardVector();
    FVector SpawnLoc = GetActorLocation() + (Forward * 150.0f) + FVector(0, 0, 50.0f);
    FTransform SpawnTrans(SpawnLoc);

    // 드랍 요청
    PoolSys->DropItem(TestItem, SpawnTrans, GetActorLocation());

    UE_LOG(LogTemp, Log, TEXT("[Debug] Character Dropped Item ID: 1"));
}

// === [TEST] 서버: 아이템 삭제 ===
void ATSCharacter::Server_DebugRemoveItem_Implementation()
{
    UWorld* World = GetWorld();
    if (!World) return;

    // 카메라 시선 방향으로 레이캐스트 준비
    FVector StartLoc;
    FRotator CamRot;
    
    if (Controller)
    {
        Controller->GetPlayerViewPoint(StartLoc, CamRot);
    }
    else
    {
        StartLoc = GetActorLocation();
        CamRot = GetActorRotation();
    }

    FVector EndLoc = StartLoc + (CamRot.Vector() * 2000.0f); // 20m 거리

    FHitResult Hit;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);

    // 판정 범위 1m 스피어 스윕
    FCollisionShape SphereShape = FCollisionShape::MakeSphere(100.0f); 

    bool bHit = World->SweepSingleByChannel(Hit, StartLoc, EndLoc, FQuat::Identity, ECC_Visibility, SphereShape, Params);
    
    if (bHit)
    {
        // 가까이 있는 액터 -> 풀 반납
        if (AWorldItem* HitItem = Cast<AWorldItem>(Hit.GetActor()))
        {
            if (auto* PoolSys = World->GetSubsystem<UWorldItemPoolSubsystem>())
            {
                PoolSys->ReleaseItemActor(HitItem);
                UE_LOG(LogTemp, Log, TEXT("[Debug] Removed Actor Item: %s"), *HitItem->GetName());
            }
        }
        // 멀리 있는 인스턴스 -> 숨김 처리 및 데이터 삭제
        else if (UInstancedStaticMeshComponent* ISM = Cast<UInstancedStaticMeshComponent>(Hit.GetComponent()))
        {
            int32 InstanceIndex = Hit.Item;

            if (InstanceIndex != -1)
            {
                auto* InstanceSys = World->GetSubsystem<UWorldItemInstanceSubsystem>();
                if (!InstanceSys) return;

                // 어떤 아이템인지 데이터 확인
                FSlotStructMaster FoundData;
                if (InstanceSys->GetItemDataByInstanceIndex(InstanceIndex, FoundData))
                {
                    int32 TargetItemID = FoundData.ItemData.StaticDataID;

                    // 서버 데이터 영구 삭제 (데이터 맵에서 제거)
                    InstanceSys->RemoveInstancePermanent(InstanceIndex);

                    // 클라이언트들에게 시각적 동기화 (숨김) 요청
                    AActor* PoolActor = UGameplayStatics::GetActorOfClass(this, ATSItemPoolActor::StaticClass());
                    if (ATSItemPoolActor* TSPool = Cast<ATSItemPoolActor>(PoolActor))
                    {
                        TSPool->Multicast_SetInstanceVisible(InstanceIndex, TargetItemID, false, FTransform::Identity);
                        
                        UE_LOG(LogTemp, Log, TEXT("[Debug] Permanent Remove Instance Index: %d / ID: %d"), InstanceIndex, TargetItemID);
                    }
                }
                else
                {
                    UE_LOG(LogTemp, Warning, TEXT("[Debug] Failed to find data for Instance Index: %d (Already removed?)"), InstanceIndex);
                }
            }
        }
    }
}
// 테스트 코드