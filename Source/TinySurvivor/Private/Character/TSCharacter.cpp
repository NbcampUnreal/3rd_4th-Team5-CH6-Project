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
	auto GiveByTag = [Manager, this](const TCHAR* TagName)
	{
		const FGameplayTag GameplayTag= FGameplayTag::RequestGameplayTag(FName(TagName));
		if (GameplayTag.IsValid())
		{
			Manager->GiveAbilityByTag(ASC, GameplayTag);
		} 
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("%s 태그 없음"),TagName);
		}
	};
	
	// MOVE
	GiveByTag(TEXT("Ability.Move.JumpOrClimb"));
	GiveByTag(TEXT("Ability.Move.Roll"));
	GiveByTag(TEXT("Ability.Move.Sprint"));
	GiveByTag(TEXT("Ability.Move.Crouch"));
	
	// Interact
	GiveByTag(TEXT("Ability.Interact.Build"));
	GiveByTag(TEXT("Ability.Interact.Interact"));
	GiveByTag(TEXT("Ability.Interact.Ping"));
	GiveByTag(TEXT("Ability.Interact.LeftClick"));
	GiveByTag(TEXT("Ability.Interact.RightClick"));

	const FGameplayTag HotKeyTag = FGameplayTag::RequestGameplayTag(FName("Ability.HotKey"));
	if (HotKeyTag.IsValid())
	{
		Manager->GiveAbilityByTag(ASC, HotKeyTag);
	}
	
	
}

void ATSCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (SpringArmComponent)
	{
		SpringArmBaseLocation = SpringArmComponent->GetRelativeLocation();
		SpringArmRightLocation = SpringArmBaseLocation +FVector (0.f, RightShoulderOffset, 0.f); // +40
		SpringArmLeftLocation  = SpringArmBaseLocation + FVector(0.f, LeftShoulderOffset, 0.f);  // -80

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

void ATSCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D Input = Value.Get<FVector2D>();
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
void ATSCharacter::OnJumpOrClimb(const struct FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("SpaceBar pressed"));
	const FGameplayTag JumpOrClimbTag = FGameplayTag::RequestGameplayTag(FName("Ability.Move.JumpOrClimb"));
	if (ASC && JumpOrClimbTag.IsValid())
	{
		ASC->TryActivateAbilitiesByTag(JumpOrClimbTag.GetSingleTagContainer(), /*bAllowRemoteActivation=*/true);
	}
}

void ATSCharacter::OnRoll(const struct FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("ctrl pressed"));
	const FGameplayTag RollTag = FGameplayTag::RequestGameplayTag(FName("Ability.Move.Roll"));
	if (ASC && RollTag.IsValid())
	{
		ASC->TryActivateAbilitiesByTag(RollTag.GetSingleTagContainer(), /*bAllowRemoteActivation=*/true);
	}
}

void ATSCharacter::OnCrouch(const struct FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("c pressed"));
	const FGameplayTag CrouchTag = FGameplayTag::RequestGameplayTag(FName("Ability.Move.Crouch"));
	if (ASC && CrouchTag.IsValid())
	{
		ASC->TryActivateAbilitiesByTag(CrouchTag.GetSingleTagContainer(), /*bAllowRemoteActivation=*/true);
	}
}

void ATSCharacter::OnSprintStarted(const struct FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("shift pressed"));
	const FGameplayTag SprintTag = FGameplayTag::RequestGameplayTag(FName("Ability.Move.Sprint"));
	if (ASC && SprintTag.IsValid())
	{
		ASC->TryActivateAbilitiesByTag(SprintTag.GetSingleTagContainer(), /*bAllowRemoteActivation=*/true);
	}
}

void ATSCharacter::OnSprintCompleted(const struct FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("shift end"));
	const FGameplayTag SprintTag = FGameplayTag::RequestGameplayTag(FName("Ability.Move.Sprint"));
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
	const FGameplayTag BuildTag = FGameplayTag::RequestGameplayTag(FName("Ability.Interact.Build"));
	if (ASC && BuildTag.IsValid())
	{
		ASC->TryActivateAbilitiesByTag(BuildTag.GetSingleTagContainer(), /*bAllowRemoteActivation=*/true);
	}
}

void ATSCharacter::OnInteract(const struct FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("e pressed"));
	const FGameplayTag InteractTag = FGameplayTag::RequestGameplayTag(FName("Ability.Interact.Interact"));
	if (ASC && InteractTag.IsValid())
	{
		ASC->TryActivateAbilitiesByTag(InteractTag.GetSingleTagContainer(), /*bAllowRemoteActivation=*/true);
	}
}

void ATSCharacter::OnLeftClick(const struct FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("l-click pressed"));
	const FGameplayTag LeftClickTag = FGameplayTag::RequestGameplayTag(FName("Ability.Interact.LeftClick"));
	if (ASC && LeftClickTag.IsValid())
	{
		ASC->TryActivateAbilitiesByTag(LeftClickTag.GetSingleTagContainer(), /*bAllowRemoteActivation=*/true);
	}
}

void ATSCharacter::OnRightClick(const struct FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("r-click pressed"));
	ServerSendUseItemEvent();
}

void ATSCharacter::OnPing(const struct FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("wheel pressed"));
	const FGameplayTag PingTag = FGameplayTag::RequestGameplayTag(FName("Ability.Interact.Ping"));
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
	
	/*
	 * 
	 * 
	 * 
	 * 아이템, door, wall 이런거 판정 로직 여기에 구현하면 될 듯
	 */
	
}

void ATSCharacter::ServerSendHotKeyEvent_Implementation(int HotKeyIndex)
{
	if (!ASC) return;
	const FGameplayTag EventTag = FGameplayTag::RequestGameplayTag(FName("Input.HotKey"));
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
		                                   &ATSCharacter::OnJumpOrClimb);
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