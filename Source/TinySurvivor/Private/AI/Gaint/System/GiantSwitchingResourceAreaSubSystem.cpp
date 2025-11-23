#include "AI/Gaint/System/GiantSwitchingResourceAreaSubSystem.h"
#include "AI/Gaint/System/GiantSwitchingResourceArea.h"
#include "AI/Gaint/System/TSGiantSwitchingAreaDataAsset.h"
#include "Engine/AssetManager.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY(GiantSwitchingResourceAreaSubSystem);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	

	//------------------------------
	// UGiantSwitchingResourceAreaSubSystem 라이프 사이클 
	//------------------------------

bool UGiantSwitchingResourceAreaSubSystem::ShouldCreateSubsystem(UObject* Outer) const
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
		UE_LOG(GiantSwitchingResourceAreaSubSystem, Log, TEXT("GiantSwitchingResourceAreaSubSystem: 서버에서 생성 (NetMode: %d)"),  static_cast<int32>(World->GetNetMode()));
	}
	else
	{// 클라이언트
		UE_LOG(GiantSwitchingResourceAreaSubSystem, Log, TEXT("GiantSwitchingResourceAreaSubSystem: 클라이언트에서는 생성하지 않음"));
	}
	
	return bIsServer;
}

void UGiantSwitchingResourceAreaSubSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	// 1) AssetManager 접근
	UAssetManager& AssetMgr = UAssetManager::Get();

	// 2) PrimaryAssetId 정의
	const FPrimaryAssetId ConfigId(TEXT("GiantSwitchingAreaData"), TEXT("DA_GiantSwitchingAreaData"));
	UE_LOG(GiantSwitchingResourceAreaSubSystem, Warning, TEXT("[GiantSwitchingResourceAreaSubSystem] Trying to sync-load asset: %s"), *ConfigId.ToString());

	// 3) AssetManager에서 경로 얻기
	FSoftObjectPath AssetPath = AssetMgr.GetPrimaryAssetPath(ConfigId);
	if (!AssetPath.IsValid())
	{
		UE_LOG(GiantSwitchingResourceAreaSubSystem, Error, TEXT("[GiantSwitchingResourceAreaSubSystem] Invalid asset path for: %s"), *ConfigId.ToString());
		return;
	}

	// 4) 완전 동기 로드 (Blocking)
	// 인 게임 시작 전 필요한 필수 데이터 에셋은 웬만하면 동기 로드로 타이밍 안전성 확보할 것.
	UObject* LoadedObject = AssetPath.TryLoad();
	if (!LoadedObject)
	{
		UE_LOG(GiantSwitchingResourceAreaSubSystem, Error, TEXT("[GiantSwitchingResourceAreaSubSystem] Failed to synchronously load asset: %s"), *AssetPath.ToString());
		return;
	}

	// 5) 캐스팅
	GiantSwitchingAreaData = Cast<UTSGiantSwitchingAreaDataAsset>(LoadedObject);
	if (GiantSwitchingAreaData)
	{
		UE_LOG(GiantSwitchingResourceAreaSubSystem, Warning, TEXT("[GiantSwitchingResourceAreaSubSystem] Sync loaded config: %s"), *GiantSwitchingAreaData->GetName());
	}
	else
	{
		UE_LOG(GiantSwitchingResourceAreaSubSystem, Error, TEXT("[GiantSwitchingResourceAreaSubSystem] Loaded asset is not GiantSwitchingAreaData type! Path: %s"), *AssetPath.ToString());
	}
}

