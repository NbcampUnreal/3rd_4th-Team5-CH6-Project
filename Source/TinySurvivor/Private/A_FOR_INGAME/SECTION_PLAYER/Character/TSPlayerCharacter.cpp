// All CopyRight From YulRyongGameStudio //


#include "A_FOR_INGAME/SECTION_PLAYER/Character/TSPlayerCharacter.h"

#include "A_FOR_INGAME/SECTION_INTERACT/Comp/Player/TSPlayerInteractComponent.h"

#include "A_FOR_INGAME/SECTION_GAS/Data/DataAsset/TSGiveGAGEDataAsset.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "EnhancedInputComponent.h"

#include "A_FOR_INGAME/SECTION_ITEM/Inventory/Inventory/Comp/BackPack/TSBackPackInventoryComponent.h"
#include "A_FOR_INGAME/SECTION_ITEM/Inventory/Inventory/Comp/Body/TSBodyInventoryComponent.h"
#include "A_FOR_INGAME/SECTION_ITEM/Inventory/Inventory/Comp/HotKey/TSHotKeyInventoryComponent.h"
#include "A_FOR_INGAME/SECTION_ITEM/Inventory/Equip/Comp/BackPack/TSBackPackEquipVisualComponent.h"
#include "A_FOR_INGAME/SECTION_ITEM/Inventory/Equip/Comp/Body/TSBodyEquipVisualComponent.h"
#include "A_FOR_INGAME/SECTION_ITEM/Inventory/Equip/Comp/HotKey/TSHotKeyEquipVisualComponent.h"

#include "A_FOR_INGAME/SECTION_INGAMECYCLE/System/TSInGameCycleControlSystem.h"

#include "A_FOR_INGAME/SECTION_PLAYER/Comp/TSPlayerInputActionComponent.h"

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
		GetCharacterMovement()->bUseControllerDesiredRotation = false;	// 부드러운 회전 보간 사용 안 함 (즉각 반응 위해) (bUseControllerRotationYaw 켜져 있으면 효과 없음)

		// 3. 스프링 암 설정 (카메라 회전 관련)
		SpringArmComponent->bUsePawnControlRotation = true; // 카메라가 컨트롤러 회전을 따르게 함
	}
}

void ATSPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (HasAuthority()) SubscribeInGameCycleDelegate();
}

void ATSPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	// IMC 등록, IA 바인딩
	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
	if (!IsValid(EnhancedInputComponent)) return;
	if (IsValid(PlayerInputHandleComponent)) PlayerInputHandleComponent->SetupPlayerInput(EnhancedInputComponent);
}

void ATSPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// 컨트롤러 검사 
	if (!IsValid(NewController)) return;
	APlayerController* PC = Cast<APlayerController>(NewController);
	if (!IsValid(PC)) return;

	// 초기화 준비 등록 
	if (!HasAuthority()) return;
	UTSInGameCycleControlSystem* GameCycleControlSystem = UTSInGameCycleControlSystem::Get(this);
	if (!IsValid(GameCycleControlSystem)) return;
	GameCycleControlSystem->InitInGamePlayerRegister(PC);
	
	// GA 초기화 
	if (!IsValid(GetPlayerState())) return;
	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPlayerState());
	if (!IsValid(ASC)) return;
	ASC->InitAbilityActorInfo(GetPlayerState(), this);
	
	// 초기화 실시 및 완료 알림 
	InitGAAndSendInitComplete_internal(ASC, PC);
}

void ATSPlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	
	if (!IsValid(GetPlayerState()))return;
	
	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPlayerState());
	if (!IsValid(ASC)) return;
	
	ASC->InitAbilityActorInfo(GetPlayerState(), this);
	
}

void ATSPlayerCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (HasAuthority()) UnSubscribeToPlayerState();
	
	Super::EndPlay(EndPlayReason);
}

#pragma endregion
//======================================================================================================================
#pragma region 인게임_사이클
	
	
	//━━━━━━━━━━━━━━━━━━━━
	// 인게임 사이클
	//━━━━━━━━━━━━━━━━━━━━	

