// All CopyRight From YulRyongGameStudio //


#include "A_FOR_INGAME/SECTION_WORLD/Resource/System/TSResourceDataSystem.h"
#include "A_FOR_INGAME/SECTION_WORLD/Resource/Data/DataAsset/TSResourceDataSystemDataAsset.h"
#include "A_FOR_INGAME/SECTION_WORLD/Resource/Data/DataAsset/TSResourceTablesDataAsset.h"
#include "A_FOR_INGAME/SECTION_WORLD/Resource/Library/TSResourceDataSystemCachingHelperLibrary.h"
#include "A_FOR_INGAME/SECTION_WORLD/Resource/Library/TSResourceDataTableLogLibrary.h"
#include "A_FOR_INGAME/SECTION_WORLD/Resource/Setting/TSResourceDataSystemSettings.h"

//======================================================================================================================	
#pragma region 게터
	
	
	//━━━━━━━━━━━━━━━━━━━━
	// 게터
	//━━━━━━━━━━━━━━━━━━━━	

UTSResourceDataSystem* UTSResourceDataSystem::Get(const UObject* InWorldContextObject)
{
	if (!IsValid(InWorldContextObject)) return nullptr;
	
	UWorld* World = InWorldContextObject->GetWorld();
	if (!IsValid(World)) return nullptr;
			
	UGameInstance* GameInstance = World->GetGameInstance();

	UTSResourceDataSystem* ItemDataSubSystem = GameInstance->GetSubsystem<UTSResourceDataSystem>();
	if (!IsValid(ItemDataSubSystem)) return nullptr;

	return ItemDataSubSystem;
}

#pragma endregion
//======================================================================================================================	
#pragma region 라이프_사이클
	
	
	//━━━━━━━━━━━━━━━━━━━━
	// 라이프 사이클
	//━━━━━━━━━━━━━━━━━━━━	
void UTSResourceDataSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	// 데이터 초기화
	ResourceStaticDataArray.Empty();
	ResourceStaticDataMap.Empty();
	
	// 데이터 배열 공간 확보
	ResourceStaticDataArray.Reserve(1000);
	ResourceStaticDataMap.Reserve(1000);
	
	InitializeResourceStaticData_internal();
}

#pragma endregion
//======================================================================================================================	
#pragma region 외부_API
	
	//━━━━━━━━━━━━━━━━━━━━
	// 외부 API
	//━━━━━━━━━━━━━━━━━━━━

FTSResourceStaticData* UTSResourceDataSystem::GetResourceStaticData(int32 InResourceID)
{
	if (!ResourceStaticDataMap.Contains(InResourceID)) return nullptr;
	return *ResourceStaticDataMap.Find(InResourceID);
}

#pragma endregion
//======================================================================================================================	
#pragma region 내부_API
	
	//━━━━━━━━━━━━━━━━━━━━
	// 내부 API
	//━━━━━━━━━━━━━━━━━━━━
	

void UTSResourceDataSystem::InitializeResourceStaticData_internal()
{
	// 1. DeveloperSettings에서 세팅 객체 가져오기
	const UTSResourceDataSystemSettings* Settings = GetDefault<UTSResourceDataSystemSettings>();
	if (!IsValid(Settings)) return;

	// 2. 소프트 포인터로 등록된 메인 데이터 에셋 로드하기
	UTSResourceDataSystemDataAsset* MainDataAsset = Settings->GlobalResourceDataAsset.LoadSynchronous();
	if (!IsValid(MainDataAsset)) return;

	// 캐싱용 임시 변수
	TMap<int32, FTSResourceStaticData> TempResourceStaticDataMap;
	TempResourceStaticDataMap.Reserve(1000);
	
	// 3. 메인 에셋 내의 테이블 에셋 배열 순회
	for (const TObjectPtr<UTSResourceTablesDataAsset >& TableAssetPtr : MainDataAsset->ResourceInfoDataAssetArray)
	{
		// 기본 정보 유효성 체크 
		if (!IsValid(TableAssetPtr)) continue;
		
		// [캐싱]
		UTSResourceDataSystemCachingHelperLibrary::CacheBaseTable_Lib(TableAssetPtr->ResourceBaseInfoTable, TempResourceStaticDataMap, &FTSResourceStaticData::ResourceBaseInfoTable);
		UTSResourceDataSystemCachingHelperLibrary::CacheOtherTable_Lib(TableAssetPtr->ResourceUIInfoTable, TempResourceStaticDataMap, &FTSResourceStaticData::ResourceUIInfoTable);
		UTSResourceDataSystemCachingHelperLibrary::CacheOtherTable_Lib(TableAssetPtr->ResourceLootInfoTable, TempResourceStaticDataMap, &FTSResourceStaticData::ResourceLootInfoTable);	
	}
	
	// 4. 배열에 데이터 넣기
	for (const auto& [Key, Value] : TempResourceStaticDataMap)
	{
		ResourceStaticDataArray.Add(Value);
	}
	
	// 5. 조회용 맵에 넣기
	for (auto& StaticData : ResourceStaticDataArray)
	{
		ResourceStaticDataMap.Add(StaticData.ResourceBaseInfoTable.ResourceID, &StaticData);
	}
	
	// 로그 
	for (const auto& [Key, Value]  : ResourceStaticDataMap)
	{
		UTSResourceDataTableLogLibrary::LogResourceStaticData_Lib(Value);
	}
}
#pragma endregion
//======================================================================================================================	
