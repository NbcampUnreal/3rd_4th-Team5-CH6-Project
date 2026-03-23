// All CopyRight From YulRyongGameStudio //


#include "A_FOR_INGAME/SECTION_PLAYER/Comp/TSPlayerInputActionComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "SECTION_INPUT/InGame/TSInGamePlayerInputDataAsset.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "A_FOR_COMMON/GameplayMessage/Data/Struct/Invnetory/FTSActiveHotkeyMessageData.h"
#include "A_FOR_COMMON/GameplayMessage/Setting/TSGameplayChannelSetting.h"
#include "A_FOR_COMMON/Tag/Interact/PossibleInteractTag.h"
#include "GameFramework/Character.h"
#include "GameFramework/GameplayMessageSubsystem.h"


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
	
	// Default Movement
	{
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
		
	// hotkey
	{
		if (IsValid(InputData->HotKeyIMC)) EISubsystem->AddMappingContext(InputData->HotKeyIMC, 0);
		if (IsValid(InputData->IA_HotKeyOne)) InEnhancedInputComponent->BindAction(InputData->IA_HotKeyOne, ETriggerEvent::Started, this, &UTSPlayerInputActionComponent::HotKeyOne_internal);
		if (IsValid(InputData->IA_HotKeyTwo)) InEnhancedInputComponent->BindAction(InputData->IA_HotKeyTwo, ETriggerEvent::Started, this, &UTSPlayerInputActionComponent::HotKeyTwo_internal);
		if (IsValid(InputData->IA_HotKeyThree)) InEnhancedInputComponent->BindAction(InputData->IA_HotKeyThree, ETriggerEvent::Started, this, &UTSPlayerInputActionComponent::HotKeyThree_internal);
		if (IsValid(InputData->IA_HotKeyFour)) InEnhancedInputComponent->BindAction(InputData->IA_HotKeyFour, ETriggerEvent::Started, this, &UTSPlayerInputActionComponent::HotKeyFour_internal);
		if (IsValid(InputData->IA_HotKeyFive)) InEnhancedInputComponent->BindAction(InputData->IA_HotKeyFive, ETriggerEvent::Started, this, &UTSPlayerInputActionComponent::HotKeyFive_internal);
		if (IsValid(InputData->IA_HotKeySix)) InEnhancedInputComponent->BindAction(InputData->IA_HotKeySix, ETriggerEvent::Started, this, &UTSPlayerInputActionComponent::HotKeySix_internal);
		if (IsValid(InputData->IA_HotKeySeven)) InEnhancedInputComponent->BindAction(InputData->IA_HotKeySeven, ETriggerEvent::Started, this, &UTSPlayerInputActionComponent::HotKeySeven_internal);
		if (IsValid(InputData->IA_HotKeyEight)) InEnhancedInputComponent->BindAction(InputData->IA_HotKeyEight, ETriggerEvent::Started, this, &UTSPlayerInputActionComponent::HotKeyEight_internal);
		if (IsValid(InputData->IA_HotKeyNine)) InEnhancedInputComponent->BindAction(InputData->IA_HotKeyNine, ETriggerEvent::Started, this, &UTSPlayerInputActionComponent::HotKeyNine_internal);
	}
	
	// interact
	{
		if (IsValid(InputData->InteractIMC)) EISubsystem->AddMappingContext(InputData->InteractIMC, 0);
		if (IsValid(InputData->IA_InteractByNormal)) InEnhancedInputComponent->BindAction(InputData->IA_InteractByNormal, ETriggerEvent::Started, this, &UTSPlayerInputActionComponent::InteractByNormal_internal);
		if (IsValid(InputData->IA_InteractByLeftClick)) InEnhancedInputComponent->BindAction(InputData->IA_InteractByLeftClick, ETriggerEvent::Started, this, &UTSPlayerInputActionComponent::InteractByLeftClick_internal);
	}
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
#pragma region 핫키_관련
	
	//━━━━━━━━━━━━━━━━━━━━
	// 핫키_관련
	//━━━━━━━━━━━━━━━━━━━━	
	
void UTSPlayerInputActionComponent::HotKeyOne_internal(const FInputActionValue& Value)
{
	SendHotKeyMessage_internal(1);
}

void UTSPlayerInputActionComponent::HotKeyTwo_internal(const FInputActionValue& Value)
{
	SendHotKeyMessage_internal(2);
}

void UTSPlayerInputActionComponent::HotKeyThree_internal(const FInputActionValue& Value)
{
	SendHotKeyMessage_internal(3);
}

void UTSPlayerInputActionComponent::HotKeyFour_internal(const FInputActionValue& Value)
{
	SendHotKeyMessage_internal(4);
}

void UTSPlayerInputActionComponent::HotKeyFive_internal(const FInputActionValue& Value)
{
	SendHotKeyMessage_internal(5);
}

void UTSPlayerInputActionComponent::HotKeySix_internal(const FInputActionValue& Value)
{
	SendHotKeyMessage_internal(5);
}

void UTSPlayerInputActionComponent::HotKeySeven_internal(const FInputActionValue& Value)
{
	SendHotKeyMessage_internal(7);
}

void UTSPlayerInputActionComponent::HotKeyEight_internal(const FInputActionValue& Value)
{
	SendHotKeyMessage_internal(8);
}

void UTSPlayerInputActionComponent::HotKeyNine_internal(const FInputActionValue& Value)
{
	SendHotKeyMessage_internal(9);
}

void UTSPlayerInputActionComponent::SendHotKeyMessage_internal(const int32& InActiveHotKeyIndex)
{
	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(GetWorld());
	if (!IsValid(&MessageSubsystem)) return;	

	FtSActiveHotkeyMessageData MSG;
	MSG.ActiveHotkeyIndex = InActiveHotKeyIndex;

	
	const UTSGameplayChannelSetting* Settings = GetDefault<UTSGameplayChannelSetting>();
	if (!IsValid(Settings)) return;
	
	MessageSubsystem.BroadcastMessage(Settings->SendCurrentActiveHotKeyChannelTag, MSG);
}

#pragma endregion
//======================================================================================================================
#pragma region 상호작용_관련
	
	//━━━━━━━━━━━━━━━━━━━━
	// 상호작용_관련
	//━━━━━━━━━━━━━━━━━━━━	

void UTSPlayerInputActionComponent::InteractByNormal_internal(const FInputActionValue& Value)
{
	if (!IsValid(OwnerCharacter)) return;
	UAbilitySystemComponent* OwnerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OwnerCharacter);
	if (!IsValid(OwnerASC)) return;
	
	FGameplayTagContainer TagContainer;
	TagContainer.AddTag(PossibleInteractTag::TS_Interact_Normal);
	OwnerASC->TryActivateAbilitiesByTag(TagContainer);
	
}

void UTSPlayerInputActionComponent::InteractByLeftClick_internal(const FInputActionValue& Value)
{
	if (!IsValid(OwnerCharacter)) return;
	UAbilitySystemComponent* OwnerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OwnerCharacter);
	if (!IsValid(OwnerASC)) return;
	
	FGameplayTagContainer TagContainer;
	TagContainer.AddTag(PossibleInteractTag::TS_Interact_LeftClick);
	OwnerASC->TryActivateAbilitiesByTag(TagContainer);
	
}

#pragma endregion
//======================================================================================================================