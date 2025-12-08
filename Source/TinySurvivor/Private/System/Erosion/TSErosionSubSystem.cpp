#include "System/Erosion/TSErosionSubSystem.h"
#include "System/Erosion/ErosionConfigData.h"
#include "System/Erosion/ErosionStateInfo.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "TimerManager.h"
#include "Engine/AssetManager.h"
#include "System/Time/TimeTickManager.h"

DEFINE_LOG_CATEGORY(ErosionManager)

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//-----------------------------
	// UTSErosionSubSystem 라이프사이클
	//-----------------------------

bool UTSErosionSubSystem::ShouldCreateSubsystem(UObject* Outer) const
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
		UE_LOG(ErosionManager, Log, TEXT("ShouldCreateSubsystem: 서버에서 생성 (NetMode: %d)"), 
			static_cast<int32>(World->GetNetMode()));
	}
	else
	{// 클라이언트
		UE_LOG(ErosionManager, Log, TEXT("ShouldCreateSubsystem: 클라이언트에서는 생성하지 않음"));
	}
	
	return bIsServer;
}

void UTSErosionSubSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	// TimeTickManager가 먼저 초기화되도록 명시적 의존성 설정
	Collection.InitializeDependency(UTimeTickManager::StaticClass());
}

void UTSErosionSubSystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	// 서버만
	if (!IsServerAuth()) return;

#pragma region 침식도 데이터 에셋 가져오기
	
	// 1) AssetManager 접근
	UAssetManager& AssetMgr = UAssetManager::Get();

	// 2) PrimaryAssetId 정의
	const FPrimaryAssetId ConfigId(TEXT("ErosionConfigData"), TEXT("DA_ErosionConfig_Default"));
	UE_LOG(ErosionManager, Warning, TEXT("[Erosion] Trying to sync-load asset: %s"), *ConfigId.ToString());

	// 3) AssetManager에서 경로 얻기
	FSoftObjectPath AssetPath = AssetMgr.GetPrimaryAssetPath(ConfigId);
	if (!AssetPath.IsValid())
	{
		UE_LOG(ErosionManager, Error, TEXT("[Erosion] Invalid asset path for: %s"), *ConfigId.ToString());
		return;
	}

	// 4) 완전 동기 로드 (Blocking)
	// 인 게임 시작 전 필요한 필수 데이터 에셋은 웬만하면 동기 로드로 타이밍 안전성 확보할 것.
	UObject* LoadedObject = AssetPath.TryLoad();
	if (!LoadedObject)
	{
		UE_LOG(ErosionManager, Error, TEXT("[Erosion] Failed to synchronously load asset: %s"), *AssetPath.ToString());
		return;
	}

	// 5) 캐스팅
	Config = Cast<UErosionConfigData>(LoadedObject);
	if (Config)
	{
		UE_LOG(ErosionManager, Warning, TEXT("[Erosion] Sync loaded config: %s"), *Config->GetName());
	}
	else
	{
		UE_LOG(ErosionManager, Error, TEXT("[Erosion] Loaded asset is not UErosionConfigData type! Path: %s"), *AssetPath.ToString());
	}
#pragma endregion
	
	// StateInfo 싱글톤 존재 보장 (없으면 자동 스폰)
	EnsureStateInfoExists();
	
	// UI 업데이트
	BroadcastToStateInfo();
	
	// TimeTickManager 가져오기
	UTimeTickManager* TimeTickManager = GetWorld()->GetSubsystem<UTimeTickManager>();
	if (!TimeTickManager)
	{
		// TimeTickManager가 없으면 부패(Decay) 시스템 동작 불가
		UE_LOG(ErosionManager, Error, TEXT("Initialize: TimeTickManager를 찾을 수 없습니다!"));
		return;
	}
	
	// TimeTickManager의 1초 신호 구독
	TimeTickManager->OnSecondTick.AddDynamic(this, &UTSErosionSubSystem::UpdateErosion);
	
	UE_LOG(ErosionManager, Log, TEXT("DecayManager 초기화 완료 (TimeTickManager에 구독됨)"));

	// 매니저 자신이 라이트의 요청 델리게이트를 구독함
	OnErosionLightSourceBroadcastDelegate.AddDynamic(this, &UTSErosionSubSystem::AddOrSubtractErosion);

	// 변수 캐싱
	CachedCurrentNaturalErosion = Config->NaturalErosionRate;
	CachedCurrentNaturalErosionSpeed = Config->NaturalErosionSpeed;
	bShowDebug = Config->bShowDebug;
	StageStabilizeTime = Config->StageStabilizeTime;

	// 처음 시작 시 현재 침식도 레벨 브로드 캐스트
	FTimerHandle FirstTimeUpdateErosionStage;
	GetWorld()->GetTimerManager().SetTimer(FirstTimeUpdateErosionStage,this,&UTSErosionSubSystem::FirstTimeStageStabilize,1.0f,	false);
}

