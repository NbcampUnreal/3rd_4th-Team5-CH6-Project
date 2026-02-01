#include "AI/Gaint/TSGiantAIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISenseConfig_Sight.h"
#include "System/Erosion/TSErosionSubSystem.h"
#include "Components/StateTreeAIComponent.h"
#include "System/Erosion/ErosionTag.h"
#include "StateTree.h"
#include "Kismet/KismetSystemLibrary.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	

	//----------------------------------------
	// ATSGiantAIController 라이프 사이클
	//----------------------------------------

ATSGiantAIController::ATSGiantAIController()
{
	// tick
	PrimaryActorTick.bCanEverTick = true;
	
	// net
	bReplicates = true;
	SetReplicatingMovement(false);
	SetNetUpdateFrequency(100.f);
	SetMinNetUpdateFrequency(10.f);
	
	// 퍼셉션
	PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent"));
	SetPerceptionComponent(*PerceptionComponent);
	
	// Sight Config 생성
	UAISenseConfig_Sight* SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));

	SightConfig->SightRadius = 3000.f;                        // 시야 거리
	SightConfig->LoseSightRadius = 5000.f;                    // 시야 끊기는 거리
	SightConfig->PeripheralVisionAngleDegrees = 80.f;         // 시야각 (좌우 80도)
	SightConfig->SetMaxAge(60.f);                              // 감지 유지 시간
	
	// 적, 중립, 아군 전부 감지
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

	// Hearing Config 생성
	UAISenseConfig_Hearing* HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));
	HearingConfig->HearingRange = 5000.f;
	HearingConfig->LoSHearingRange = 7000.f; /* 5.7 버전 제거되지 않았음*/ 
	HearingConfig->bUseLoSHearing = true; /* 5.7 버전 제거되지 앟았음 */
	
	// 적, 중립, 아군 전부 감지
	HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
	HearingConfig->DetectionByAffiliation.bDetectFriendlies = true;
	HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;

	// PerceptionComponent에 Config 등록
	PerceptionComponent->ConfigureSense(*SightConfig);
	PerceptionComponent->ConfigureSense(*HearingConfig);

	// Dominant Sense 설정 (청각를 대표 센스로)
	PerceptionComponent->SetDominantSense(HearingConfig->GetSenseImplementation());

	// Perception Update 이벤트 바인딩
	PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this,&ATSGiantAIController::OnTargetPerceptionUpdated);
	
	// 스테이트 트리
	GiantStateTreeComponent = CreateDefaultSubobject<UStateTreeAIComponent>(TEXT("GiantStateTreeComponent"));
	if (!IsValid(GiantStateTreeAsset)) return;
	GiantStateTreeComponent->SetStateTree(GiantStateTreeAsset);
	GiantStateTreeComponent->bAutoActivate = true;
}

void ATSGiantAIController::BeginPlay()
{
	Super::BeginPlay();
	
	if (HasAuthority())
	{
		UTSErosionSubSystem* ErosionSubSystem = UTSErosionSubSystem::GetErosionSubSystem(this);
		if (!IsValid(ErosionSubSystem))
		{
			UE_LOG(ErosionManager, Warning, TEXT("거인 : 침식도 매니저 찾지 못했음."));
			return;
		}
		else
		{
			UE_LOG(ErosionManager, Warning, TEXT("거인 : 침식도 매니저 찾음."));
		}
	
		// 매니저의 침식도 스테이지 이벤트 구독
		ErosionSubSystem->OnErosionChangedDelegate.AddDynamic(this, &ATSGiantAIController::ChangeStateTreeLogicByErosionValue);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
	
	//----------------------------------------
	// ATSGiantAIController 퍼셉션
	//----------------------------------------

void ATSGiantAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	
}

UStateTreeAIComponent* ATSGiantAIController::GetStateTreeAIComponent_Implementation()
{
	return GiantStateTreeComponent;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
	
	//----------------------------------------
	// ATSGiantAIController 침식도
	//----------------------------------------

UAbilitySystemComponent* ATSGiantAIController::GetAbilitySystemComponent() const
{
	if (!IsValid(GetPawn())) return nullptr;
	if (!UKismetSystemLibrary::DoesImplementInterface(GetPawn(), UAbilitySystemInterface::StaticClass())) return nullptr;
	IAbilitySystemInterface* AbilitySystemInterface = Cast<IAbilitySystemInterface>(GetPawn());
	if (!AbilitySystemInterface) return nullptr;
	return AbilitySystemInterface->GetAbilitySystemComponent();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
	
	//----------------------------------------
	// ATSGiantAIController 침식도
	//----------------------------------------

void ATSGiantAIController::ChangeStateTreeLogicByErosionValue(float ErosionValue)
{
	if (IsValid(GiantStateTreeComponent)) return;
	GiantStateTreeComponent->SendStateTreeEvent(CheckErosionTag( ErosionValue));
}

FGameplayTag ATSGiantAIController::CheckErosionTag(float& ErosionValue)
{
	if (ErosionValue < 30)
		return ErosionTags::ErosionStage_Less30;
	
	else if (30 <= ErosionValue && ErosionValue < 60)
		return ErosionTags::ErosionStage_Between30And60;
	
	else if (60 <= ErosionValue && ErosionValue < 90)
		return ErosionTags::ErosionStage_Between60And90;
	
	else if (90 <= ErosionValue && ErosionValue < 100.f)
		return ErosionTags::ErosionStage_Between90AndMAX;
	
	else if (100.f <= ErosionValue)
		return ErosionTags::ErosionStage_MAX;
	
	return FGameplayTag::EmptyTag;
}
