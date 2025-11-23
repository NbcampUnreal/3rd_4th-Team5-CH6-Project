#include "AI/Gaint/System/GiantSwitchingResourceArea.h"
#include "AI/Gaint/System/GiantSwitchingResourceAreaSubSystem.h"
#include "Components/BoxComponent.h"
#include "System/Time/TimeTickManager.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//----------------------------------
	// AGiantSwitchingResourceArea 라이프 사이클
	//----------------------------------

AGiantSwitchingResourceArea::AGiantSwitchingResourceArea()
{
	PrimaryActorTick.bCanEverTick = false;
	
	if (HasAuthority())
	{
		ResourceArea = CreateDefaultSubobject<UBoxComponent>("ResourceArea");
		SetRootComponent(ResourceArea);
	}
}

void AGiantSwitchingResourceArea::BeginPlay()
{
	Super::BeginPlay();
	
	UGiantSwitchingResourceAreaSubSystem* GSRASystem = UGiantSwitchingResourceAreaSubSystem::GetGiantSwitchingResourceAreaSubSystem(this);
	if (!IsValid(GSRASystem))
	{
		UE_LOG(GiantSwitchingResourceAreaSubSystem, Log, TEXT("GiantSwitchingResourceAreaSubSystem: 유효하지 않음."));
		return;
	}
	
	UE_LOG(GiantSwitchingResourceAreaSubSystem, Log, TEXT("GiantSwitchingResourceAreaSubSystem: ResourceArea 등록 시도."));
	GSRASystem->AddResourceArea(ResourceAreaTag, this);
	
	// TimeTickManager 가져오기
	UTimeTickManager* TimeTickManager = GetWorld()->GetSubsystem<UTimeTickManager>();
	if (!TimeTickManager)
	{
		UE_LOG(GiantSwitchingResourceAreaSubSystem, Error, TEXT("Initialize: TimeTickManager를 찾을 수 없습니다!"));
		return;
	}
	
	// TimeTickManager의 1초 신호 구독
	TimeTickManager->OnSecondTick.AddDynamic(this, &AGiantSwitchingResourceArea::ControlInterestMaxAge);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//----------------------------------
	// AGiantSwitchingResourceArea 관심 지역 시스템 관련
	//----------------------------------
	
void AGiantSwitchingResourceArea::ControlInterestMaxAge()
{
	if (InterestMaxAge.Num() == 0) return;
	
	InterestMaxAgeTimer++;
	if (InterestMaxAgeTimer >= InterestMaxAgeTime)
	{
		if (InterestMaxAge.Num() > 0)
		{
			auto It = InterestMaxAge.begin();   // 첫 번째 iterator
			InterestMaxAge.Remove(*It);									   // 해당 요소 제거
			UE_LOG(GiantSwitchingResourceAreaSubSystem, Log, TEXT("GiantSwitchingResourceAreaSubSystem: 시간 지남."));
		}
		else
		{
			UE_LOG(GiantSwitchingResourceAreaSubSystem, Log, TEXT("GiantSwitchingResourceAreaSubSystem: 시간 지날 게 없음."));
		}
		
		if (IntersectArea.Num() > 0)
		{
			auto It = IntersectArea.begin();    // 첫 번째 iterator
			
			UGiantSwitchingResourceAreaSubSystem* GSRASystem = UGiantSwitchingResourceAreaSubSystem::GetGiantSwitchingResourceAreaSubSystem(this);
			if (!IsValid(GSRASystem))
			{
				UE_LOG(GiantSwitchingResourceAreaSubSystem, Log, TEXT("GiantSwitchingResourceAreaSubSystem: 관심 지역 잊으려는데 시스템이 유효하지 않음."));
				return;
			}
	
			UE_LOG(GiantSwitchingResourceAreaSubSystem, Log, TEXT("GiantSwitchingResourceAreaSubSystem: IntersectArea 제거 시도."));
			GSRASystem->RemoveIntersectArea(*It);
			
		}
		else
		{
			UE_LOG(GiantSwitchingResourceAreaSubSystem, Log, TEXT("GiantSwitchingResourceAreaSubSystem: 잊을 관심 지역 없음"));
		}
		
		InterestMaxAgeTimer = 0;
	}
}

TArray<FVector> AGiantSwitchingResourceArea::GetAllIntersectArea() const
{
	TArray<FVector> InterestLocationArray;
	if (IntersectArea.Num() == 0)
	{
		return InterestLocationArray;
	}
	for (auto Value : IntersectArea)
	{
		InterestLocationArray.Add(Value);
	}
	return InterestLocationArray;
}

void AGiantSwitchingResourceArea::AddIntersectArea(const FVector& Location)
{
	if (HasAuthority())
	{
		if (IntersectArea.Num() >= MaxIntersectArea)
		{
			UE_LOG(GiantSwitchingResourceAreaSubSystem, Log, TEXT("GiantSwitchingResourceAreaSubSystem: 관심 지역 최대 갯수 넘음"));
			return;
		}
		
		FVector* FoundLocation = IntersectArea.Find(Location);
		if (FoundLocation != nullptr)
		{
			UE_LOG(GiantSwitchingResourceAreaSubSystem, Log, TEXT("GiantSwitchingResourceAreaSubSystem: 이미 등록된 관심 지역."));
		}
		else
		{
			UE_LOG(GiantSwitchingResourceAreaSubSystem, Log, TEXT("GiantSwitchingResourceAreaSubSystem: 관심 지역 등록"));
			UE_LOG(GiantSwitchingResourceAreaSubSystem, Log, TEXT("GiantSwitchingResourceAreaSubSystem: 관심 지역 위치 : %s"), *Location.ToString());
			UE_LOG(GiantSwitchingResourceAreaSubSystem, Log, TEXT("GiantSwitchingResourceAreaSubSystem: 관심 지역 태그 : %s"), *ResourceAreaTag.ToString());
			UE_LOG(GiantSwitchingResourceAreaSubSystem, Log, TEXT("GiantSwitchingResourceAreaSubSystem: 관심 지역 이름 : %s"), *GetName());
			IntersectArea.Add(Location);
			
			float GameTime = GetWorld()->GetTimeSeconds();
			InterestMaxAge.Add(GameTime);
		}
	}
}

void AGiantSwitchingResourceArea::RemoveIntersectArea(const FVector& Location)
{
	if (HasAuthority())
	{
		FVector* FoundLocation = IntersectArea.Find(Location);
		if (FoundLocation == nullptr)
		{
			UE_LOG(GiantSwitchingResourceAreaSubSystem, Log, TEXT("GiantSwitchingResourceAreaSubSystem: 이미 제거된 관심 지역."));
		}
		else
		{
			UE_LOG(GiantSwitchingResourceAreaSubSystem, Log, TEXT("GiantSwitchingResourceAreaSubSystem: 관심 지역 제거"));
			UE_LOG(GiantSwitchingResourceAreaSubSystem, Log, TEXT("GiantSwitchingResourceAreaSubSystem: 관심 지역 위치 : %s"), *Location.ToString());
			UE_LOG(GiantSwitchingResourceAreaSubSystem, Log, TEXT("GiantSwitchingResourceAreaSubSystem: 관심 지역 태그 : %s"), *ResourceAreaTag.ToString());
			UE_LOG(GiantSwitchingResourceAreaSubSystem, Log, TEXT("GiantSwitchingResourceAreaSubSystem: 관심 지역 이름 : %s"), *GetName());
			IntersectArea.Remove(Location);
		}
	}
}
