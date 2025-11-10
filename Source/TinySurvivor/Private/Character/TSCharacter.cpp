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

ATSCharacter::ATSCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent> (TEXT("Spring Arm Component"));
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

	Attributes = PS -> GetAttributeSet();
	if (!Attributes)
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerState Attributes is NULL."));
		return;
	}
	ASC->InitAbilityActorInfo(PS, this);
}
void ATSCharacter::InitializeAbilities()
{
	//매니저를 통해 어빌리티 부여 하는 함수
	if (!HasAuthority() || !ASC)
	{
		return;
	}
	if (!ASC)
	{
		return;
	}

	// GAS
	
	/*const FGameplayTag JumpOrClimbTag = FGameplayTag::RequestGameplayTag(FName("Ability.Move.JumpOrClimb"));
	UTSAbilityManagerSubSystem::GetAbilityManager(this)->GiveAbilityByTag(ASC,JumpOrClimbTag);
	*/
	const FGameplayTag RollTag= FGameplayTag::RequestGameplayTag(FName("Ability.Move.Roll"));
	UTSAbilityManagerSubSystem::GetAbilityManager(this)->GiveAbilityByTag(ASC,RollTag);
	/*const FGameplayTag SprintTag= FGameplayTag::RequestGameplayTag(FName("Ability.Move.Sprint"));
	UTSAbilityManagerSubSystem::GetAbilityManager(this)->GiveAbilityByTag(ASC,SprintTag);
	const FGameplayTag LyingDownTag= FGameplayTag::RequestGameplayTag(FName("Ability.Move.LyingDown"));
	UTSAbilityManagerSubSystem::GetAbilityManager(this)->GiveAbilityByTag(ASC,LyingDownTag);
	const FGameplayTag BuildTag= FGameplayTag::RequestGameplayTag(FName("Ability.Interact.Build"));
	UTSAbilityManagerSubSystem::GetAbilityManager(this)->GiveAbilityByTag(ASC,BuildTag);
	const FGameplayTag InteractTag= FGameplayTag::RequestGameplayTag(FName("Ability.Interact.Interact"));
	UTSAbilityManagerSubSystem::GetAbilityManager(this)->GiveAbilityByTag(ASC,InteractTag);
	const FGameplayTag PingTag= FGameplayTag::RequestGameplayTag(FName("Ability.Interact.Ping"));
	UTSAbilityManagerSubSystem::GetAbilityManager(this)->GiveAbilityByTag(ASC,PingTag);
	const FGameplayTag PingTag= FGameplayTag::RequestGameplayTag(FName("Ability.Interact.WheelScroll"));
	UTSAbilityManagerSubSystem::GetAbilityManager(this)->GiveAbilityByTag(ASC,WheelScrollTag);
	const FGameplayTag LeftClickTag= FGameplayTag::RequestGameplayTag(FName("Ability.Interact.LeftClick"));
	UTSAbilityManagerSubSystem::GetAbilityManager(this)->GiveAbilityByTag(ASC,LeftClickTag);
	const FGameplayTag RightClickTag= FGameplayTag::RequestGameplayTag(FName("Ability.Interact.RightClick"));
	UTSAbilityManagerSubSystem::GetAbilityManager(this)->GiveAbilityByTag(ASC,RightClickTag);
	*/

	const FGameplayTag HotKeyTag = FGameplayTag::RequestGameplayTag(FName("Ability.HotKey"));
	if (HotKeyTag.IsValid())
	{
		UTSAbilityManagerSubSystem::GetAbilityManager(this)->GiveAbilityByTag(ASC, HotKeyTag);
	}



}

void ATSCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>( PlayerController -> GetLocalPlayer()))
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

		if (ATSPlayerController* PS = Cast<ATSPlayerController>(PlayerController))
		{
			PS -> InitializePlayerHUD(this);
		}
	}

}
void ATSCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D Input = Value.Get<FVector2D>();
	const FRotator ControlRot = Controller->GetControlRotation();
	const FRotator YawRot(0.f, ControlRot.Yaw, 0.f);
	const FVector Forward = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
	const FVector Right   = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);

	if (!FMath::IsNearlyZero(Input.Y))
		AddMovementInput(Forward, Input.Y); 

	if (!FMath::IsNearlyZero(Input.X))
		AddMovementInput(Right,   Input.X);
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
	
}
//---- Non GAS

//GAS
void ATSCharacter::OnJumpOrClimb(const struct FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("SpaceBar pressed"));
}
void ATSCharacter::OnRoll(const struct FInputActionValue& Value)
{
	const FGameplayTag RollTag= FGameplayTag::RequestGameplayTag(FName("Ability.Move.Roll"));
	if (ASC && RollTag.IsValid())
	{
		ASC->TryActivateAbilitiesByTag(RollTag.GetSingleTagContainer(), /*bAllowRemoteActivation=*/true);
	}
}
void ATSCharacter::OnCrouch(const struct FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("c pressed"));
}

void ATSCharacter::OnSprintStarted(const struct FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("shift pressed"));
}
void ATSCharacter::OnSprintCompleted(const struct FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("shift end"));
}
void ATSCharacter::OnLyingDown(const struct FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("z pressed"));
}
void ATSCharacter::OnBuild(const struct FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("b pressed"));
}
void ATSCharacter::OnInteract(const struct FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("i pressed"));
}
void ATSCharacter::OnLeftClick(const struct FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("l-click pressed"));
}
void ATSCharacter::OnRightClick(const struct FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("r-click pressed"));
}
void ATSCharacter::OnPing(const struct FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("wheel pressed"));
}
void ATSCharacter::OnWheelScroll(const struct FInputActionValue& Vaule)
{
	UE_LOG(LogTemp, Log, TEXT("wheel scroll pressed"));
}

void ATSCharacter::OnHotKey1(const struct FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("1 pressed"));
	SendHotKeyEvent(0); //1번키 = 0번 슬롯!!!
}
void ATSCharacter::OnHotKey2(const struct FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("2 pressed"));
	SendHotKeyEvent(1);
}
void ATSCharacter::OnHotKey3(const struct FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("3 pressed"));
	SendHotKeyEvent(2);
}
void ATSCharacter::OnHotKey4(const struct FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("4 pressed"));
	SendHotKeyEvent(3);
}
void ATSCharacter::OnHotKey5(const struct FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("5 pressed"));
	SendHotKeyEvent(4);
}
void ATSCharacter::OnHotKey6(const struct FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("6 pressed"));
	SendHotKeyEvent(5);
}
void ATSCharacter::OnHotKey7(const struct FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("7 pressed"));
	SendHotKeyEvent(6);
	
}
void ATSCharacter::OnHotKey8(const struct FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("8 pressed"));
	SendHotKeyEvent(7);

}
void ATSCharacter::OnHotKey9(const struct FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("9 pressed"));
	SendHotKeyEvent(8);
}
void ATSCharacter::OnHotKey0(const struct FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("0 pressed"));
	SendHotKeyEvent(9); // 0번키 = 9번 슬롯 !!!!!!!!

}
void ATSCharacter::SendHotKeyEvent(int HotKeyIndex)
{
	if (!ASC) return;
	const FGameplayTag EventTag = FGameplayTag::RequestGameplayTag(FName("Input.HotKey"));
	FGameplayEventData EventData;
	EventData.EventTag = EventTag;
	EventData.EventMagnitude = static_cast<float>(HotKeyIndex); 
    
	EventData.Instigator = this;
	EventData.Target = this;
	
	//페이로드 데이터를 사용하여 해당 액터에 대한 능력을 트리거하는데 사용하는 함수. 저는 페이로드라고 안쓰고 EventData로 사용함
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this/*Actor*/,EventTag,EventData/*Payload*/);
}

void ATSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
void ATSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (!InputDataAsset) return;
	
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent -> BindAction(InputDataAsset->MoveAction, ETriggerEvent::Triggered, this, &ATSCharacter::Move);
		EnhancedInputComponent -> BindAction(InputDataAsset->LookAction, ETriggerEvent::Triggered, this, &ATSCharacter::Look);
		EnhancedInputComponent->BindAction(InputDataAsset->ShoulderSwitchAction, ETriggerEvent::Started, this, &ATSCharacter::ShoulderSwitch);
		
		EnhancedInputComponent->BindAction(InputDataAsset->JumpOrClimbAction, ETriggerEvent::Started, this, &ATSCharacter::OnJumpOrClimb);
		EnhancedInputComponent->BindAction(InputDataAsset->RollAction, ETriggerEvent::Started, this, &ATSCharacter::OnRoll);
		EnhancedInputComponent->BindAction(InputDataAsset->LyingDownAction, ETriggerEvent::Started, this, &ATSCharacter::OnLyingDown);
		EnhancedInputComponent->BindAction(InputDataAsset->BuildAction, ETriggerEvent::Started, this, &ATSCharacter::OnBuild);
		EnhancedInputComponent->BindAction(InputDataAsset->InteractAction, ETriggerEvent::Started, this, &ATSCharacter::OnInteract);
		EnhancedInputComponent->BindAction(InputDataAsset->PingAction, ETriggerEvent::Started, this, &ATSCharacter::OnPing);
		EnhancedInputComponent->BindAction(InputDataAsset->WheelScrollAction, ETriggerEvent::Started, this, &ATSCharacter::OnWheelScroll);
		EnhancedInputComponent->BindAction(InputDataAsset->LeftClickAction, ETriggerEvent::Started, this, &ATSCharacter::OnLeftClick);
		EnhancedInputComponent->BindAction(InputDataAsset->RightClickAction, ETriggerEvent::Started, this, &ATSCharacter::OnRightClick);
		EnhancedInputComponent->BindAction(InputDataAsset->SprintAction, ETriggerEvent::Started, this, &ATSCharacter::OnSprintStarted);
		EnhancedInputComponent->BindAction(InputDataAsset->SprintAction, ETriggerEvent::Completed, this, &ATSCharacter::OnSprintCompleted);
		
		EnhancedInputComponent->BindAction(InputDataAsset->HotKey1Action, ETriggerEvent::Started, this, &ATSCharacter::OnHotKey1);
		EnhancedInputComponent->BindAction(InputDataAsset->HotKey2Action, ETriggerEvent::Started, this, &ATSCharacter::OnHotKey2);
		EnhancedInputComponent->BindAction(InputDataAsset->HotKey3Action, ETriggerEvent::Started, this, &ATSCharacter::OnHotKey3);
		EnhancedInputComponent->BindAction(InputDataAsset->HotKey4Action, ETriggerEvent::Started, this, &ATSCharacter::OnHotKey4);
		EnhancedInputComponent->BindAction(InputDataAsset->HotKey5Action, ETriggerEvent::Started, this, &ATSCharacter::OnHotKey5);
		EnhancedInputComponent->BindAction(InputDataAsset->HotKey6Action, ETriggerEvent::Started, this, &ATSCharacter::OnHotKey6);
		EnhancedInputComponent->BindAction(InputDataAsset->HotKey7Action, ETriggerEvent::Started, this, &ATSCharacter::OnHotKey7);
		EnhancedInputComponent->BindAction(InputDataAsset->HotKey8Action, ETriggerEvent::Started, this, &ATSCharacter::OnHotKey8);
		EnhancedInputComponent->BindAction(InputDataAsset->HotKey9Action, ETriggerEvent::Started, this, &ATSCharacter::OnHotKey9);
		EnhancedInputComponent->BindAction(InputDataAsset->HotKey0Action, ETriggerEvent::Started, this, &ATSCharacter::OnHotKey0);
	}
}



