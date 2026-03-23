// All CopyRight From YulRyongGameStudio //


#include "A_FOR_INGAME/SECTION_PLAYER/Comp/TSPlayerInputActionComponent.h"
#include "SECTION_INPUT/InGame/TSInGamePlayerInputDataAsset.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "GameFramework/Character.h"


//======================================================================================================================	
#pragma region 라이프_사이클
	
	//━━━━━━━━━━━━━━━━━━━━
	// 라이프 사이클
	//━━━━━━━━━━━━━━━━━━━━	

UTSPlayerInputActionComponent::UTSPlayerInputActionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

#pragma endregion
//======================================================================================================================	
#pragma region 외부_API
	
	//━━━━━━━━━━━━━━━━━━━━
	// 외부_API
	//━━━━━━━━━━━━━━━━━━━━	
void UTSPlayerInputActionComponent::SetupPlayerInput(UEnhancedInputComponent* InEnhancedInputComponent)
{
	if (!IsValid(InEnhancedInputComponent)) return;
	if (!IsValid(InputData)) return;
	if (!IsValid(GetOwner())) return;
	
	OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (!IsValid(OwnerCharacter)) return;

	if (!IsValid(OwnerCharacter->GetController())) return;
	APlayerController* PC = Cast<APlayerController>(OwnerCharacter->GetController());
	if (!IsValid(PC)) return;
	
	UEnhancedInputLocalPlayerSubsystem* EISubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
	if (!IsValid(EISubsystem)) return;
	
	// DataAsset에 담아둔 기본 IMC를 등록
	if (IsValid(InputData->DefaultMovementIMC)) EISubsystem->AddMappingContext(InputData->DefaultMovementIMC, 0);
	
	
	// 1. Move & Look
	if (IsValid(InputData->IA_Move)) InEnhancedInputComponent->BindAction(InputData->IA_Move, ETriggerEvent::Triggered, this, &UTSPlayerInputActionComponent::Move_internal);
	if (IsValid(InputData->IA_Look)) InEnhancedInputComponent->BindAction(InputData->IA_Look, ETriggerEvent::Triggered, this, &UTSPlayerInputActionComponent::Look_internal);

	// 2. Jump
	if (IsValid(InputData->IA_Jump)) InEnhancedInputComponent->BindAction(InputData->IA_Jump, ETriggerEvent::Started, this, &UTSPlayerInputActionComponent::Jump_internal);

	// 3. Sprint
	if (IsValid(InputData->IA_Sprint)) InEnhancedInputComponent->BindAction(InputData->IA_Sprint, ETriggerEvent::Started, this, &UTSPlayerInputActionComponent::Sprint_internal);
	if (IsValid(InputData->IA_Walk)) InEnhancedInputComponent->BindAction(InputData->IA_Walk, ETriggerEvent::Started, this, &UTSPlayerInputActionComponent::Walk_internal);
}

#pragma endregion
//======================================================================================================================	
#pragma region 움직임_관련
	
	
	//━━━━━━━━━━━━━━━━━━━━
	// 움직임_관련
	//━━━━━━━━━━━━━━━━━━━━	

void UTSPlayerInputActionComponent::Move_internal(const FInputActionValue& Value)
{
	if (!IsValid(OwnerCharacter)) return;
	if (!IsValid(OwnerCharacter->GetController())) return;
	
	FVector2D MovementVector = Value.Get<FVector2D>();
	if(MovementVector.IsNearlyZero()) return;
	
	// 여기서 추후 태그 확인해서 막을 수 있음.
	
	const FRotator Rotation = OwnerCharacter->GetController()->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	OwnerCharacter->AddMovementInput(ForwardDirection, MovementVector.Y);
	OwnerCharacter->AddMovementInput(RightDirection, MovementVector.X);
}

void UTSPlayerInputActionComponent::Look_internal(const FInputActionValue& Value)
{
	if (!IsValid(OwnerCharacter)) return;
	if (!IsValid(OwnerCharacter->GetController())) return;
	
	FVector2D LookAxisVector = Value.Get<FVector2D>();
	if (LookAxisVector.IsNearlyZero()) return;

	// 여기서 추후 태그 확인해서 막을 수 있음.
	
	OwnerCharacter->AddControllerYawInput(LookAxisVector.X);
	OwnerCharacter->AddControllerPitchInput(LookAxisVector.Y);
}

void UTSPlayerInputActionComponent::Jump_internal(const FInputActionValue& Value)
{
	if (!IsValid(OwnerCharacter)) return;
	if (!IsValid(OwnerCharacter->GetController())) return;
}

void UTSPlayerInputActionComponent::Sprint_internal(const FInputActionValue& Value)
{
	if (!IsValid(OwnerCharacter)) return;
	if (!IsValid(OwnerCharacter->GetController())) return;
}

void UTSPlayerInputActionComponent::Walk_internal(const FInputActionValue& Value)
{
	if (!IsValid(OwnerCharacter)) return;
	if (!IsValid(OwnerCharacter->GetController())) return;
}

#pragma endregion
//======================================================================================================================