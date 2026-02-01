// ItemDataSubsystem.cpp
#include "Item/System/ItemDataSubsystem.h"
#include "Item/System/ItemSystemSettings.h"
#include "Engine/Engine.h"

// 로그 카테고리 정의
DEFINE_LOG_CATEGORY_STATIC(LogItemDataSubsystem, Log, All);

//========================================
// Static Empty Data (빈 데이터 반환용)
//========================================
namespace
{
	static const FItemData EmptyItemData;
	static const FBuildingData EmptyBuildingData;
	static const FResourceData EmptyResourceData;
}

//========================================
// Constructor
//========================================
UItemDataSubsystem::UItemDataSubsystem()
	: bIsInitialized(false)
{
}

#pragma region Lifecycle
//========================================
// Subsystem Lifecycle
//========================================
void UItemDataSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	UE_LOG(LogItemDataSubsystem, Log, TEXT("ItemDataSubsystem Initialized"));
	
	// 네트워크 환경 확인 (디버그용)
	if (const UWorld* World = GetWorld())
	{
		const ENetMode NetMode = World->GetNetMode();
		UE_LOG(LogItemDataSubsystem, Log, TEXT("  NetMode: %s"),
			NetMode == NM_Standalone ? TEXT("Standalone") :
			NetMode == NM_DedicatedServer ? TEXT("DedicatedServer") :
			NetMode == NM_ListenServer ? TEXT("ListenServer") :
			NetMode == NM_Client ? TEXT("Client") : TEXT("Unknown"));
	}
#endif
	
	// Project Settings에서 자동 로드
	const UItemSystemSettings* Settings = UItemSystemSettings::Get();
	if (!Settings)
	{
		UE_LOG(LogItemDataSubsystem, Error, TEXT("ItemSystemSettings를 찾을 수 없습니다!"));
		return;
	}
	
	// Soft Reference 동기 로드
	UItemTableAsset* LoadedAsset = Settings->ItemTableAsset.LoadSynchronous();
	if (!LoadedAsset)
	{
		UE_LOG(LogItemDataSubsystem, Error, TEXT("ItemTableAsset 로드 실패! Project Settings > Item System에서 설정하세요."));
		return;
	}
	
	// 자동 초기화
	if (InitializeFromAsset(LoadedAsset))
	{
		UE_LOG(LogItemDataSubsystem, Log, TEXT("ItemDataSubsystem 자동 초기화 성공"));
	}
	else
	{
		UE_LOG(LogItemDataSubsystem, Error, TEXT("ItemDataSubsystem 초기화 실패"));
	}
}

void UItemDataSubsystem::Deinitialize()
{
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	UE_LOG(LogItemDataSubsystem, Log, TEXT("ItemDataSubsystem Deinitialized"));
#endif
	
	ClearAllCaches();
	TableAsset = nullptr;
	bIsInitialized = false;
	
	Super::Deinitialize();
}
#pragma endregion

#pragma region StaticAccessHelper
//========================================
// Static Access Helper
//========================================
UItemDataSubsystem* UItemDataSubsystem::GetItemDataSubsystem(const UObject* WorldContext)
{
	if (!WorldContext)
	{
		UE_LOG(LogItemDataSubsystem, Warning, TEXT("GetItemDataSubsystem: WorldContext가 null입니다"));
		return nullptr;
	}
	
	// WorldContext에서 UWorld 객체 가져오기
	const UWorld* World = GEngine->GetWorldFromContextObject(
		WorldContext, 
		EGetWorldErrorMode::LogAndReturnNull
	);
	
	if (!World)
	{
		UE_LOG(LogItemDataSubsystem, Warning, TEXT("GetItemDataSubsystem: WorldContext에서 UWorld를 가져오지 못했습니다"));
		return nullptr;
	}
	
	// World에서 GameInstance 가져오기
	UGameInstance* GameInstance = World->GetGameInstance();
	if (!GameInstance)
	{
		UE_LOG(LogItemDataSubsystem, Warning, TEXT("GetItemDataSubsystem: GameInstance가 null입니다"));
		return nullptr;
	}
	
	// GameInstance에서 ItemDataSubsystem 가져오기
	UItemDataSubsystem* Subsystem = GameInstance->GetSubsystem<UItemDataSubsystem>();
	if (!Subsystem)
	{
		UE_LOG(LogItemDataSubsystem, Error, TEXT("GetItemDataSubsystem: Subsystem을 찾을 수 없습니다!"));
		return nullptr;
	}
	
	// 서브시스템 초기화 확인
	if (!Subsystem->IsInitialized())
	{
		UE_LOG(LogItemDataSubsystem, Warning, 
			TEXT("GetItemDataSubsystem: Subsystem은 찾았지만 아직 초기화되지 않았습니다. 먼저 InitializeFromAsset()를 호출하세요."));
	}
	
	return Subsystem;
}
#pragma endregion

