#include "AI/Gaint/TSGiant.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AI/Gaint/TSGiantAbilitySystemComponent.h"
#include "AI/Gaint/TSGiantAIController.h"
#include "AI/Gaint/TSGiantAttributeSet.h"
#include "Kismet/KismetSystemLibrary.h"

ATSGiant::ATSGiant()
{
	// tick
	PrimaryActorTick.bCanEverTick = false;
	
	// net
	bReplicates = true;
	SetReplicatingMovement(true);
	SetNetUpdateFrequency(100.f);
	SetMinNetUpdateFrequency(10.f);
	SetNetCullDistanceSquared(340282346638528859811704183484516925440.0f);
	
	// 회전
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 80.0f, 0.0f);
	
	// 속도
	GetCharacterMovement()->MaxWalkSpeed = 1000.f;
	
	// 콜리전
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	GetMesh()->SetCollisionResponseToAllChannels(ECR_Ignore);
	
	// GAS
	GiantAbilitySystemComponent = CreateDefaultSubobject<UTSGiantAbilitySystemComponent>(TEXT("GiantAbilitySystemComponent"));
	GiantAbilitySystemComponent->SetIsReplicated(true);
	GiantAbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	GiantAttributeSet = CreateDefaultSubobject<UTSGiantAttributeSet>(TEXT("GiantAttributeSet"));
}

void ATSGiant::BeginPlay()
{
	Super::BeginPlay();
	GiantAbilitySystemComponent->InitAbilityActorInfo(this, this);
	GiantAbilitySystemComponent->SetGiantAbilityList();
}

UAbilitySystemComponent* ATSGiant::GetAbilitySystemComponent() const
{
	return GiantAbilitySystemComponent;
}

UStateTreeAIComponent* ATSGiant::GetStateTreeAIComponent_Implementation()
{
	if (!IsValid(GetController())) return nullptr;
	AAIController* GiantAIController = CastChecked<AAIController>(GetController());
	if (!IsValid(GiantAIController)) return nullptr;
	
	if (!UKismetSystemLibrary::DoesImplementInterface(GiantAIController, UTSGiantCompInterface::StaticClass())) return nullptr;
	return ITSGiantCompInterface::Execute_GetStateTreeAIComponent(GiantAIController);
}
