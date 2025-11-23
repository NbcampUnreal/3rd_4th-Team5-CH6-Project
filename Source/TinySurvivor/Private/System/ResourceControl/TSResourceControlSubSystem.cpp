#include "System/ResourceControl/TSResourceControlSubSystem.h"
#include "AI/Gaint/System/GiantSwitchingResourceAreaSubSystem.h"
#include "System/ResourceControl/TSResourcePoint.h"

DEFINE_LOG_CATEGORY(ResourceControlSystem)

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	

	//------------------------------
	// UTSResourceControlSubSystem 라이프 사이클 
	//------------------------------
	
bool UTSResourceControlSubSystem::ShouldCreateSubsystem(UObject* Outer) const
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
		UE_LOG(ResourceControlSystem, Log, TEXT("ShouldCreateSubsystem: 서버에서 생성 (NetMode: %d)"), 
			static_cast<int32>(World->GetNetMode()));
	}
	else
	{// 클라이언트
		UE_LOG(ResourceControlSystem, Log, TEXT("ShouldCreateSubsystem: 클라이언트에서는 생성하지 않음"));
	}
	
	return bIsServer;
}

void UTSResourceControlSubSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UTSResourceControlSubSystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
}

void UTSResourceControlSubSystem::Deinitialize()
{
	Super::Deinitialize();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	

	//------------------------------
	// UTSResourceControlSubSystem 외부 유틸
	//------------------------------

UTSResourceControlSubSystem* UTSResourceControlSubSystem::GetUTSResourceControlSubSystem(const UObject* WorldContextObject)
{
	if (IsValid(WorldContextObject))
	{
		if (UWorld* World = WorldContextObject->GetWorld(); IsValid(World))
		{
			return World->GetSubsystem<UTSResourceControlSubSystem>();
		}
	}
	return nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	

	//------------------------------
	// UTSResourceControlSubSystem 자원 포인트 관리
	//------------------------------

void UTSResourceControlSubSystem::AssignReSourcePointToSector(const TObjectPtr<ATSResourcePoint> ResourcePoint)
{
	if (!IsValid(ResourcePoint))
	{
		UE_LOG(ResourceControlSystem, Warning, TEXT("등록하려는 자원 포인트가 유요하지 않음."));
		return;
	}
	
	const FGameplayTag SectionTag = ResourcePoint->GetSectionTag();
	const ENodeType NodeType = ResourcePoint->GetResourceItemType();

	// 섹터 Bucket 가져오기 (없으면 새로 생성)
	FResourcePointBucket& Bucket = ResourcePointMap.FindOrAdd(SectionTag);

	// Bucket 안에서 현재 NodeType과 동일한 그룹(FResourcePoints)을 찾기
	FResourcePoints* FoundGroup = nullptr;
	for (FResourcePoints& Group : Bucket.ResourcePointBucket)
	{
		if (Group.NodeType == NodeType)
		{
			FoundGroup = &Group;
			break;
		}
	}
	
	// NodeType 그룹이 없으면 새로 만들기
	if (!FoundGroup)
	{
		UE_LOG(ResourceControlSystem, Warning, TEXT("새로운 노드 타입 그룹 생성됨. Section: %s, NodeType: %d"), *SectionTag.ToString(), (int32)NodeType);
		FResourcePoints NewGroup;
		NewGroup.NodeType = NodeType;
		NewGroup.CachedResourcePoints.AddUnique(ResourcePoint);
		Bucket.ResourcePointBucket.Add(MoveTemp(NewGroup));
		DebugPrintAllBuckets();
		return;
	}

	// 4) 기존 그룹이 있다면 포인트 추가
	if (!FoundGroup->CachedResourcePoints.Contains(ResourcePoint))
	{
		FoundGroup->CachedResourcePoints.AddUnique(ResourcePoint);
		UE_LOG(ResourceControlSystem, Warning, TEXT("기존 노드 타입 그룹에 포인트 추가됨. Section: %s, NodeType: %d"),*SectionTag.ToString(), (int32)NodeType);
		DebugPrintAllBuckets();
	}
	
}

void UTSResourceControlSubSystem::DebugPrintAllBuckets()
{
	UE_LOG(ResourceControlSystem, Warning, TEXT("=========== 전체 버킷 디버그 출력 시작 ==========="));

	for (const auto& Elem : ResourcePointMap)
	{
		const FGameplayTag& SectionTag = Elem.Key;
		const FResourcePointBucket& Bucket = Elem.Value;

		UE_LOG(ResourceControlSystem, Warning, TEXT("섹터: %s"), *SectionTag.ToString());

		for (const FResourcePoints& Group : Bucket.ResourcePointBucket)
		{
			const UEnum* EnumPtr = StaticEnum<ENodeType>();
			FString NodeTypeName = EnumPtr
				? EnumPtr->GetDisplayNameTextByValue(static_cast<int64>(Group.NodeType)).ToString()
				: TEXT("Unknown");

			UE_LOG(ResourceControlSystem, Warning,
				TEXT("  NodeType: %s"),
				*NodeTypeName);

			for (const TObjectPtr<ATSResourcePoint>& RP : Group.CachedResourcePoints)
			{
				if (IsValid(RP))
				{
					const FVector Loc = RP->GetActorLocation();
					UE_LOG(ResourceControlSystem, Warning,
						TEXT("      • %s at (%.1f, %.1f, %.1f)"),
						*RP->GetName(),
						Loc.X, Loc.Y, Loc.Z
					);
				}
				else
				{
					UE_LOG(ResourceControlSystem, Warning,
						TEXT("      • Invalid ResourcePoint (null)"));
				}
			}
		}
	}
	UE_LOG(ResourceControlSystem, Warning, TEXT("=========== 전체 버킷 디버그 출력 끝 ==========="));
}