#pragma region Initialization
//========================================
// Initialization
//========================================
bool UItemDataSubsystem::InitializeFromAsset(UItemTableAsset* InTableAsset)
{
	if (!ValidateTableAsset(InTableAsset))
	{// 테이블 에셋 유효성 검사
		UE_LOG(LogItemDataSubsystem, Error, TEXT("초기화 실패: 유효하지 않은 TableAsset"));
		return false;
	}
	
	// 기존 캐시 초기화
	ClearAllCaches();
	
	// 데이터 에셋 저장
	TableAsset = InTableAsset;
	
	// 각 데이터 테이블 캐싱
	CacheItemData();		// 아이템 데이터 캐싱
	CacheBuildingData();	// 건축물 데이터 캐싱
	CacheResourceData();	// 자원 데이터 캐싱
	
	// 초기화 완료 플래그 설정
	bIsInitialized = true;
	
#if UE_BUILD_DEVELOPMENT || UE_BUILD_DEBUG
	// 개발/디버그 빌드에서 캐시 상태 로그 출력
	UE_LOG(LogItemDataSubsystem, Log, TEXT("ItemDataSubsystem 초기화 성공"));
	UE_LOG(LogItemDataSubsystem, Log, TEXT("- Items: %d"), ItemDataCache.Num());
	UE_LOG(LogItemDataSubsystem, Log, TEXT("- Buildings: %d"), BuildingDataCache.Num());
	UE_LOG(LogItemDataSubsystem, Log, TEXT("- Resources: %d"), ResourceDataCache.Num());
	
	// 에디터에서만 자동 테스트 실행
	RunInitializationTests();
#endif
	
	return true;
}
#pragma endregion

#pragma region ItemDataGetters
//========================================
// Item Data Getters
//========================================
const FItemData& UItemDataSubsystem::GetItemData(int32 ItemID) const
{
	if (!bIsInitialized)
	{// 초기화 여부 확인
		UE_LOG(LogItemDataSubsystem, Warning, TEXT("GetItemData 호출 시점: 아직 초기화되지 않음"));
		return EmptyItemData;
	}
	
	if (const FItemData* Found = ItemDataCache.Find(ItemID))
	{// 캐시에서 아이템 데이터 검색
		return *Found;
	}
	
	// 아이템 ID를 캐시에서 찾지 못했을 경우 경고 로그 출력
	UE_LOG(LogItemDataSubsystem, Warning, TEXT("ItemID %d를 캐시에서 찾을 수 없음"), ItemID);
	return EmptyItemData;
}

bool UItemDataSubsystem::GetItemDataSafe(int32 ItemID, FItemData& OutData) const
{
	if (!bIsInitialized)
	{// 초기화 여부 확인
		UE_LOG(LogItemDataSubsystem, Warning, TEXT("GetItemDataSafe 호출 시점: 아직 초기화되지 않음"));
		return false;
	}
	
	if (const FItemData* Found = ItemDataCache.Find(ItemID))
	{// 캐시에서 아이템 데이터 검색
		OutData = *Found; // 안전하게 OutData에 복사
		return true;
	}
	
	// 아이템 ID를 캐시에서 찾지 못했을 경우 false 반환
	return false;
}

TArray<int32> UItemDataSubsystem::GetAllItemIDs() const
{
	// 캐시에 저장된 모든 아이템 ID를 배열로 가져오기
	TArray<int32> ItemIDs;
	ItemDataCache.GetKeys(ItemIDs);
	return ItemIDs;
}

TArray<int32> UItemDataSubsystem::GetItemIDsByCategory(EItemCategory Category) const
{
	// 지정한 카테고리에 속하는 아이템 ID만 필터링하여 반환
	TArray<int32> FilteredIDs;
	
	// 캐시에 저장된 모든 아이템을 순회
	for (const TPair<int32, FItemData>& Pair : ItemDataCache)
	{
		if (Pair.Value.Category == Category)
		{// 아이템의 카테고리가 요청한 카테고리와 일치하면 배열에 추가
			FilteredIDs.Add(Pair.Key);
		}
	}

	return FilteredIDs;
}
#pragma endregion

#pragma region BuildingDataGetters
//========================================
// Building Data Getters
//========================================
const FBuildingData& UItemDataSubsystem::GetBuildingData(int32 BuildingID) const
{
	if (!bIsInitialized)
	{// 초기화 여부 확인
		UE_LOG(LogItemDataSubsystem, Warning, TEXT("GetBuildingData 호출 시점: 아직 초기화되지 않음"));
		return EmptyBuildingData;
	}
	
	if (const FBuildingData* Found = BuildingDataCache.Find(BuildingID))
	{// 캐시에서 건축물 데이터 검색
		return *Found;
	}
	
	// 건축물 ID를 캐시에서 찾지 못했을 경우 경고 로그 출력
	UE_LOG(LogItemDataSubsystem, Warning, TEXT("BuildingID %d를 캐시에서 찾을 수 없음"), BuildingID);
	return EmptyBuildingData;
}

