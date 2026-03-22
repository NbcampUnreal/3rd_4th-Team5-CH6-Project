// All CopyRight From YulRyongGameStudio //


#include "A_FOR_INGAME/SECTION_ITEM/Item/System/TSItemDataSubSystem.h"
#include "A_FOR_INGAME/SECTION_ITEM/Item/Data/DataAsset/TSItemDataSystemDataAsset.h"
#include "A_FOR_INGAME/SECTION_ITEM/Item/Data/DataAsset/TSItemTablesDataAsset.h"
#include "A_FOR_INGAME/SECTION_ITEM/Item/Library/TSItemDataSystemCachingHelperLibrary.h"
#include "A_FOR_INGAME/SECTION_ITEM/Item/Library/TSItemDataTableLogLibrary.h"
#include "A_FOR_INGAME/SECTION_ITEM/Item/Setting/TSItemDataSystemSettings.h"

//======================================================================================================================	
#pragma region 게터
	
	
	//━━━━━━━━━━━━━━━━━━━━
	// 게터
	//━━━━━━━━━━━━━━━━━━━━	

UTSItemDataSubSystem* UTSItemDataSubSystem::Get(const UObject* WorldContextObject)
{
	if (!IsValid(WorldContextObject)) return nullptr;
	
	UWorld* World = WorldContextObject->GetWorld();
	if (!IsValid(World)) return nullptr;
			
	UGameInstance* GameInstance = World->GetGameInstance();

	UTSItemDataSubSystem* ItemDataSubSystem = GameInstance->GetSubsystem<UTSItemDataSubSystem>();
	if (!IsValid(ItemDataSubSystem)) return nullptr;

	return ItemDataSubSystem;
}

#pragma endregion
//======================================================================================================================	
#pragma region 라이프_사이클
	
	
	//━━━━━━━━━━━━━━━━━━━━
	// 라이프 사이클
	//━━━━━━━━━━━━━━━━━━━━	
	
void UTSItemDataSubSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	// 데이터 초기화
	ItemStaticDataArray.Empty();
	ItemStaticDataMap.Empty();
	
	// 데이터 배열 공간 확보
	ItemStaticDataArray.Reserve(1000);
	ItemStaticDataMap.Reserve(1000);
	
	InitializeItemStaticData_internal();
}

#pragma endregion
//======================================================================================================================	
#pragma region 외부_API
	
	//━━━━━━━━━━━━━━━━━━━━
	// 외부 API
	//━━━━━━━━━━━━━━━━━━━━

FTSITemStaticData* UTSItemDataSubSystem::GetItemStaticData(int32 ItemID)
{
	return *ItemStaticDataMap.Find(ItemID);
}

#pragma endregion
//======================================================================================================================	
#pragma region 내부_API
	
	//━━━━━━━━━━━━━━━━━━━━
	// 내부 API
	//━━━━━━━━━━━━━━━━━━━━
	
void UTSItemDataSubSystem::InitializeItemStaticData_internal()
{
	
	// 1. DeveloperSettings에서 세팅 객체 가져오기
	const UTSItemDataSystemSettings* Settings = GetDefault<UTSItemDataSystemSettings>();
	if (!IsValid(Settings)) return;

	// 2. 소프트 포인터로 등록된 메인 데이터 에셋 로드하기
	UTSItemDataSystemDataAsset* MainDataAsset = Settings->GlobalItemDataAsset.LoadSynchronous();
	if (!IsValid(MainDataAsset)) return;

	// 캐싱용 임시 변수
	TMap<int32, FTSITemStaticData> TempItemStaticDataMap;
	TempItemStaticDataMap.Reserve(1000);
	
	// 3. 메인 에셋 내의 테이블 에셋 배열 순회
	for (const TObjectPtr<UTSItemTablesDataAsset>& TableAssetPtr : MainDataAsset->ItemInfoDataAssetArray)
	{
		// 기본 정보 유효성 체크
		if (!IsValid(TableAssetPtr)) continue;

		// [캐싱]
		UTSItemDataSystemCachingHelperLibrary::CacheBaseTable_Lib(TableAssetPtr->ItemBaseInfoTable, TempItemStaticDataMap, &FTSITemStaticData::ItemBaseInfoTable);
		UTSItemDataSystemCachingHelperLibrary::CacheOtherTable_Lib(TableAssetPtr->ItemInventoryInfoTable, TempItemStaticDataMap, &FTSITemStaticData::ItemInventoryInfoTable);
		UTSItemDataSystemCachingHelperLibrary::CacheOtherTable_Lib(TableAssetPtr->ItemUIInfoTable, TempItemStaticDataMap, &FTSITemStaticData::ItemUIInfoTable);
	}	
	
	// 4. 배열에 데이터 넣기
	for (const auto& [Key, Value] : TempItemStaticDataMap)
	{
		ItemStaticDataArray.Add(Value);
	}
	
	// 5. 조회용 맵에 넣기.
	for (auto& StaticData : ItemStaticDataArray)
	{
		ItemStaticDataMap.Add(StaticData.ItemBaseInfoTable.ItemID, &StaticData);
	}
	
	// 로그 
	for (const auto& [Key, Value] : ItemStaticDataMap)
	{
		UTSItemDataTableLogLibrary::LogStaticItemData_Lib(Value);
	}
}

#pragma endregion
//======================================================================================================================	