// All CopyRight From YulRyongGameStudio //


#include "A_FOR_INGAME/SECTION_WORLD/ResourceSpawn/System/TSResourceSpawnTableDataSystem.h"
#include "A_FOR_INGAME/SECTION_WORLD/ResourceSpawn/Data/DataAsset/TSResourceSpawnSystemDataAsset.h"
#include "A_FOR_INGAME/SECTION_WORLD/ResourceSpawn/Library/TSResourceSpawnTableCachingHelperLibrary.h"
#include "A_FOR_INGAME/SECTION_WORLD/ResourceSpawn/Library/TSResourceSpawnTableDebugLibrary.h"
#include "A_FOR_INGAME/SECTION_WORLD/ResourceSpawn/Setting/TSResourceSpawnSetting.h"

//======================================================================================================================	
#pragma region 게터
	
	//━━━━━━━━━━━━━━━━━━━━
	// 게터
	//━━━━━━━━━━━━━━━━━━━━	

UTSResourceSpawnTableDataSystem* UTSResourceSpawnTableDataSystem::Get(const UObject* InWorldContextObject)
{
	if (!IsValid(InWorldContextObject)) return nullptr;
	
	UWorld* World = InWorldContextObject->GetWorld();
	if (!IsValid(World)) return nullptr;
			
	UGameInstance* GameInstance = World->GetGameInstance();

	UTSResourceSpawnTableDataSystem* ItemDataSubSystem = GameInstance->GetSubsystem<UTSResourceSpawnTableDataSystem>();
	if (!IsValid(ItemDataSubSystem)) return nullptr;

	return ItemDataSubSystem;
}

#pragma endregion
//======================================================================================================================	
#pragma region 라이프_사이클
	
	//━━━━━━━━━━━━━━━━━━━━
	// 라이프 사이클
	//━━━━━━━━━━━━━━━━━━━━	
	
void UTSResourceSpawnTableDataSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	// 데이터 초기화
	ResourceSpawnLogicDataArray.Empty();
	ResourceSpawnLogicDataMap.Empty();
	
	// 데이터 배열 공간 확보
	ResourceSpawnLogicDataArray.Reserve(1000);
	ResourceSpawnLogicDataMap.Reserve(1000);
	
	InitializeResourceSpawnTableStaticData_internal();
}

#pragma endregion
//======================================================================================================================	
#pragma region 내부_API
	
	//━━━━━━━━━━━━━━━━━━━━
	// 내부 API
	//━━━━━━━━━━━━━━━━━━━━	

void UTSResourceSpawnTableDataSystem::InitializeResourceSpawnTableStaticData_internal()
{
	// 1. DeveloperSettings에서 세팅 객체 가져오기
	const UTSResourceSpawnSetting* Settings = GetDefault<UTSResourceSpawnSetting>();
	if (!IsValid(Settings)) return;

	// 2. 소프트 포인터로 등록된 메인 데이터 에셋 로드하기
	UTSResourceSpawnSystemDataAsset* MainDataAsset = Settings->GlobalResourceSpawnDataAsset.LoadSynchronous();
	if (!IsValid(MainDataAsset)) return;
	
	// 캐싱용 임시 변수
	TMap<FGameplayTag, FTSResourceSpawnTableStaticData> TempItemStaticDataMap;
	TempItemStaticDataMap.Reserve(1000);
	
	// 3. 메인 에셋 내의 테이블 에셋 배열 순회
	for (auto& Table : MainDataAsset->ResourceSpawnTableArray)
	{
		// 기본 정보 유효성 체크
		if (!IsValid(Table)) continue;
		
		// [캐싱]
		UTSResourceSpawnTableCachingHelperLibrary::CacheResourceSpawnTable_Lib(Table, TempItemStaticDataMap);
	}
	
	// 4. 배열에 캐싱
	for (const auto& [Key, Value] : TempItemStaticDataMap)
	{
		ResourceSpawnLogicDataArray.Add(Value);
	}
	
	// 5. 조회용 맵에 캐싱
	for (auto& StaticData : ResourceSpawnLogicDataArray)
	{
		ResourceSpawnLogicDataMap.Add(StaticData.RegionTag, &StaticData);
	}
	
	// 디버그
	bWantPrintDeBugLog = Settings->bWantPrintDeBugLog;
	if (bWantPrintDeBugLog == false) return;
	for (const auto& [Key, Value] : ResourceSpawnLogicDataMap)
	{
		UTSResourceSpawnTableDebugLibrary::PrintDebugInfoSpawnTable_Lib(*Value);
	}
}

#pragma endregion
//======================================================================================================================	