bool UItemDataSubsystem::GetBuildingDataSafe(int32 BuildingID, FBuildingData& OutData) const
{
	if (!bIsInitialized)
	{// 초기화 여부 확인
		UE_LOG(LogItemDataSubsystem, Warning, TEXT("GetBuildingDataSafe 호출 시점: 아직 초기화되지 않음"));
		return false;
	}
	
	if (const FBuildingData* Found = BuildingDataCache.Find(BuildingID))
	{// 캐시에서 건축물 데이터 검색
		OutData = *Found; // 안전하게 OutData에 복사
		return true;
	}
	
	// 건축물 ID를 캐시에서 찾지 못했을 경우 false 반환
	return false;
}

TArray<int32> UItemDataSubsystem::GetAllBuildingIDs() const
{
	// 캐시에 저장된 모든 건축물 ID를 배열로 가져오기
	TArray<int32> BuildingIDs;
	BuildingDataCache.GetKeys(BuildingIDs);
	return BuildingIDs;
}

TArray<int32> UItemDataSubsystem::GetBuildingIDsByType(EBuildingType BuildingType) const
{
	// 지정한 건축물 타입에 속하는 건축물 ID만 필터링하여 반환
	TArray<int32> FilteredIDs;
	
	// 캐시에 저장된 모든 건축물 데이터를 순회
	for (const TPair<int32, FBuildingData>& Pair : BuildingDataCache)
	{
		if (Pair.Value.BuildingType == BuildingType)
		{// 건축물 타입이 요청한 타입과 일치하면 배열에 추가
			FilteredIDs.Add(Pair.Key);
		}
	}

	return FilteredIDs;
}
#pragma endregion

#pragma region ResourceDataGetters
//========================================
// Resource Data Getters
//========================================
const FResourceData& UItemDataSubsystem::GetResourceData(int32 ResourceID) const
{
	if (!bIsInitialized)
	{// 초기화 여부 확인
		UE_LOG(LogItemDataSubsystem, Warning, TEXT("GetResourceData 호출 시점: 아직 초기화되지 않음"));
		return EmptyResourceData;
	}
	
	if (const FResourceData* Found = ResourceDataCache.Find(ResourceID))
	{// 캐시에서 자원 데이터 검색
		return *Found;
	}
	
	// 자원 ID를 캐시에서 찾지 못했을 경우 경고 로그 출력
	UE_LOG(LogItemDataSubsystem, Warning, TEXT("ResourceID %d를 캐시에서 찾을 수 없음"), ResourceID);
	return EmptyResourceData;
}

bool UItemDataSubsystem::GetResourceDataSafe(int32 ResourceID, FResourceData& OutData) const
{
	if (!bIsInitialized)
	{// 초기화 여부 확인
		UE_LOG(LogItemDataSubsystem, Warning, TEXT("GetResourceDataSafe 호출 시점: 아직 초기화되지 않음"));
		return false;
	}
	
	if (const FResourceData* Found = ResourceDataCache.Find(ResourceID))
	{// 캐시에서 자원 데이터 검색
		OutData = *Found; // 안전하게 OutData에 복사
		return true;
	}
	
	// 자원 ID를 캐시에서 찾지 못했을 경우 false 반환
	return false;
}

TArray<int32> UItemDataSubsystem::GetAllResourceIDs() const
{
	// 캐시에 저장된 모든 자원 ID를 배열로 가져오기
	TArray<int32> ResourceIDs;
	ResourceDataCache.GetKeys(ResourceIDs);
	return ResourceIDs;
}

TArray<int32> UItemDataSubsystem::GetResourceIDsByNodeType(ENodeType NodeType) const
{
	// 지정한 노드 타입에 속하는 자원 ID만 필터링하여 반환
	TArray<int32> FilteredIDs;
	
	// 캐시에 저장된 모든 자원 데이터를 순회
	for (const TPair<int32, FResourceData>& Pair : ResourceDataCache)
	{
		if (Pair.Value.NodeType == NodeType)
		{// 자원의 노드 타입이 요청한 타입과 일치하면 배열에 추가
			FilteredIDs.Add(Pair.Key);
		}
	}

	return FilteredIDs;
}
#pragma endregion

#pragma region Validation
//========================================
// Validation
//========================================

// 특정 아이템 ID가 유효한지 확인
bool UItemDataSubsystem::IsItemDataValid(int32 ItemID) const
{
	// 초기화 여부 확인 후, 캐시에 해당 ID가 존재하는지 반환
	return bIsInitialized && ItemDataCache.Contains(ItemID);
}

// 특정 건축물 ID가 유효한지 확인
bool UItemDataSubsystem::IsBuildingDataValid(int32 BuildingID) const
{
	// 초기화 여부 확인 후, 캐시에 해당 ID가 존재하는지 반환
	return bIsInitialized && BuildingDataCache.Contains(BuildingID);
}

