// TimeTickManager.cpp
#include "System/Time/TimeTickManager.h"
#include "TimerManager.h"

DEFINE_LOG_CATEGORY_STATIC(LogTimeTickManager, Log, All);

#pragma region Lifecycle
bool UTimeTickManager::ShouldCreateSubsystem(UObject* Outer) const
{
	// 부모 클래스 체크
	if (!Super::ShouldCreateSubsystem(Outer))
	{
		return false;
	}
	
	// World 유효성 체크
	UWorld* World = Cast<UWorld>(Outer);
	if (!World)
	{// World가 없으면 서브시스템을 생성하지 않음
		return false;
	}
	
	// 게임 월드인지 확인 (에디터 프리뷰, PIE 등은 제외)
	if (!World->IsGameWorld())
	{// 게임 실행 중인 월드가 아니면 생성하지 않음
		return false;
	}
	
	// 서버에서만 생성 (클라이언트에서는 생성하지 않음)
	// NetMode 종류:
	//  - NM_Standalone      (0): 싱글플레이 → 서버 역할
	//  - NM_DedicatedServer (1): 데디케이티드 서버 → 서버
	//  - NM_ListenServer    (2): 리슨 서버 → 서버
	//  - NM_Client          (3): 클라이언트 → 생성 안 함!
	const bool bIsServer = World->GetNetMode() != NM_Client;
	if (bIsServer)
	{// 서버
		UE_LOG(LogTimeTickManager, Log, TEXT("ShouldCreateSubsystem: 서버에서 생성 (NetMode: %d)"), 
			static_cast<int32>(World->GetNetMode()));
	}
	else
	{// 클라이언트
		UE_LOG(LogTimeTickManager, Log, TEXT("ShouldCreateSubsystem: 클라이언트에서는 생성하지 않음"));
	}
	
	return bIsServer;
}

void UTimeTickManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTimeTickManager, Error, TEXT("World가 유효하지 않습니다"));
		return;
	}
	
	// 타이머 중복 등록 방지
	if (SecondTimerHandle.IsValid())
	{
		UE_LOG(LogTimeTickManager, Warning, TEXT("Initialize: 타이머가 이미 존재합니다. 설정을 건너뜁니다."));
		return;
	}
	
	// 1초마다 BroadcastSecondTick 호출하는 타이머 설정
	World->GetTimerManager().SetTimer(
		SecondTimerHandle,						// 타이머 핸들
		this,										// 타이머를 소유하는 객체
		&UTimeTickManager::BroadcastSecondTick,		// 주기적으로 호출할 함수
		1.0f,										// 호출 간격 (초)
		true										// 반복 실행 여부
	);
	
	UE_LOG(LogTimeTickManager, Log, TEXT("TimeTickManager 초기화 완료 (서버 전용, 1초 타이머 시작됨))"));
}

void UTimeTickManager::Deinitialize()
{
	UWorld* World = GetWorld();
	
	// 타이머가 유효하고, World 및 TimerManager가 존재할 때만 해제
	if (World)
	{
		FTimerManager& TimerManager = World->GetTimerManager();
		if (TimerManager.IsTimerActive(SecondTimerHandle))
		{
			TimerManager.ClearTimer(SecondTimerHandle);
			UE_LOG(LogTimeTickManager, Log, TEXT("타이머가 정상적으로 해제되었습니다"));
		}
	}
	
	Super::Deinitialize();
	
	UE_LOG(LogTimeTickManager, Log, TEXT("TimeTickManager 비활성화 완료"));
}
#pragma endregion

#pragma region TickHandling
void UTimeTickManager::BroadcastSecondTick()
{
	//UE_LOG(LogTimeTickManager, Log, TEXT("[TimeTickManager] Before Broadcast"));
	
	// 1초마다 델리게이트 브로드캐스트
	OnSecondTick.Broadcast();
	
	//UE_LOG(LogTimeTickManager, Log, TEXT("[TimeTickManager] After Broadcast"));
	
	// 디버그용 (필요 시 주석 해제)
	// UE_LOG(LogTimeTickManager, Log, TEXT("1초 Tick 브로드캐스트 완료 (구독자 존재 여부: %s)"),
	// 	OnSecondTick.IsBound() ? TEXT("true") : TEXT("false"));
}
#pragma endregion