void UTSErosionSubSystem::Deinitialize()
{
	// TimeTickManager가 아직 유효한 경우에만 구독 해제 시도
	UTimeTickManager* TimeTickManager = GetWorld()->GetSubsystem<UTimeTickManager>();
	if (IsValid(TimeTickManager))
	{
		TimeTickManager->OnSecondTick.RemoveDynamic(this, &UTSErosionSubSystem::UpdateErosion);
		if (bShowDebug) UE_LOG(ErosionManager, Log, TEXT("TimeTickManager 구독 해제 완료"));
	}
	else
	{
		if (bShowDebug) UE_LOG(ErosionManager, Verbose, TEXT("TimeTickManager가 이미 소멸됨 (구독 해제 생략)"));
	}

	// 모든 타이머 해제
	if (IsValid(GetWorld()))
	{
		GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
	}
	
	// 모든 구독 해제 (자신)
	OnErosionChangedDelegate.RemoveAll(this);
	OnErosionLightSourceSubDelegate.RemoveAll(this);
	OnErosionLightSourceBroadcastDelegate.RemoveAll(this);
	
	Super::Deinitialize();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//-----------------------------
	// UTSErosionSubSystem 외부 API (inGame에서 호출)
	//-----------------------------

UTSErosionSubSystem* UTSErosionSubSystem::GetErosionSubSystem(const UObject* WorldContextObject)
{
	if (IsValid(WorldContextObject))
	{
		if (UWorld* World = WorldContextObject->GetWorld(); IsValid(World))
		{
			return World->GetSubsystem<UTSErosionSubSystem>();
		}
	}
	return nullptr;
}

void UTSErosionSubSystem::AddOrSubtractErosion(float AddOrSubtract)
{
	// 서버만
	if (!IsServerAuth()) return;

	// 0) MAX 인 경우 생존 리셋이 들어오기 전까지 업데이트 X
	if (bMaxInfluenceActive) return;
	
	// 1) 값 갱신
	CurrentErosion = FMath::Clamp(CurrentErosion + AddOrSubtract, Config->MinErosion, Config->MaxErosion);

	// 2) 침식도 안정화 중인가? (안정화 중인면 스테이지 변경 업데이트 하지 않음)
	if (bIsStageStabling) return;

	// 침식도 안정화 중인가? (안정화 중인면 스테이지 변경 업데이트 하지 않음)
	if (bIsStageStabling == false)
	{
		// 3) 침식도에 따른 이벤트 발신
		OnErosionChangedBroadcast();
	}
	
	// 4) UI 업데이트
	BroadcastToStateInfo();

	if (bShowDebug) UE_LOG(ErosionManager, Warning, TEXT("건축물 / AI 몬스터 / 거인에 의해 변경된 [Erosion] 값 +%.1f"), CurrentErosion);
}

void UTSErosionSubSystem::SetNaturalErosion(float NaturalErosionRate, float NaturalErosionSpeed)
{
	CachedCurrentNaturalErosion = NaturalErosionRate;
	CachedCurrentNaturalErosionSpeed = NaturalErosionSpeed;
}

void UTSErosionSubSystem::ResetErosion()
{
	// 서버만
	if (!IsServerAuth() || !Config) return;

	// 1) 리셋
	CurrentErosion = Config->MinErosion;
	LastErosion = Config->MinErosion;
	bMaxInfluenceActive = false;

	// 2) 침식 스테이지 업데이트 
	OnErosionChangedBroadcast();
	
	// 3) UI 업데이트
	BroadcastToStateInfo();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//-----------------------------
	// UTSErosionSubSystem 타이머
	//-----------------------------

void UTSErosionSubSystem::UpdateErosion()
{
	// 서버만
	if (!IsServerAuth() || !Config) return;

	// 0) MAX 인 경우 생존 리셋이 들어오기 전까지 업데이트 X
	if (bMaxInfluenceActive) return;
	
	// 1) 자연 상승
	ApplyNaturalIncrease();

	// 2) 빛 건축물 침식도 업데이트 요청
	OnErosionLightSourceSubDelegate.Broadcast();

	// 침식도 레벨 안정화 상태인가? (안정화 중인면 스테이지 변경 업데이트 하지 않음)
	if (bIsStageStabling == false)
	{
		// 현재 침식도와 마지막 침식도가 같으면 업데이트 X 
		if (LastErosion == CurrentErosion) return;
	
		// 3) 침식도에 따른 이벤트 발신
		OnErosionChangedBroadcast();
	}
	else
	{
		StabilizeStage();
		if (bShowDebug) UE_LOG(ErosionManager, Warning, TEXT("레벨 전환 안정화 중 %.1f / %.1f"), StageStabilizeStack, StageStabilizeTime);
	}
	
	// 4) UI 클라 동기화(Replicated Info 갱신)
	BroadcastToStateInfo();

	// 5) 마지막 침식도 값 업데이트 
	LastErosion = CurrentErosion;

	// 디버깅
	if (bShowDebug) UE_LOG(ErosionManager, Warning, TEXT("최종 [Erosion] 값 +%.1f"), CurrentErosion);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	

	//-----------------------------
	// UTSErosionSubSystem 내부 유틸 함수 
	//-----------------------------
	
void UTSErosionSubSystem::EnsureStateInfoExists()
{
	// 서버만
	if (!IsServerAuth()) return;
	
	// 재생성 방지
	if (IsValid(StateInfo))
	{
		if (bShowDebug) UE_LOG(ErosionManager, Warning, TEXT("StateInfo IsValid"));
		return;
	}
	if (UWorld* World = GetWorld(); World)
	{
		// 월드에서 인스턴스 탐색
		for (TActorIterator<AErosionStateInfo> It(World); It; ++It)
		{
			if (It->IsValidLowLevel() && It->IsA(AErosionStateInfo::StaticClass()))
			{
				StateInfo = *It;
				if (bShowDebug) UE_LOG(ErosionManager, Warning, TEXT("StateInfo Found"));
				StateInfo->bShowDebug = bShowDebug;
				return;
			}
		}

		// 없으면 스폰
		if (!StateInfo)
		{
			UE_LOG(ErosionManager, Warning, TEXT("StateInfo Spawn"));
			
			FActorSpawnParameters Params;
			Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			Params.bNoFail = true;
			Params.Owner = nullptr;
			Params.Instigator = nullptr;
			
			StateInfo = World->SpawnActor<AErosionStateInfo>(AErosionStateInfo::StaticClass(), FTransform::Identity, Params);
			
			if (IsValid(StateInfo))
			{
				StateInfo->SetReplicates(true);
				StateInfo->SetReplicateMovement(false); 
				StateInfo->SetNetUpdateFrequency(1.f);
				StateInfo->bShowDebug = bShowDebug;
				if (bShowDebug) UE_LOG(ErosionManager, Warning, TEXT("StateInfo spawn Success"));
			}
			else
			{
				if (bShowDebug) UE_LOG(ErosionManager, Warning, TEXT("StateInfo spawn Fail"));
			}
		}
	}
}

bool UTSErosionSubSystem::IsServerAuth() const
{
	if (const UWorld* World = GetWorld(); World)
	{
		if (World->GetAuthGameMode())
		{
			return true;
		}
	}
	return false;
}

void UTSErosionSubSystem::ApplyNaturalIncrease()
{
	// 1초씩 누적 (이 변수는 변경 X)
	NaturalElapsedTime += 1.0f;

	// 10초일 경우 리셋 및 더하기 (처음은 기본값 -> SetNaturalErosion를 통해 변경 가능.)
	if (NaturalElapsedTime >= CachedCurrentNaturalErosionSpeed)
	{
		NaturalElapsedTime = 0.0f;
		CurrentErosion += CachedCurrentNaturalErosion;
		CurrentErosion = FMath::Clamp(CurrentErosion, Config->MinErosion, Config->MaxErosion);
	}

	// 디버깅용 
	if (bShowDebug) UE_LOG(ErosionManager, Warning, TEXT("[Erosion] 자연 상승 (%f / %f) +%.1f → %.1f"), NaturalElapsedTime, CachedCurrentNaturalErosionSpeed, CachedCurrentNaturalErosion, CurrentErosion);
}

void UTSErosionSubSystem::BroadcastToStateInfo()
{
	if (!IsValid(StateInfo)) return;
	StateInfo->ServerSetErosion(CurrentErosion);
}

void UTSErosionSubSystem::OnErosionChangedBroadcast()
{
	if (!Config) return;
	if (bIsStageStabling) return;

	EErosionStage NewStage = EErosionStage::None;

	// 추후 기획이 잡히는대로 스테이지를 명획히야 데이터 에셋으로부터 캐싱하여 가져오는 것으로 변경해야 한다.
	// 혹은 기획쪽에서 굳이 필요없다고 하면 이대로 가도 무방할 듯. 
	if (CurrentErosion < 30)
		NewStage = EErosionStage::None;
	else if (30 <= CurrentErosion && CurrentErosion < 60)
		NewStage = EErosionStage::Stage30;
	else if (60 <= CurrentErosion && CurrentErosion < 90)
		NewStage = EErosionStage::Stage60;
	else if (90 <= CurrentErosion && CurrentErosion < Config->MaxErosion)
		NewStage = EErosionStage::Stage90;
	else if (Config->MaxErosion <= CurrentErosion)
		NewStage = EErosionStage::Max;

	// 이전과 같은 단계면 아무 일도 안 함
	if (NewStage == CurrentStage) return;
	
	// 스테이지 갱신 
	CurrentStage = NewStage;
	
	// 새로운 스테이지 발송 
	OnErosionChangedDelegate.Broadcast(CurrentErosion);

	// 새로운 스테이지가 되었으므로 침식도 스테이지 변화 안정화 시작
	bIsStageStabling = true;

	
	// 디버깅용
	if (bShowDebug) UE_LOG(ErosionManager, Warning, TEXT("[Erosion] //========================================================//"));
	switch (CurrentStage)
	{
	case EErosionStage::None:
		if (bShowDebug) UE_LOG(ErosionManager, Warning, TEXT("[Erosion] 안정 구간(30 미만)으로 돌아왔다."));
		break;
	case EErosionStage::Stage30:
		if (bShowDebug) UE_LOG(ErosionManager, Warning, TEXT("[Erosion] 30이 넘었다. (현재 30~60 미만)"));
		break;
	case EErosionStage::Stage60:
		if (bShowDebug) UE_LOG(ErosionManager, Warning, TEXT("[Erosion] 60이 넘었다.(현재 60~90 미만)"));
		break;
	case EErosionStage::Stage90:
		if (bShowDebug) UE_LOG(ErosionManager, Warning, TEXT("[Erosion] 90이 넘었다.(현재 90~Max 미만)"));
		break;
	case EErosionStage::Max:
		if (bShowDebug) UE_LOG(ErosionManager, Warning, TEXT("[Erosion] Max다. (현재 MAX)"));
		bMaxInfluenceActive = true;
		break;
	default:
		break;
	}
	if (bShowDebug) UE_LOG(ErosionManager, Warning, TEXT("[Erosion] //========================================================//"));
	
	// 참고 : 현재 수치를 보내고 그걸 그냥 받아서 알아서 판단하는 게 좋을 수도 있고,
	// 참고 : 스테이지가 명확히 나누어져 있으면 그걸로 전부 통일하는 것도 나쁘지 않을 듯 하다.
	// 참고 : 이는 결국 어떻게 게임 플레이 설계를 잡느냐에 따라 다른 듯.
}

void UTSErosionSubSystem::FirstTimeStageStabilize()
{
	if (!Config) return;
	if (bIsStageStabling) return;

	EErosionStage NewStage = EErosionStage::None;

	// 추후 기획이 잡히는대로 스테이지를 명획히야 데이터 에셋으로부터 캐싱하여 가져오는 것으로 변경해야 한다.
	// 혹은 기획쪽에서 굳이 필요없다고 하면 이대로 가도 무방할 듯. 
	if (CurrentErosion < 30)
		NewStage = EErosionStage::None;
	else if (30 <= CurrentErosion && CurrentErosion < 60)
		NewStage = EErosionStage::Stage30;
	else if (60 <= CurrentErosion && CurrentErosion < 90)
		NewStage = EErosionStage::Stage60;
	else if (90 <= CurrentErosion && CurrentErosion < Config->MaxErosion)
		NewStage = EErosionStage::Stage90;
	else if (Config->MaxErosion <= CurrentErosion)
		NewStage = EErosionStage::Max;

	// 스테이지 갱신 
	CurrentStage = NewStage;
	
	// 새로운 스테이지 발송 
	OnErosionChangedDelegate.Broadcast(CurrentErosion);

	// 새로운 스테이지가 되었으므로 침식도 스테이지 변화 안정화 시작 ? (일단 보류)
	// bIsStageStabling = true;

	// 디버깅용
	if (bShowDebug) UE_LOG(ErosionManager, Warning, TEXT("[Erosion] //========================================================//"));
	switch (CurrentStage)
	{
	case EErosionStage::None:
		if (bShowDebug) UE_LOG(ErosionManager, Warning, TEXT("[Erosion] 안정 구간(30 미만)으로 돌아왔다."));
		break;
	case EErosionStage::Stage30:
		if (bShowDebug) UE_LOG(ErosionManager, Warning, TEXT("[Erosion] 30이 넘었다. (현재 30~60 미만)"));
		break;
	case EErosionStage::Stage60:
		if (bShowDebug) UE_LOG(ErosionManager, Warning, TEXT("[Erosion] 60이 넘었다.(현재 60~90 미만)"));
		break;
	case EErosionStage::Stage90:
		if (bShowDebug) UE_LOG(ErosionManager, Warning, TEXT("[Erosion] 90이 넘었다.(현재 90~Max 미만)"));
		break;
	case EErosionStage::Max:
		if (bShowDebug) UE_LOG(ErosionManager, Warning, TEXT("[Erosion] Max다. (현재 MAX)"));
		bMaxInfluenceActive = true;
		break;
	default:
		break;
	}
	if (bShowDebug) UE_LOG(ErosionManager, Warning, TEXT("[Erosion] //========================================================//"));
}

void UTSErosionSubSystem::StabilizeStage()
{
	if (bIsStageStabling == true)
	{
		StageStabilizeStack += 1.f;

		if (StageStabilizeStack >= StageStabilizeTime)
		{
			StageStabilizeStack = 0.f;
			bIsStageStabling = false;
		}
	}
}

// UI 와 침식도 스테이지 업데이트를 타이머에서만 호출할 것인가?
// 1번 안 : 무조건 1초 타이머에서만 하기 (브로드 캐스트 줄어듬. 성능 향상) / "(단점 : 1초 뒤에 보이는 변화)
// 2번 안 : UI, 침식도를 변화에 맞춰 브로드 캐스트 (브로드 캐스트 늘어남) / "(장점 : 변화가 곧바로 보임. 1초 뒤 보임 같은 거 없음.)
// 현재는 일단 2번안으로 넣어놓음.