// 특정 자원 ID가 유효한지 확인
bool UItemDataSubsystem::IsResourceDataValid(int32 ResourceID) const
{
	// 초기화 여부 확인 후, 캐시에 해당 ID가 존재하는지 반환
	return bIsInitialized && ResourceDataCache.Contains(ResourceID);
}
#pragma endregion

#pragma region Debug
//========================================
// Statistics & Debug
//========================================

// 캐시에 저장된 데이터 통계 정보를 가져오기
void UItemDataSubsystem::GetCacheStatistics(int32& OutItemCount, int32& OutBuildingCount, int32& OutResourceCount) const
{
	OutItemCount = ItemDataCache.Num();			// 아이템 캐시에 저장된 데이터 수
	OutBuildingCount = BuildingDataCache.Num();	// 건축물 캐시에 저장된 데이터 수
	OutResourceCount = ResourceDataCache.Num();	// 자원 캐시에 저장된 데이터 수
}

// 캐시 디버그 정보를 콘솔에 출력
void UItemDataSubsystem::PrintCacheDebugInfo() const
{
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	UE_LOG(LogItemDataSubsystem, Display, TEXT("=== ItemDataSubsystem Cache Statistics ==="));
	UE_LOG(LogItemDataSubsystem, Display, TEXT("Initialized: %s"), bIsInitialized ? TEXT("Yes") : TEXT("No"));
	UE_LOG(LogItemDataSubsystem, Display, TEXT("Items Cached: %d"), ItemDataCache.Num());
	UE_LOG(LogItemDataSubsystem, Display, TEXT("Buildings Cached: %d"), BuildingDataCache.Num());
	UE_LOG(LogItemDataSubsystem, Display, TEXT("Resources Cached: %d"), ResourceDataCache.Num());
	
	// 아이템 카테고리별 통계 출력
	if (bIsInitialized && ItemDataCache.Num() > 0)
	{
		UE_LOG(LogItemDataSubsystem, Display, TEXT("\n--- Item Categories ---"));
		
		TMap<EItemCategory, int32> CategoryCounts;
		for (const TPair<int32, FItemData>& Pair : ItemDataCache)
		{
			int32& Count = CategoryCounts.FindOrAdd(Pair.Value.Category, 0);
			Count++;
		}
		
		for (const TPair<EItemCategory, int32>& Pair : CategoryCounts)
		{
			FString CategoryName;
			switch (Pair.Key)
			{
				case EItemCategory::MATERIAL: CategoryName = TEXT("Material"); break;
				case EItemCategory::WEAPON: CategoryName = TEXT("Weapon"); break;
				case EItemCategory::TOOL: CategoryName = TEXT("Tool"); break;
				case EItemCategory::CONSUMABLE: CategoryName = TEXT("Consumable"); break;
				case EItemCategory::ARMOR:      CategoryName = TEXT("Armor"); break;
				default: CategoryName = TEXT("Unknown"); break;
			}
			UE_LOG(LogItemDataSubsystem, Display, TEXT("  %s: %d"), *CategoryName, Pair.Value);
		}
	}
	
	// 모든 아이템의 상세 정보 로그 출력 추가
	// if (bIsInitialized && ItemDataCache.Num() > 0)
	// {
	// 	UE_LOG(LogItemDataSubsystem, Display, TEXT("\n--- Item Detailed Info ---"));
	// 	
	// 	for (const TPair<int32, FItemData>& Pair : ItemDataCache)
	// 	{
	// 		UE_LOG(LogItemDataSubsystem, Display, TEXT("ItemID %d Details:"), Pair.Key);
	// 		Pair.Value.PrintDebugInfo();
	// 	}
	// }
	
	// 건축물 타입별 통계 출력
	if (bIsInitialized && BuildingDataCache.Num() > 0)
	{
		UE_LOG(LogItemDataSubsystem, Display, TEXT("\n--- Building Types ---"));
		
		TMap<EBuildingType, int32> TypeCounts;
		for (const TPair<int32, FBuildingData>& Pair : BuildingDataCache)
		{
			int32& Count = TypeCounts.FindOrAdd(Pair.Value.BuildingType, 0);
			Count++;
		}
		
		for (const TPair<EBuildingType, int32>& Pair : TypeCounts)
		{
			FString TypeName;
			switch (Pair.Key)
			{
			case EBuildingType::STRUCT: TypeName = TEXT("Struct"); break;
				case EBuildingType::CRAFTING: TypeName = TEXT("Crafting"); break;
				case EBuildingType::LIGHT: TypeName = TEXT("Light"); break;
				case EBuildingType::STORAGE: TypeName = TEXT("Storage"); break;
				default: TypeName = TEXT("Unknown"); break;
			}
			UE_LOG(LogItemDataSubsystem, Display, TEXT("  %s: %d"), *TypeName, Pair.Value);
		}
	}
	
	// 모든 건축물의 상세 정보 로그 출력 추가
	// if (bIsInitialized && BuildingDataCache.Num() > 0)
	// {
	// 	UE_LOG(LogItemDataSubsystem, Display, TEXT("\n--- Building Detailed Info ---"));
	// 	
	// 	for (const TPair<int32, FBuildingData>& Pair : BuildingDataCache)
	// 	{
	// 		UE_LOG(LogItemDataSubsystem, Display, TEXT("BuildingID %d Details:"), Pair.Key);
	// 		Pair.Value.PrintDebugInfo();
	// 	}
	// }
	
	// 자원 NodeType별 통계 출력
	if (bIsInitialized && ResourceDataCache.Num() > 0)
	{
		UE_LOG(LogItemDataSubsystem, Display, TEXT("\n--- Resource Node Types ---"));
		
		TMap<ENodeType, int32> NodeTypeCounts;
		for (const TPair<int32, FResourceData>& Pair : ResourceDataCache)
		{
			int32& Count = NodeTypeCounts.FindOrAdd(Pair.Value.NodeType, 0);
			Count++;
		}
		
		for (const TPair<ENodeType, int32>& Pair : NodeTypeCounts)
		{
			FString NodeTypeName;
			switch (Pair.Key)
			{
				case ENodeType::JUNK: NodeTypeName = TEXT("Junk"); break;
				case ENodeType::PLANT: NodeTypeName = TEXT("Plant"); break;
				case ENodeType::INTERACT: NodeTypeName = TEXT("Interact"); break;
				case ENodeType::WOOD: NodeTypeName = TEXT("Wood"); break;
				case ENodeType::PLASTIC: NodeTypeName = TEXT("Plastic"); break;
				case ENodeType::DRINK: NodeTypeName = TEXT("Drink"); break;
				case ENodeType::FOOD: NodeTypeName = TEXT("Food"); break;
				case ENodeType::MINERAL: NodeTypeName = TEXT("Mineral"); break;
				case ENodeType::MONSTER_CORPSE: NodeTypeName = TEXT("Monster Corpse"); break;
				default: NodeTypeName = TEXT("Unknown"); break;
			}
			UE_LOG(LogItemDataSubsystem, Display, TEXT("  %s: %d"), *NodeTypeName, Pair.Value);
		}
	}
	
	// 모든 자원 원천의 상세 정보 로그 출력 추가
	// if (bIsInitialized && ResourceDataCache.Num() > 0)
	// {
	// 	UE_LOG(LogItemDataSubsystem, Display, TEXT("\n--- Resource Detailed Info ---"));
	// 	
	// 	for (const TPair<int32, FResourceData>& Pair : ResourceDataCache)
	// 	{
	// 		UE_LOG(LogItemDataSubsystem, Display, TEXT("ResourceID %d Details:"), Pair.Key);
	// 		Pair.Value.PrintDebugInfo();
	// 	}
	// }
	
	UE_LOG(LogItemDataSubsystem, Display, TEXT("=========================================="));
#endif
}

