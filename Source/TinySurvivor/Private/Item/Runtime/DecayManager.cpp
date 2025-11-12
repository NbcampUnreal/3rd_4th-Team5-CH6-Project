// DecayManager.cpp
#include "Item/Runtime/DecayManager.h"
#include "System/Time/TimeTickManager.h"

DEFINE_LOG_CATEGORY_STATIC(LogDecayManager, Log, All);

#pragma region Lifecycle
bool UDecayManager::ShouldCreateSubsystem(UObject* Outer) const
{
	if (!Super::ShouldCreateSubsystem(Outer))
	{
		return false;
	}
	
	// World 유효성 체크
	UWorld* World = Cast<UWorld>(Outer);
	if (!World || !World->IsGameWorld())
	{// 월드가 없거나 게임 월드가 아니면 생성하지 않음
		return false;
	}
	
	// 서버에서만 생성 (클라이언트에서는 생성하지 않음)
	// TimeTickManager와 동일 조건
	const bool bIsServer = World->GetNetMode() != NM_Client;
	
	if (bIsServer)
	{
		UE_LOG(LogDecayManager, Log, TEXT("ShouldCreateSubsystem: 서버에서 생성"));
	}
	else
	{
		UE_LOG(LogDecayManager, Log, TEXT("ShouldCreateSubsystem: 클라이언트에서는 생성하지 않음"));
	}
	
	return bIsServer;
}

void UDecayManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	// TimeTickManager가 먼저 초기화되도록 명시적 의존성 설정
	// 이를 통해 TimeTickManager → DecayManager 순서 보장
	Collection.InitializeDependency(UTimeTickManager::StaticClass());
	
	// TimeTickManager 가져오기
	TimeTickManager = GetWorld()->GetSubsystem<UTimeTickManager>();
	
	if (!TimeTickManager)
	{// TimeTickManager가 없으면 부패(Decay) 시스템 동작 불가
		UE_LOG(LogDecayManager, Error, TEXT("Initialize: TimeTickManager를 찾을 수 없습니다!"));
		return;
	}
	
	// TimeTickManager의 1초 신호 구독
	TimeTickManager->OnSecondTick.AddDynamic(this, &UDecayManager::OnSecondTick);
	
	UE_LOG(LogDecayManager, Log, TEXT("DecayManager 초기화 완료 (TimeTickManager에 구독됨)"));
}

void UDecayManager::Deinitialize()
{
	// TimeTickManager가 아직 유효한 경우에만 구독 해제 시도
	if (IsValid(TimeTickManager))
	{
		TimeTickManager->OnSecondTick.RemoveDynamic(this, &UDecayManager::OnSecondTick);
		UE_LOG(LogDecayManager, Log, TEXT("TimeTickManager 구독 해제 완료"));
	}
	else
	{
		UE_LOG(LogDecayManager, Verbose, TEXT("TimeTickManager가 이미 소멸됨 (구독 해제 생략)"));
	}
	
	// 캐시 포인터 정리
	TimeTickManager = nullptr;
	
	Super::Deinitialize();
	
	UE_LOG(LogDecayManager, Log, TEXT("DecayManager 비활성화 완료"));
}
#pragma endregion

#pragma region TickHandling
void UDecayManager::OnSecondTick()
{
	//UE_LOG(LogDecayManager, Log, TEXT("[DecayManager] Before Broadcast"));
	
	// TimeTickManager로부터 1초 신호를 받음
	// 구독자들에게 부패도 업데이트 신호 전달
	OnDecayTick.Broadcast();
	
	//UE_LOG(LogDecayManager, Log, TEXT("[DecayManager] After Broadcast"));
	
	// 디버그용 (필요 시 주석 해제)
	// UE_LOG(LogDecayManager, Log, TEXT("부패 Tick 브로드캐스트 완료 (구독자 존재 여부: %s)"),
	// 	OnDecayTick.IsBound() ? TEXT("true") : TEXT("false"));
}
#pragma endregion
