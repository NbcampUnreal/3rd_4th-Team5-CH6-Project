// All CopyRight From YulRyongGameStudio //


#include "A_FOR_INGAME/SECTION_PLAYER/Character/TSPlayerCharacter.h"

#include "A_FOR_INGAME/SECTION_INTERACT/Comp/Player/TSPlayerInteractComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

#include "A_FOR_INGAME/SECTION_ITEM/Inventory/Inventory/Comp/BackPack/TSBackPackInventoryComponent.h"
#include "A_FOR_INGAME/SECTION_ITEM/Inventory/Inventory/Comp/Body/TSBodyInventoryComponent.h"
#include "A_FOR_INGAME/SECTION_ITEM/Inventory/Inventory/Comp/HotKey/TSHotKeyInventoryComponent.h"

#include "A_FOR_INGAME/SECTION_PLAYER/Comp/TSPlayerInputActionComponent.h"
#include "EnhancedInputComponent.h"
#include "A_FOR_INGAME/SECTION_ITEM/Inventory/Equip/Comp/BackPack/TSBackPackEquipVisualComponent.h"
#include "A_FOR_INGAME/SECTION_ITEM/Inventory/Equip/Comp/Body/TSBodyEquipVisualComponent.h"
#include "A_FOR_INGAME/SECTION_ITEM/Inventory/Equip/Comp/HotKey/TSHotKeyEquipVisualComponent.h"

#include "GameFramework/PlayerState.h"

#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

#include "GameFramework/CharacterMovementComponent.h"

//======================================================================================================================	
#pragma region 라이프_사이클
	
	
	//━━━━━━━━━━━━━━━━━━━━
	// 라이프 사이클
	//━━━━━━━━━━━━━━━━━━━━	

ATSPlayerCharacter::ATSPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	
	bReplicates = true;
	SetReplicatingMovement(true);
	SetNetUpdateFrequency(100.f);
	SetMinNetUpdateFrequency(100.f);
	
	// 인벤토리  컴포넌트 
	HotKeyInventoryComponent = CreateDefaultSubobject<UTSHotKeyInventoryComponent>(TEXT("HotKeyInventoryComponent"));
	BackpackInventoryComponent = CreateDefaultSubobject<UTSBackPackInventoryComponent>(TEXT("BackpackInventoryComponent"));
	EquipmentInventoryComponent = CreateDefaultSubobject<UTSBodyInventoryComponent>(TEXT("EquipmentInventoryComponent"));
	
	// 비쥬얼 컴포넌트 
	HotKeyEquipVisualComponent = CreateDefaultSubobject<UTSHotKeyEquipVisualComponent>(TEXT("HotKeyEquipVisualComponent"));
	BackpackEquipVisualComponent = CreateDefaultSubobject<UTSBackPackEquipVisualComponent>(TEXT("BackpackEquipVisualComponent"));
	EquipVisualComponent = CreateDefaultSubobject<UTSBodyEquipVisualComponent>(TEXT("EquipVisualComponent"));
	
	// 플레이어 인터렉트 컴포넌트  
	PlayerInteractComponent = CreateDefaultSubobject<UTSPlayerInteractComponent>(TEXT("PlayerInteractComponent"));
	
	// 입력 컴포넌트들
	PlayerInputHandleComponent = CreateDefaultSubobject<UTSPlayerInputActionComponent>(TEXT("PlayerInputComponent"));
	
	// 카메라와 스프링 암 
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->SetupAttachment(GetRootComponent());
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);
	
	// 회전 관련 설정 
	{
		// 1. 캐릭터 몸체가 컨트롤러의 Yaw(좌우 회전) 값을 그대로 따르도록 설정
		bUseControllerRotationYaw = true;		// true: 마우스 돌리면 캐릭터 몸도 같이 돌아감
		bUseControllerRotationPitch = false;	// 보통 위아래는 캐릭터 몸이 숙여지면 이상하므로 false
		bUseControllerRotationRoll = false;

		// 2. 캐릭터 무브먼트 컴포넌트 설정 변경
		GetCharacterMovement()->bOrientRotationToMovement = false;		// 이동 방향으로 몸 돌리기 끄기
		GetCharacterMovement()->bUseControllerDesiredRotation = false;	// 부드러운 회전 보간 사용 안 함 (즉각 반응 위해)

		// 3. 스프링 암 설정 (카메라 회전 관련)
		SpringArmComponent->bUsePawnControlRotation = true; // 카메라가 컨트롤러 회전을 따르게 함
	}
}

void ATSPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
	if (!IsValid(EnhancedInputComponent)) return;
	
	if (IsValid(PlayerInputHandleComponent)) PlayerInputHandleComponent->SetupPlayerInput(EnhancedInputComponent);
}

void ATSPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	if (!IsValid(GetPlayerState())) return;
	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPlayerState());
	if (!IsValid(ASC)) return;
	
	ASC->InitAbilityActorInfo(GetPlayerState(), this);
}

void ATSPlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	
	if (!IsValid(GetPlayerState())) return;
	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPlayerState());
	if (!IsValid(ASC)) return;
	
	ASC->InitAbilityActorInfo(GetPlayerState(), this);
}

#pragma endregion
//======================================================================================================================
#pragma region GAS
	//━━━━━━━━━━━━━━━━━━━━
	// GAS
	//━━━━━━━━━━━━━━━━━━━━


UAbilitySystemComponent* ATSPlayerCharacter::GetAbilitySystemComponent() const
{
	if (!IsValid(GetPlayerState())) return nullptr;

	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPlayerState());
	if (!IsValid(ASC)) return nullptr;
	
	return ASC;
}


#pragma endregion
//======================================================================================================================	