void UItemDataSubsystem::PrintItemDebugInfo(int32 ItemID) const
{
	if (!bIsInitialized)
	{
		UE_LOG(LogItemDataSubsystem, Warning, TEXT("[PrintItemDebugInfo] ItemDataSubsystem is NOT initialized."));
		return;
	}
	
	const FItemData* FoundData = ItemDataCache.Find(ItemID);
	
	if (!FoundData)
	{
		UE_LOG(LogItemDataSubsystem, Warning, TEXT("[PrintItemDebugInfo] ItemID %d not found in ItemDataCache."), ItemID);
		return;
	}
	
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	UE_LOG(LogItemDataSubsystem, Display, TEXT("\n========== Debug Info for ItemID %d =========="), ItemID);
	FoundData->PrintDebugInfo();
	UE_LOG(LogItemDataSubsystem, Display, TEXT("====================================================\n"));
#endif
}

void UItemDataSubsystem::PrintBuildingDebugInfo(int32 BuildingID) const
{
	if (!bIsInitialized)
	{
		UE_LOG(LogItemDataSubsystem, Warning, TEXT("[PrintBuildingDebugInfo] ItemDataSubsystem is NOT initialized."));
		return;
	}
	
	const FBuildingData* FoundData = BuildingDataCache.Find(BuildingID);
	
	if (!FoundData)
	{
		UE_LOG(LogItemDataSubsystem, Warning, TEXT("[PrintBuildingDebugInfo] BuildingID %d not found in BuildingDataCache."), BuildingID);
		return;
	}
	
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	UE_LOG(LogItemDataSubsystem, Display, TEXT("\n========== Debug Info for BuildingID %d =========="), BuildingID);
	FoundData->PrintDebugInfo();
	UE_LOG(LogItemDataSubsystem, Display, TEXT("====================================================\n"));
#endif
}