void ATSPlayerCharacter::SubscribeInGameCycleDelegate()
{
	UTSInGameCycleControlSystem* GameCycleControlSystem = UTSInGameCycleControlSystem::Get(this);
	if (!IsValid(GameCycleControlSystem)) return;
	GameCycleControlSystem->InGameCycleDelegate.AddDynamic(this, &ATSPlayerCharacter::OnReceivedInGameCycleDelegate_internal);
}

void ATSPlayerCharacter::UnSubscribeToPlayerState()
{
	UTSInGameCycleControlSystem* GameCycleControlSystem = UTSInGameCycleControlSystem::Get(this);
	if (!IsValid(GameCycleControlSystem)) return;
	GameCycleControlSystem->InGameCycleDelegate.RemoveAll(this);
}

void ATSPlayerCharacter::OnReceivedInGameCycleDelegate_internal(ETSInGameCycleMode InGameCycleMode, FTSSaveMasterData& InData)
{
	switch (InGameCycleMode)
	{
	case ETSInGameCycleMode::NEW:
		CallWhenNewModeIsCalled_internal();
		break;
		
	case ETSInGameCycleMode::LOAD:
		CallWhenLoadModeIsCalled_internal(InData);
		break;
		
	case ETSInGameCycleMode::PLAY:
		CallWhenPlayModeIsCalled_internal();
		break;
	}
}

void ATSPlayerCharacter::CallWhenNewModeIsCalled_internal()
{
	// 새 게임 준비 등록 
	UTSInGameCycleControlSystem* GameCycleControlSystem = UTSInGameCycleControlSystem::Get(this);
	if (!IsValid(GameCycleControlSystem)) return;
	if (!IsValid(GetController())) return;
	APlayerController* PC = CastChecked<APlayerController>(GetController());
	GameCycleControlSystem->NEW_GamePlayerRegister(PC);
	
	// TODO : 새 게임 준비 로직
	/////////////////////////////////////////////////////////////////////////////////////	
	/////////////////////////////////////////////////////////////////////////////////////	
	/////////////////////////////////////////////////////////////////////////////////////	
	/////////////////////////////////////////////////////////////////////////////////////	
	/////////////////////////////////////////////////////////////////////////////////////	
	/////////////////////////////////////////////////////////////////////////////////////	
	/////////////////////////////////////////////////////////////////////////////////////	
	/////////////////////////////////////////////////////////////////////////////////////	
	/////////////////////////////////////////////////////////////////////////////////////	
	/////////////////////////////////////////////////////////////////////////////////////	
	/////////////////////////////////////////////////////////////////////////////////////	
	/////////////////////////////////////////////////////////////////////////////////////	
	/////////////////////////////////////////////////////////////////////////////////////	
	/////////////////////////////////////////////////////////////////////////////////////	
	/////////////////////////////////////////////////////////////////////////////////////	
	/////////////////////////////////////////////////////////////////////////////////////	
	/////////////////////////////////////////////////////////////////////////////////////	
	
	// 새 게임 준비 완료 알림
	GameCycleControlSystem->NEW_GamePlayerComplete(PC);
}

void ATSPlayerCharacter::CallWhenLoadModeIsCalled_internal(FTSSaveMasterData& InData)
{
}

void ATSPlayerCharacter::CallWhenPlayModeIsCalled_internal()
{
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

void ATSPlayerCharacter::InitGAAndSendInitComplete_internal(UAbilitySystemComponent* InASC, APlayerController* InPC)
{
	if (!IsValid(InPC)) return;
	if (!IsValid(InASC)) return;	
	if (!IsValid(BaseGAGEData)) return;
	
	for (auto& GA : BaseGAGEData->BaseGameplayAbilities)
	{
		if (!IsValid(GA)) continue;
		FGameplayAbilitySpec AbilitySpec(GA, 1, INDEX_NONE, this);
		InASC->GiveAbility(AbilitySpec);
	}
	
	UTSInGameCycleControlSystem* GameCycleControlSystem = UTSInGameCycleControlSystem::Get(this);
	if (!IsValid(GameCycleControlSystem)) return;

	// 초기화 준비 완료 
	GameCycleControlSystem->InitInGamePlayerComplete(InPC);
}


#pragma endregion
//======================================================================================================================	
