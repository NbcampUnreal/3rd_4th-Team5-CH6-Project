#include "System/Erosion/TSErosionSubSystem.h"
#include "System/Erosion/ErosionConfigData.h"
#include "System/Erosion/ErosionStateInfo.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "TimerManager.h"
#include "Engine/AssetManager.h"

DEFINE_LOG_CATEGORY(ErosionManager)

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//-----------------------------
	// UTSErosionSubSystem 라이프사이클
	//-----------------------------

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
	
	// 1초마다 UpdateErosion 실행
	// 1초 갱신 타이머를 쓰는 매니저가 맣아질 경우 추후 시관 관리 전역 매니저가 1초 갱신 타이머를 세팅하고
	// 다른 매니저들이 이를 구독하여 쓰는 형식으로 타이머 갯수를 줄일 필요가 있다.
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(ErosionTimerHandle,this,&UTSErosionSubSystem::UpdateErosion,TickInterval,true);
	}

	// 매니저 자신이 라이트의 요청 델리게이트를 구독함
	OnErosionLightSourceBroadcastDelegate.AddDynamic(this, &UTSErosionSubSystem::AddOrSubtractErosion);

	// 변수 캐싱
	CachedCurrentNaturalErosion = Config->NaturalErosionRate;
	CachedCurrentNaturalErosionSpeed = Config->NaturalErosionSpeed;
	bShowDebug = Config->bShowDebug;
}

void UTSErosionSubSystem::Deinitialize()
{
	// 타이머 해제
	if (UWorld* World = GetWorld())
	{
		if (World->GetTimerManager().IsTimerActive(ErosionTimerHandle))
		{
			World->GetTimerManager().ClearTimer(ErosionTimerHandle);
		}
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

	// 2) 침식 스테이지 업데이트 
	OnErosionChangedBroadcast();
	
	// 3) UI 업데이트
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

	// 현재 침식도와 마지막 침식도가 같으면 업데이트 X 
	if (LastErosion == CurrentErosion) return;
	
	// 3) 침식도에 따른 이벤트 발신
	OnErosionChangedBroadcast();

	// 4) 클라 동기화(Replicated Info 갱신)
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
		UE_LOG(ErosionManager, Warning, TEXT("StateInfo IsValid"));
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
				UE_LOG(ErosionManager, Warning, TEXT("StateInfo Found"));
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
				UE_LOG(ErosionManager, Warning, TEXT("StateInfo spawn Success"));
			}
			else
			{
				UE_LOG(ErosionManager, Warning, TEXT("StateInfo spawn Fail"));
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

	// 디버깅용 
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
	
	// 참고 : 현재 수치를 보내고 그걸 그냥 받아서 알아서 판단하는 게 좋을 수도 있고,
	// 참고 : 스테이지가 명확히 나누어져 있으면 그걸로 전부 통일하는 것도 나쁘지 않을 듯 하다.
	// 참고 : 이는 결국 어떻게 게임 플레이 설계를 잡느냐에 따라 다른 듯.
}

// TOD : UI 와 침식도 스테이지 업데이트를 타이머에서만 호출할 것인가?
// 1번 안 : 무조건 1초 타이머에서만 하기 (브로드 캐스트 줄어듬. 성능 향상) / "(단점 : 1초 뒤에 보이는 변화)
// 2번 안 : UI, 침식도를 변화에 맞춰 브로드 캐스트 (브로드 캐스트 늘어남) / "(장점 : 변화가 곧바로 보임. 1초 뒤 보임 같은 거 없음.)
// 현재는 일단 2번안으로 넣어놓음.