void UItemDataSubsystem::PrintResourceDebugInfo(int32 ResourceID) const
{
	if (!bIsInitialized)
	{
		UE_LOG(LogItemDataSubsystem, Warning, TEXT("[PrintResourceDebugInfo] ItemDataSubsystem is NOT initialized."));
		return;
	}
	
	const FResourceData* FoundData = ResourceDataCache.Find(ResourceID);
	
	if (!FoundData)
	{
		UE_LOG(LogItemDataSubsystem, Warning, TEXT("[PrintResourceDebugInfo] ResourceID %d not found in ResourceDataCache."), ResourceID);
		return;
	}
	
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	UE_LOG(LogItemDataSubsystem, Display, TEXT("\n========== Debug Info for ResourceID %d =========="), ResourceID);
	FoundData->PrintDebugInfo();
	UE_LOG(LogItemDataSubsystem, Display, TEXT("====================================================\n"));
#endif
}

// 캐시를 새로고침 (TableAsset 기반으로 다시 로드)
void UItemDataSubsystem::RefreshCache()
{
	// TableAsset이 설정되지 않았으면 경고 후 종료
	if (!TableAsset)
	{
		UE_LOG(LogItemDataSubsystem, Warning, TEXT("RefreshCache: TableAsset이 설정되지 않음"));
		return;
	}
	
	UE_LOG(LogItemDataSubsystem, Log, TEXT("TableAsset에서 캐시 새로고침 시작..."));
	
	// 기존 캐시 초기화 후 데이터 재캐싱
	ClearAllCaches();
	CacheItemData();
	CacheBuildingData();
	CacheResourceData();
	
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	UE_LOG(LogItemDataSubsystem, Log, TEXT("캐시 새로고침 완료"));
	// 새로고침 후 캐시 상태 디버그 출력
	PrintCacheDebugInfo();
#endif
}