void UGiantSwitchingResourceAreaSubSystem::Deinitialize()
{
	Super::Deinitialize();
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	

	//------------------------------
	// UGiantSwitchingResourceAreaSubSystem 외부 유틸 
	//------------------------------

UGiantSwitchingResourceAreaSubSystem* UGiantSwitchingResourceAreaSubSystem::GetGiantSwitchingResourceAreaSubSystem(const UObject* WorldContextObject)
{
	if (IsValid(WorldContextObject))
	{
		if (UWorld* World = WorldContextObject->GetWorld(); IsValid(World))
		{
			return World->GetSubsystem<UGiantSwitchingResourceAreaSubSystem>();
		}
	}
	return nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	

	//------------------------------
	// UGiantSwitchingResourceAreaSubSystem 섹터 관리 시스템
	//------------------------------

void UGiantSwitchingResourceAreaSubSystem::AddResourceArea(const FGameplayTag& ResourceAreaIDTag, const TWeakObjectPtr<AGiantSwitchingResourceArea> ResourceArea)
{
	if (!ResourceArea.IsValid())
	{
		UE_LOG(GiantSwitchingResourceAreaSubSystem, Log, TEXT("GiantSwitchingResourceAreaSubSystem: ResourceArea 유효하지 않음."));
		return;
	}
	
	auto Value = ResourceAreaMap.Find(ResourceAreaIDTag);
	if (Value != nullptr)
	{
		UE_LOG(GiantSwitchingResourceAreaSubSystem, Log, TEXT("GiantSwitchingResourceAreaSubSystem: 이미 IDTag로 추가된 지역을 추가하려고 시도했음."));
		return;
	}
	
	ResourceAreaMap.Add(ResourceAreaIDTag, ResourceArea);
	UE_LOG(GiantSwitchingResourceAreaSubSystem, Log, TEXT("GiantSwitchingResourceAreaSubSystem: 추가 성공"));
	UE_LOG(GiantSwitchingResourceAreaSubSystem, Log, TEXT("GiantSwitchingResourceAreaSubSystem: 등록 지역 태그 : %s"), *ResourceAreaIDTag.ToString());
	UE_LOG(GiantSwitchingResourceAreaSubSystem, Log, TEXT("GiantSwitchingResourceAreaSubSystem: 등록 지역 액터 : %s"), *ResourceArea->GetName());
}

AGiantSwitchingResourceArea* UGiantSwitchingResourceAreaSubSystem::GetNearestResourceArea(const FVector& Location) const
{
	FVector GiantCurrentLocation = Location;
	TArray<AActor*> ResourceAreaActors;
	float Distance = 0.f;
	
	if (!IsValid(GiantSwitchingAreaData))
	{
		UE_LOG(GiantSwitchingResourceAreaSubSystem, Log, TEXT("GiantSwitchingResourceAreaSubSystem: GiantSwitchingAreaData is nullptr"));
		return nullptr;
	}
	
	UGameplayStatics::GetAllActorsOfClass(GetWorld(),  GiantSwitchingAreaData->AreaClass, ResourceAreaActors);
	AActor* Nearest  = UGameplayStatics::FindNearestActor(GiantCurrentLocation, ResourceAreaActors, Distance);
	if (!IsValid(Nearest))
	{
		UE_LOG(GiantSwitchingResourceAreaSubSystem, Log, TEXT("GiantSwitchingResourceAreaSubSystem: 가능한 서칭 지역이 없음. nullptr"));
		return nullptr;
	}
	
	auto ResourceArea = Cast<AGiantSwitchingResourceArea>(Nearest);
	UE_LOG(GiantSwitchingResourceAreaSubSystem, Log, TEXT("GiantSwitchingResourceAreaSubSystem: 찾은 지역 태그 : %s"), *ResourceArea->GetResourceAreaTag().ToString());
	UE_LOG(GiantSwitchingResourceAreaSubSystem, Log, TEXT("GiantSwitchingResourceAreaSubSystem: 찾은 지역 액터 : %s"), *ResourceArea->GetName());
	return ResourceArea;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	

	//------------------------------
	// UGiantSwitchingResourceAreaSubSystem 관심 지역 시스템
	//------------------------------

void UGiantSwitchingResourceAreaSubSystem::AddIntersectArea(const FVector& Location)
{
	FVector IntersetLocation = Location;
	TArray<AActor*> ResourceAreaActors;
	float Distance = 0.f;
	
	if (!IsValid(GiantSwitchingAreaData))
	{
		UE_LOG(GiantSwitchingResourceAreaSubSystem, Log, TEXT("GiantSwitchingResourceAreaSubSystem: GiantSwitchingAreaData is nullptr"));
		return;
	}
	
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), GiantSwitchingAreaData->AreaClass, ResourceAreaActors);
	AActor* Nearest  = UGameplayStatics::FindNearestActor(Location, ResourceAreaActors, Distance);
	if (!IsValid(Nearest))
	{
		UE_LOG(GiantSwitchingResourceAreaSubSystem, Log, TEXT("GiantSwitchingResourceAreaSubSystem: 가능한 서칭 지역이 없음. nullptr"));
		return;
	}
	
	auto ResourceArea = Cast<AGiantSwitchingResourceArea>(Nearest);
	auto FoundResourceArea = ResourceAreaMap.Find(ResourceArea->GetResourceAreaTag());
	if (!FoundResourceArea->IsValid())
	{
		UE_LOG(GiantSwitchingResourceAreaSubSystem, Log, TEXT("GiantSwitchingResourceAreaSubSystem: 찾으려는 지역이 유효하지 않음"));
		return;
	}
	
	CurrentIntersectAreaMap.Add(IntersetLocation, ResourceArea->GetResourceAreaTag());
	ResourceArea->AddIntersectArea(IntersetLocation);
}

void UGiantSwitchingResourceAreaSubSystem::RemoveIntersectArea(const FVector& Location)
{
	FGameplayTag* FoundResourceAreaTag = CurrentIntersectAreaMap.Find(Location);
	if (FoundResourceAreaTag == nullptr)
	{
		UE_LOG(GiantSwitchingResourceAreaSubSystem, Log, TEXT("GiantSwitchingResourceAreaSubSystem: 찾으려는 지역 태그가 유효하지 않음"));
		return;
	}

	auto FoundResourceArea = ResourceAreaMap.Find(*FoundResourceAreaTag);
	if (!FoundResourceArea->IsValid())
	{
		UE_LOG(GiantSwitchingResourceAreaSubSystem, Log, TEXT("GiantSwitchingResourceAreaSubSystem: 찾으려는 지역이 유효하지 않음"));
		return;
	}
		
	CurrentIntersectAreaMap.Remove(Location);
	FoundResourceArea->Get()->RemoveIntersectArea(Location);
}