void UItemDataSubsystem::RunInitializationTests()
{
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	UE_LOG(LogItemDataSubsystem, Display, TEXT("========================================"));
	UE_LOG(LogItemDataSubsystem, Display, TEXT("ItemDataSubsystem 초기화 테스트 시작"));
	UE_LOG(LogItemDataSubsystem, Display, TEXT("========================================"));
	
	// 1. 통계 출력
	PrintCacheDebugInfo();
	
	// 2. 아이템 조회 테스트
	UE_LOG(LogItemDataSubsystem, Display, TEXT("\n--- 아이템 조회 테스트 ---"));
	FItemData ItemData;
	if (GetItemDataSafe(1, ItemData))
	{
		UE_LOG(LogItemDataSubsystem, Display, TEXT("✅ 아이템 ID 1 찾음:"));
		UE_LOG(LogItemDataSubsystem, Display, TEXT("  - 이름(KR): %s"), *ItemData.Name_KR.ToString());
		UE_LOG(LogItemDataSubsystem, Display, TEXT("  - 이름(EN): %s"), *ItemData.Name_EN.ToString());
		UE_LOG(LogItemDataSubsystem, Display, TEXT("  - MaxStack: %d"), ItemData.MaxStack);
	}
	else
	{
		UE_LOG(LogItemDataSubsystem, Warning, TEXT("❌ 아이템 ID 1을 찾을 수 없습니다"));
	}
	
	if (GetItemDataSafe(300, ItemData))
	{
		UE_LOG(LogItemDataSubsystem, Display, TEXT("✅ 아이템 ID 300 찾음:"));
		PrintItemDebugInfo(300);
	}
	else
	{
		UE_LOG(LogItemDataSubsystem, Warning, TEXT("❌ 아이템 ID 300을 찾을 수 없습니다"));
	}
	
	// 3. 존재하지 않는 ID 테스트
	UE_LOG(LogItemDataSubsystem, Display, TEXT("\n--- 존재하지 않는 ID 테스트 ---"));
	if (GetItemDataSafe(9999, ItemData))
	{
		UE_LOG(LogItemDataSubsystem, Display, TEXT("✅ ID 9999 찾음 (이상함!)"));
	}
	else
	{
		UE_LOG(LogItemDataSubsystem, Display, TEXT("❌ ID 9999를 찾을 수 없습니다 (정상)"));
	}
	
	// 4. 건축물 조회 테스트
	UE_LOG(LogItemDataSubsystem, Display, TEXT("\n--- 건축물 조회 테스트 ---"));
	FBuildingData BuildingData;
	if (GetBuildingDataSafe(500, BuildingData))
	{
		UE_LOG(LogItemDataSubsystem, Display, TEXT("✅ 건축물 ID 500 찾음:"));
		UE_LOG(LogItemDataSubsystem, Display, TEXT("  - 이름(KR): %s"), *BuildingData.Name_KR.ToString());
		UE_LOG(LogItemDataSubsystem, Display, TEXT("  - 최대 내구도: %d"), BuildingData.MaxDurability);
	}
	else
	{
		UE_LOG(LogItemDataSubsystem, Warning, TEXT("❌ 건축물 ID 500을 찾을 수 없습니다"));
	}
	
	if (GetBuildingDataSafe(501, BuildingData))
	{
		UE_LOG(LogItemDataSubsystem, Display, TEXT("✅ 건축물 ID 501 찾음:"));
		PrintBuildingDebugInfo(501);
	}
	else
	{
		UE_LOG(LogItemDataSubsystem, Warning, TEXT("❌ 건축물 ID 501을 찾을 수 없습니다"));
	}
	
	// 5. 자원 조회 테스트
	UE_LOG(LogItemDataSubsystem, Display, TEXT("\n--- 자원 원천 조회 테스트 ---"));
	FResourceData ResourceData;
	if (GetResourceDataSafe(600, ResourceData))
	{
		UE_LOG(LogItemDataSubsystem, Display, TEXT("✅ 자원 ID 600 찾음:"));
		UE_LOG(LogItemDataSubsystem, Display, TEXT("  - 이름(KR): %s"), *ResourceData.Name_KR.ToString());
		UE_LOG(LogItemDataSubsystem, Display, TEXT("  - 총 수량: %d"), ResourceData.TotalYield);
	}
	else
	{
		UE_LOG(LogItemDataSubsystem, Warning, TEXT("❌ 자원 ID 600을 찾을 수 없습니다"));
	}
	
	if (GetResourceDataSafe(606, ResourceData))
	{
		UE_LOG(LogItemDataSubsystem, Display, TEXT("✅ 자원 ID 606 찾음:"));
		PrintResourceDebugInfo(606);
	}
	else
	{
		UE_LOG(LogItemDataSubsystem, Warning, TEXT("❌ 자원 ID 606을 찾을 수 없습니다"));
	}
	
	// 6. 카테고리별 필터링 테스트
	UE_LOG(LogItemDataSubsystem, Display, TEXT("\n--- 카테고리별 필터링 테스트 ---"));
	
	TArray<int32> MaterialIDs = GetItemIDsByCategory(EItemCategory::MATERIAL);
	UE_LOG(LogItemDataSubsystem, Display, TEXT("재료(MATERIAL) 개수: %d"), MaterialIDs.Num());
	
	TArray<int32> ToolIDs = GetItemIDsByCategory(EItemCategory::TOOL);
	UE_LOG(LogItemDataSubsystem, Display, TEXT("도구(TOOL) 개수: %d"), ToolIDs.Num());
	
	TArray<int32> WeaponIDs = GetItemIDsByCategory(EItemCategory::WEAPON);
	UE_LOG(LogItemDataSubsystem, Display, TEXT("무기(WEAPON) 개수: %d"), WeaponIDs.Num());
	
	TArray<int32> ArmorIDs = GetItemIDsByCategory(EItemCategory::ARMOR);
	UE_LOG(LogItemDataSubsystem, Display, TEXT("방어구(ARMOR) 개수: %d"), ArmorIDs.Num());
	
	TArray<int32> ConsumableIDs = GetItemIDsByCategory(EItemCategory::CONSUMABLE);
	UE_LOG(LogItemDataSubsystem, Display, TEXT("소모품(CONSUMABLE) 개수: %d"), ConsumableIDs.Num());
	
	UE_LOG(LogItemDataSubsystem, Display, TEXT("========================================"));
	UE_LOG(LogItemDataSubsystem, Display, TEXT("테스트 완료"));
	UE_LOG(LogItemDataSubsystem, Display, TEXT("========================================"));
#endif
}
#pragma endregion

#pragma region InternalCachingFunctions
//========================================
// Internal Caching Functions
//========================================

// 아이템 데이터를 캐시에 저장
void UItemDataSubsystem::CacheItemData()
{
	// TableAsset 또는 ItemDataTable이 유효하지 않으면 경고 후 종료
	if (!TableAsset || !TableAsset->ItemDataTable)
	{
		UE_LOG(LogItemDataSubsystem, Warning, TEXT("CacheItemData: 유효하지 않은 ItemDataTable"));
		return;
	}
	
	const UDataTable* DataTable = TableAsset->ItemDataTable;
	TArray<FItemData*> AllRows;
	// 데이터테이블에서 모든 아이템 데이터 가져오기
	DataTable->GetAllRows<FItemData>(TEXT("CacheItemData"), AllRows);
	
	for (FItemData* RowData : AllRows)
	{
		// 유효한 아이템 ID를 가진 데이터만 캐시에 추가
		if (RowData && RowData->ItemID > 0)
		{
			ItemDataCache.Add(RowData->ItemID, *RowData);
		}
		else
		{
			UE_LOG(LogItemDataSubsystem, Warning, TEXT("유효하지 않은 ItemData 행 (ID: %d)"),
				RowData ? RowData->ItemID : -1);
		}
	}
	
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	UE_LOG(LogItemDataSubsystem, Log, TEXT("총 %d개의 아이템 캐싱 완료"), ItemDataCache.Num());
#endif
}

// 건축물 데이터를 캐시에 저장
void UItemDataSubsystem::CacheBuildingData()
{
	// TableAsset 또는 BuildingDataTable이 유효하지 않으면 경고 후 종료
	if (!TableAsset || !TableAsset->BuildingDataTable)
	{
		UE_LOG(LogItemDataSubsystem, Warning, TEXT("CacheBuildingData: Invalid BuildingDataTable"));
		return;
	}
	
	const UDataTable* DataTable = TableAsset->BuildingDataTable;
	TArray<FBuildingData*> AllRows;
	// 데이터테이블에서 모든 건축물 데이터 가져오기
	DataTable->GetAllRows<FBuildingData>(TEXT("CacheBuildingData"), AllRows);
	
	for (FBuildingData* RowData : AllRows)
	{
		// 유효한 건축물 ID를 가진 데이터만 캐시에 추가
		if (RowData && RowData->BuildingID > 0)
		{
			BuildingDataCache.Add(RowData->BuildingID, *RowData);
		}
		else
		{
			UE_LOG(LogItemDataSubsystem, Warning, TEXT("유효하지 않은 BuildingData 행 (ID: %d)"),
				RowData ? RowData->BuildingID : -1);
		}
	}
	
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	UE_LOG(LogItemDataSubsystem, Log, TEXT("총 %d개의 건축물 캐싱 완료"), BuildingDataCache.Num());
#endif
}

// 자원 데이터를 캐시에 저장
void UItemDataSubsystem::CacheResourceData()
{
	// TableAsset 또는 ResourceDataTable이 유효하지 않으면 경고 후 종료
	if (!TableAsset || !TableAsset->ResourceDataTable)
	{
		UE_LOG(LogItemDataSubsystem, Warning, TEXT("CacheResourceData: 유효하지 않은 ResourceDataTable"));
		return;
	}
	
	const UDataTable* DataTable = TableAsset->ResourceDataTable;
	TArray<FResourceData*> AllRows;
	// 데이터테이블에서 모든 자원 데이터 가져오기
	DataTable->GetAllRows<FResourceData>(TEXT("CacheResourceData"), AllRows);
	
	for (FResourceData* RowData : AllRows)
	{
		// 유효한 자원 ID를 가진 데이터만 캐시에 추가
		if (RowData && RowData->ResourceID > 0)
		{
			ResourceDataCache.Add(RowData->ResourceID, *RowData);
		}
		else
		{
			UE_LOG(LogItemDataSubsystem, Warning, TEXT("유효하지 않은 ResourceData 행 (ID: %d)"),
				RowData ? RowData->ResourceID : -1);
		}
	}
	
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	UE_LOG(LogItemDataSubsystem, Log, TEXT("총 %d개의 자원 캐싱 완료"), ResourceDataCache.Num());
#endif
}

// 모든 캐시 데이터를 초기화
void UItemDataSubsystem::ClearAllCaches()
{
	// 아이템, 건물, 자원 캐시 비우기
	ItemDataCache.Empty();
	BuildingDataCache.Empty();
	ResourceDataCache.Empty();
	
	UE_LOG(LogItemDataSubsystem, Log, TEXT("All caches cleared"));
}

// TableAsset이 유효한지 검증
bool UItemDataSubsystem::ValidateTableAsset(const UItemTableAsset* InTableAsset) const
{
	// TableAsset이 null인지 확인
	if (!InTableAsset)
	{
		UE_LOG(LogItemDataSubsystem, Error, TEXT("TableAsset이 nullptr입니다"));
		return false;
	}
	
	bool bIsValid = true;
	
	// 아이템 데이터 테이블 존재 여부 확인
	if (!InTableAsset->ItemDataTable)
	{
		UE_LOG(LogItemDataSubsystem, Error, TEXT("TableAsset에 ItemDataTable이 설정되어 있지 않습니다!"));
		bIsValid = false;
	}
	
	// 건물 데이터 테이블 존재 여부 확인
	if (!InTableAsset->BuildingDataTable)
	{
		UE_LOG(LogItemDataSubsystem, Error, TEXT("TableAsset에 BuildingDataTable이 설정되어 있지 않습니다!"));
		bIsValid = false;
	}
	
	// 자원 데이터 테이블 존재 여부 확인
	if (!InTableAsset->ResourceDataTable)
	{
		UE_LOG(LogItemDataSubsystem, Error, TEXT("TableAsset에 ResourceDataTable이 설정되어 있지 않습니다!"));
		bIsValid = false;
	}

	return bIsValid;
}
#pragma endregion