// CraftingDataSubsystem.cpp
#include "Crafting/System/CraftingDataSubsystem.h"
#include "Crafting/System/CraftingSystemSettings.h"
#include "Engine/Engine.h"

// 로그 카테고리 정의
DEFINE_LOG_CATEGORY_STATIC(LogCraftingDataSubsystem, Log, All);

//========================================
// Static Empty Data (빈 데이터 반환용)
//========================================
namespace
{
	static const FCraftingData EmptyCraftingData;
}

//========================================
// Constructor
//========================================
UCraftingDataSubsystem::UCraftingDataSubsystem()
	: bIsInitialized(false)
{
}

#pragma region Lifecycle
//========================================
// Subsystem Lifecycle
//========================================
void UCraftingDataSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	UE_LOG(LogCraftingDataSubsystem, Log, TEXT("CraftingDataSubsystem Initialized"));
	
	// 네트워크 환경 확인 (디버그용)
	if (const UWorld* World = GetWorld())
	{
		const ENetMode NetMode = World->GetNetMode();
		UE_LOG(LogCraftingDataSubsystem, Log, TEXT("  NetMode: %s"),
			NetMode == NM_Standalone ? TEXT("Standalone") :
			NetMode == NM_DedicatedServer ? TEXT("DedicatedServer") :
			NetMode == NM_ListenServer ? TEXT("ListenServer") :
			NetMode == NM_Client ? TEXT("Client") : TEXT("Unknown"));
	}
#endif
	
	// Project Settings에서 자동 로드
	const UCraftingSystemSettings* Settings = UCraftingSystemSettings::Get();
	if (!Settings)
	{
		UE_LOG(LogCraftingDataSubsystem, Error, TEXT("CraftingSystemSettings를 찾을 수 없습니다!"));
		return;
	}
	
	// Soft Reference 동기 로드
	UCraftingTableAsset* LoadedAsset = Settings->CraftingTableAsset.LoadSynchronous();
	if (!LoadedAsset)
	{
		UE_LOG(LogCraftingDataSubsystem, Error, TEXT("CraftingTableAsset 로드 실패! Project Settings > Crafting System에서 설정하세요."));
		return;
	}
	
	// 자동 초기화
	if (InitializeFromAsset(LoadedAsset))
	{
		UE_LOG(LogCraftingDataSubsystem, Log, TEXT("CraftingDataSubsystem 자동 초기화 성공"));
	}
	else
	{
		UE_LOG(LogCraftingDataSubsystem, Error, TEXT("CraftingDataSubsystem 초기화 실패"));
	}
}

void UCraftingDataSubsystem::Deinitialize()
{
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	UE_LOG(LogCraftingDataSubsystem, Log, TEXT("CraftingDataSubsystem Deinitialized"));
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
UCraftingDataSubsystem* UCraftingDataSubsystem::GetCraftingDataSubsystem(const UObject* WorldContext)
{
	if (!WorldContext)
	{
		UE_LOG(LogCraftingDataSubsystem, Warning, TEXT("GetCraftingDataSubsystem: WorldContext가 null입니다"));
		return nullptr;
	}
	
	// WorldContext에서 UWorld 객체 가져오기
	const UWorld* World = GEngine->GetWorldFromContextObject(
		WorldContext, 
		EGetWorldErrorMode::LogAndReturnNull
	);
	
	if (!World)
	{
		UE_LOG(LogCraftingDataSubsystem, Warning, TEXT("GetCraftingDataSubsystem: WorldContext에서 UWorld를 가져오지 못했습니다"));
		return nullptr;
	}
	
	// World에서 GameInstance 가져오기
	UGameInstance* GameInstance = World->GetGameInstance();
	if (!GameInstance)
	{
		UE_LOG(LogCraftingDataSubsystem, Warning, TEXT("GetCraftingDataSubsystem: GameInstance가 null입니다"));
		return nullptr;
	}
	
	// GameInstance에서 CraftingDataSubsystem 가져오기
	UCraftingDataSubsystem* Subsystem = GameInstance->GetSubsystem<UCraftingDataSubsystem>();
	if (!Subsystem)
	{
		UE_LOG(LogCraftingDataSubsystem, Error, TEXT("GetCraftingDataSubsystem: Subsystem을 찾을 수 없습니다!"));
		return nullptr;
	}
	
	// 서브시스템 초기화 확인
	if (!Subsystem->IsInitialized())
	{
		UE_LOG(LogCraftingDataSubsystem, Warning, 
			TEXT("GetCraftingDataSubsystem: Subsystem은 찾았지만 아직 초기화되지 않았습니다. 먼저 InitializeFromAsset()를 호출하세요."));
	}
	
	return Subsystem;
}
#pragma endregion

#pragma region Initialization
//========================================
// Initialization
//========================================
bool UCraftingDataSubsystem::InitializeFromAsset(UCraftingTableAsset* InTableAsset)
{
	if (!ValidateTableAsset(InTableAsset))
	{// 테이블 에셋 유효성 검사
		UE_LOG(LogCraftingDataSubsystem, Error, TEXT("초기화 실패: 유효하지 않은 TableAsset"));
		return false;
	}
	
	// 기존 캐시 초기화
	ClearAllCaches();
	
	// 데이터 에셋 저장
	TableAsset = InTableAsset;
	
	// 레시피 데이터 캐싱
	CacheCraftingData();
	
	// 초기화 완료 플래그 설정
	bIsInitialized = true;
	
#if UE_BUILD_DEVELOPMENT || UE_BUILD_DEBUG
	// 개발/디버그 빌드에서 캐시 상태 로그 출력
	UE_LOG(LogCraftingDataSubsystem, Log, TEXT("CraftingDataSubsystem 초기화 성공"));
	UE_LOG(LogCraftingDataSubsystem, Log, TEXT("- Recipes: %d"), CraftingDataCache.Num());
	
	// 에디터에서만 자동 테스트 실행
	RunInitializationTests();
#endif
	
	return true;
}
#pragma endregion

#pragma region CraftingDataGetters
//========================================
// Crafting Data Getters
//========================================
const FCraftingData& UCraftingDataSubsystem::GetCraftingData(int32 RecipeID) const
{
	if (!bIsInitialized)
	{// 초기화 여부 확인
		UE_LOG(LogCraftingDataSubsystem, Warning, TEXT("GetCraftingData 호출 시점: 아직 초기화되지 않음"));
		return EmptyCraftingData;
	}
	
	if (const FCraftingData* Found = CraftingDataCache.Find(RecipeID))
	{// 캐시에서 레시피 데이터 검색
		return *Found;
	}
	
	// 레시피 ID를 캐시에서 찾지 못했을 경우 경고 로그 출력
	UE_LOG(LogCraftingDataSubsystem, Warning, TEXT("RecipeID %d를 캐시에서 찾을 수 없음"), RecipeID);
	return EmptyCraftingData;
}

bool UCraftingDataSubsystem::GetCraftingDataSafe(int32 RecipeID, FCraftingData& OutData) const
{
	if (!bIsInitialized)
	{// 초기화 여부 확인
		UE_LOG(LogCraftingDataSubsystem, Warning, TEXT("GetCraftingDataSafe 호출 시점: 아직 초기화되지 않음"));
		return false;
	}
	
	if (const FCraftingData* Found = CraftingDataCache.Find(RecipeID))
	{// 캐시에서 레시피 데이터 검색
		OutData = *Found; // 안전하게 OutData에 복사
		return true;
	}
	
	// 레시피 ID를 캐시에서 찾지 못했을 경우 false 반환
	return false;
}

TArray<int32> UCraftingDataSubsystem::GetAllRecipeIDs() const
{
	// 캐시에 저장된 모든 레시피 ID를 배열로 가져오기
	TArray<int32> RecipeIDs;
	CraftingDataCache.GetKeys(RecipeIDs);
	return RecipeIDs;
}

TArray<int32> UCraftingDataSubsystem::GetRecipeIDsByCategory(ECraftingCategory Category) const
{
	// 지정한 카테고리에 속하는 레시피 ID만 필터링하여 반환
	TArray<int32> FilteredIDs;
	
	// 캐시에 저장된 모든 레시피를 순회
	for (const TPair<int32, FCraftingData>& Pair : CraftingDataCache)
	{
		if (Pair.Value.Category == Category)
		{// 레시피의 카테고리가 요청한 카테고리와 일치하면 배열에 추가
			FilteredIDs.Add(Pair.Key);
		}
	}
	
	return FilteredIDs;
}

TArray<int32> UCraftingDataSubsystem::GetRecipeIDsByResultItem(int32 ResultItemID) const
{
	// 특정 아이템을 결과물로 하는 레시피 필터링
	TArray<int32> FilteredIDs;
	
	for (const TPair<int32, FCraftingData>& Pair : CraftingDataCache)
	{
		if (Pair.Value.ResultItemID == ResultItemID)
		{
			FilteredIDs.Add(Pair.Key);
		}
	}
	
	return FilteredIDs;
}

TArray<int32> UCraftingDataSubsystem::GetRecipeIDsByMaterial(int32 MaterialID) const
{
	// 특정 재료를 사용하는 레시피 필터링
	TArray<int32> FilteredIDs;
	
	for (const TPair<int32, FCraftingData>& Pair : CraftingDataCache)
	{
		if (Pair.Value.ContainsMaterial(MaterialID))
		{
			FilteredIDs.Add(Pair.Key);
		}
	}
	
	return FilteredIDs;
}

// TArray<int32> UCraftingDataSubsystem::GetHandCraftableRecipeIDs() const
// {
// 	// 제작대가 필요 없는 레시피 (손제작 가능)
// 	TArray<int32> FilteredIDs;
// 	
// 	for (const TPair<int32, FCraftingData>& Pair : CraftingDataCache)
// 	{
// 		if (!Pair.Value.RequiresCraftingStation())
// 		{
// 			FilteredIDs.Add(Pair.Key);
// 		}
// 	}
//
// 	return FilteredIDs;
// }

// TArray<int32> UCraftingDataSubsystem::GetRecipeIDsByCraftingStation(int32 CraftingStationID) const
// {
// 	// 특정 제작대에서 제작 가능한 레시피
// 	TArray<int32> FilteredIDs;
// 	
// 	for (const TPair<int32, FCraftingData>& Pair : CraftingDataCache)
// 	{
// 		if (Pair.Value.RequiredCraftingStation == CraftingStationID)
// 		{
// 			FilteredIDs.Add(Pair.Key);
// 		}
// 	}
// 	
// 	return FilteredIDs;
// }

// TArray<int32> UCraftingDataSubsystem::GetUnlockedRecipeIDs() const
// {
// 	// 잠금 해제된 레시피만 필터링
// 	TArray<int32> FilteredIDs;
// 	
// 	for (const TPair<int32, FCraftingData>& Pair : CraftingDataCache)
// 	{
// 		if (!Pair.Value.IsLocked())
// 		{
// 			FilteredIDs.Add(Pair.Key);
// 		}
// 	}
// 	
// 	return FilteredIDs;
// }
#pragma endregion

#pragma region Validation
//========================================
// Validation
//========================================

// 특정 레시피 ID가 유효한지 확인
bool UCraftingDataSubsystem::IsCraftingDataValid(int32 RecipeID) const
{
	// 초기화 여부 확인 후, 캐시에 해당 ID가 존재하는지 반환
	return bIsInitialized && CraftingDataCache.Contains(RecipeID);
}

// 레시피가 잠금 해제되었는지 확인
// bool UCraftingDataSubsystem::IsRecipeUnlocked(int32 RecipeID) const
// {
// 	if (!bIsInitialized)
// 	{
// 		return false;
// 	}
// 	
// 	if (const FCraftingData* Found = CraftingDataCache.Find(RecipeID))
// 	{
// 		return !Found->IsLocked();
// 	}
// 	
// 	return false;
// }

// 레시피 데이터가 올바른지 검증
bool UCraftingDataSubsystem::IsRecipeAvailable(int32 RecipeID) const
{
	if (!bIsInitialized)
	{
		return false;
	}
	
	if (const FCraftingData* Found = CraftingDataCache.Find(RecipeID))
	{
		return Found->IsValidRecipe();
	}
	
	return false;
}
#pragma endregion

#pragma region Debug
//========================================
// Statistics & Debug
//========================================

// 캐시에 저장된 데이터 통계 정보를 가져오기
void UCraftingDataSubsystem::GetCacheStatistics(int32& OutRecipeCount) const
{
	OutRecipeCount = CraftingDataCache.Num(); // 레시피 캐시에 저장된 데이터 수
}

// 캐시 디버그 정보를 콘솔에 출력
void UCraftingDataSubsystem::PrintCacheDebugInfo() const
{
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	UE_LOG(LogCraftingDataSubsystem, Display, TEXT("=== CraftingDataSubsystem Cache Statistics ==="));
	UE_LOG(LogCraftingDataSubsystem, Display, TEXT("Initialized: %s"), bIsInitialized ? TEXT("Yes") : TEXT("No"));
	UE_LOG(LogCraftingDataSubsystem, Display, TEXT("Recipes Cached: %d"), CraftingDataCache.Num());
	
	// 레시피 카테고리별 통계 출력
	if (bIsInitialized && CraftingDataCache.Num() > 0)
	{
		UE_LOG(LogCraftingDataSubsystem, Display, TEXT("\n--- Recipe Categories ---"));
		
		TMap<ECraftingCategory, int32> CategoryCounts;
		for (const TPair<int32, FCraftingData>& Pair : CraftingDataCache)
		{
			int32& Count = CategoryCounts.FindOrAdd(Pair.Value.Category, 0);
			Count++;
		}
		
		for (const TPair<ECraftingCategory, int32>& Pair : CategoryCounts)
		{
			FString CategoryName;
			switch (Pair.Key)
			{
				case ECraftingCategory::NONE: CategoryName = TEXT("None"); break;
				case ECraftingCategory::TOOL: CategoryName = TEXT("Tool"); break;
				case ECraftingCategory::WEAPON: CategoryName = TEXT("Weapon"); break;
				case ECraftingCategory::ARMOR: CategoryName = TEXT("Armor"); break;
				case ECraftingCategory::CONSUMABLE: CategoryName = TEXT("Consumable"); break;
				//case ECraftingCategory::BUILDING: CategoryName = TEXT("Building"); break;
				default: CategoryName = TEXT("Unknown"); break;
			}
			UE_LOG(LogCraftingDataSubsystem, Display, TEXT("  %s: %d"), *CategoryName, Pair.Value);
		}
		
		// 제작대 요구사항별 통계
		// UE_LOG(LogCraftingDataSubsystem, Display, TEXT("\n--- Crafting Station Requirements ---"));
		// int32 HandCraftableCount = 0;
		// TMap<int32, int32> StationCounts;
		//
		// for (const TPair<int32, FCraftingData>& Pair : CraftingDataCache)
		// {
		// 	if (Pair.Value.RequiresCraftingStation())
		// 	{
		// 		int32& Count = StationCounts.FindOrAdd(Pair.Value.RequiredCraftingStation, 0);
		// 		Count++;
		// 	}
		// 	else
		// 	{
		// 		HandCraftableCount++;
		// 	}
		// }
		//
		// UE_LOG(LogCraftingDataSubsystem, Display, TEXT("  Hand Craftable: %d"), HandCraftableCount);
		// for (const TPair<int32, int32>& Pair : StationCounts)
		// {
		// 	UE_LOG(LogCraftingDataSubsystem, Display, TEXT("  Station ID %d: %d recipes"), Pair.Key, Pair.Value);
		// }
		
		// 잠금 상태 통계
		// UE_LOG(LogCraftingDataSubsystem, Display, TEXT("\n--- Lock Status ---"));
		// int32 UnlockedCount = 0;
		// int32 LockedCount = 0;
		//
		// for (const TPair<int32, FCraftingData>& Pair : CraftingDataCache)
		// {
		// 	if (Pair.Value.IsLocked())
		// 	{
		// 		LockedCount++;
		// 	}
		// 	else
		// 	{
		// 		UnlockedCount++;
		// 	}
		// }
		//
		// UE_LOG(LogCraftingDataSubsystem, Display, TEXT("  Unlocked: %d"), UnlockedCount);
		// UE_LOG(LogCraftingDataSubsystem, Display, TEXT("  Locked: %d"), LockedCount);
	}
	
	// 모든 레시피의 상세 정보 로그 출력 추가 (필요시 주석 해제)
	// if (bIsInitialized && CraftingDataCache.Num() > 0)
	// {
	// 	UE_LOG(LogCraftingDataSubsystem, Display, TEXT("\n--- Recipe Detailed Info ---"));
	// 	
	// 	for (const TPair<int32, FCraftingData>& Pair : CraftingDataCache)
	// 	{
	// 		UE_LOG(LogCraftingDataSubsystem, Display, TEXT("RecipeID %d Details:"), Pair.Key);
	// 		Pair.Value.PrintDebugInfo();
	// 	}
	// }
	
	UE_LOG(LogCraftingDataSubsystem, Display, TEXT("==============================================="));
#endif
}

void UCraftingDataSubsystem::PrintRecipeDebugInfo(int32 RecipeID) const
{
	if (!bIsInitialized)
	{
		UE_LOG(LogCraftingDataSubsystem, Warning, TEXT("[PrintRecipeDebugInfo] CraftingDataSubsystem is NOT initialized."));
		return;
	}
	
	const FCraftingData* FoundData = CraftingDataCache.Find(RecipeID);
	
	if (!FoundData)
	{
		UE_LOG(LogCraftingDataSubsystem, Warning, TEXT("[PrintRecipeDebugInfo] RecipeID %d not found in CraftingDataCache."), RecipeID);
		return;
	}
	
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	UE_LOG(LogCraftingDataSubsystem, Display, TEXT("\n========== Debug Info for RecipeID %d =========="), RecipeID);
	FoundData->PrintDebugInfo();
	UE_LOG(LogCraftingDataSubsystem, Display, TEXT("====================================================\n"));
#endif
}

// 캐시를 새로고침 (TableAsset 기반으로 다시 로드)
void UCraftingDataSubsystem::RefreshCache()
{
	// TableAsset이 설정되지 않았으면 경고 후 종료
	if (!TableAsset)
	{
		UE_LOG(LogCraftingDataSubsystem, Warning, TEXT("RefreshCache: TableAsset이 설정되지 않음"));
		return;
	}
	
	UE_LOG(LogCraftingDataSubsystem, Log, TEXT("TableAsset에서 캐시 새로고침 시작..."));
	
	// 기존 캐시 초기화 후 데이터 재캐싱
	ClearAllCaches();
	CacheCraftingData();
	
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	UE_LOG(LogCraftingDataSubsystem, Log, TEXT("캐시 새로고침 완료"));
	// 새로고침 후 캐시 상태 디버그 출력
	PrintCacheDebugInfo();
#endif
}

void UCraftingDataSubsystem::RunInitializationTests()
{
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	UE_LOG(LogCraftingDataSubsystem, Display, TEXT("========================================"));
	UE_LOG(LogCraftingDataSubsystem, Display, TEXT("CraftingDataSubsystem 초기화 테스트 시작"));
	UE_LOG(LogCraftingDataSubsystem, Display, TEXT("========================================"));
	
	// 1. 통계 출력
	PrintCacheDebugInfo();
	
	// 2. 레시피 조회 테스트
	UE_LOG(LogCraftingDataSubsystem, Display, TEXT("\n--- 레시피 조회 테스트 ---"));
	FCraftingData RecipeData;
	if (GetCraftingDataSafe(1, RecipeData))
	{
		UE_LOG(LogCraftingDataSubsystem, Display, TEXT("✅ 레시피 ID 1 찾음:"));
		UE_LOG(LogCraftingDataSubsystem, Display, TEXT("  - 결과물 한글명: %s"), *RecipeData.ResultName_KR.ToString());
		UE_LOG(LogCraftingDataSubsystem, Display, TEXT("  - 결과물 영문명: %s"), *RecipeData.ResultName_EN.ToString());
		UE_LOG(LogCraftingDataSubsystem, Display, TEXT("  - 결과물 ItemID: %d"), RecipeData.ResultItemID);
		UE_LOG(LogCraftingDataSubsystem, Display, TEXT("  - 제작 시간: %.2f초"), RecipeData.CraftTime);
		UE_LOG(LogCraftingDataSubsystem, Display, TEXT("  - 재료 종류: %d"), RecipeData.GetIngredientTypeCount());
	}
	else
	{
		UE_LOG(LogCraftingDataSubsystem, Warning, TEXT("❌ 레시피 ID 1을 찾을 수 없습니다"));
	}
	
	if (GetCraftingDataSafe(5, RecipeData))
	{
		UE_LOG(LogCraftingDataSubsystem, Display, TEXT("✅ 레시피 ID 5 찾음:"));
		PrintRecipeDebugInfo(5);
	}
	else
	{
		UE_LOG(LogCraftingDataSubsystem, Warning, TEXT("❌ 레시피 ID 5를 찾을 수 없습니다"));
	}
	
	// 3. 존재하지 않는 ID 테스트
	UE_LOG(LogCraftingDataSubsystem, Display, TEXT("\n--- 존재하지 않는 ID 테스트 ---"));
	if (GetCraftingDataSafe(9999, RecipeData))
	{
		UE_LOG(LogCraftingDataSubsystem, Display, TEXT("✅ ID 9999 찾음 (이상함!)"));
	}
	else
	{
		UE_LOG(LogCraftingDataSubsystem, Display, TEXT("❌ ID 9999를 찾을 수 없습니다 (정상)"));
	}
	
	// 4. 카테고리별 필터링 테스트
	UE_LOG(LogCraftingDataSubsystem, Display, TEXT("\n--- 카테고리별 필터링 테스트 ---"));
	
	TArray<int32> ToolRecipes = GetRecipeIDsByCategory(ECraftingCategory::TOOL);
	UE_LOG(LogCraftingDataSubsystem, Display, TEXT("도구(TOOL) 레시피 개수: %d"), ToolRecipes.Num());
	
	TArray<int32> WeaponRecipes = GetRecipeIDsByCategory(ECraftingCategory::WEAPON);
	UE_LOG(LogCraftingDataSubsystem, Display, TEXT("무기(WEAPON) 레시피 개수: %d"), WeaponRecipes.Num());
	
	TArray<int32> ArmorRecipes = GetRecipeIDsByCategory(ECraftingCategory::ARMOR);
	UE_LOG(LogCraftingDataSubsystem, Display, TEXT("방어구(ARMOR) 레시피 개수: %d"), ArmorRecipes.Num());
	
	TArray<int32> ConsumableRecipes = GetRecipeIDsByCategory(ECraftingCategory::CONSUMABLE);
	UE_LOG(LogCraftingDataSubsystem, Display, TEXT("소모품(CONSUMABLE) 레시피 개수: %d"), ConsumableRecipes.Num());
	
	//TArray<int32> BuildingRecipes = GetRecipeIDsByCategory(ECraftingCategory::BUILDING);
	//UE_LOG(LogCraftingDataSubsystem, Display, TEXT("건축(BUILDING) 레시피 개수: %d"), BuildingRecipes.Num());
	
	// 5. 특수 필터링 테스트
	// UE_LOG(LogCraftingDataSubsystem, Display, TEXT("\n--- 특수 필터링 테스트 ---"));
	//
	// TArray<int32> UnlockedRecipes = GetUnlockedRecipeIDs();
	// UE_LOG(LogCraftingDataSubsystem, Display, TEXT("잠금 해제된 레시피: %d"), UnlockedRecipes.Num());
	
	// 6. 재료/결과물 검색 테스트
	UE_LOG(LogCraftingDataSubsystem, Display, TEXT("\n--- 재료/결과물 검색 테스트 ---"));
	
	// MaterialID 2를 사용하는 레시피 검색
	TArray<int32> RecipesWithMaterial2 = GetRecipeIDsByMaterial(2);
	UE_LOG(LogCraftingDataSubsystem, Display, TEXT("MaterialID 2를 사용하는 레시피: %d개"), RecipesWithMaterial2.Num());
	if (RecipesWithMaterial2.Num() > 0)
	{
		UE_LOG(LogCraftingDataSubsystem, Display, TEXT("  예시: RecipeID %d"), RecipesWithMaterial2[0]);
	}
	
	// ResultItemID 200을 결과물로 하는 레시피 검색
	TArray<int32> RecipesForItem200 = GetRecipeIDsByResultItem(200);
	UE_LOG(LogCraftingDataSubsystem, Display, TEXT("ResultItemID 200을 만드는 레시피: %d개"), RecipesForItem200.Num());
	if (RecipesForItem200.Num() > 0)
	{
		UE_LOG(LogCraftingDataSubsystem, Display, TEXT("  예시: RecipeID %d"), RecipesForItem200[0]);
	}
	
	// 7. 제작 가능 여부 검증 테스트
	UE_LOG(LogCraftingDataSubsystem, Display, TEXT("\n--- 제작 가능 여부 테스트 ---"));
	
	if (IsCraftingDataValid(1))
	{
		UE_LOG(LogCraftingDataSubsystem, Display, TEXT("✅ RecipeID 1 유효함"));
		if (IsRecipeAvailable(1))
		{
			UE_LOG(LogCraftingDataSubsystem, Display, TEXT("  ✅ 제작 가능 (잠금 해제됨)"));
		}
		else
		{
			UE_LOG(LogCraftingDataSubsystem, Display, TEXT("  ❌ 제작 불가능 (잠김 또는 데이터 오류)"));
		}
	}
	else
	{
		UE_LOG(LogCraftingDataSubsystem, Warning, TEXT("❌ RecipeID 1 유효하지 않음"));
	}
	
	UE_LOG(LogCraftingDataSubsystem, Display, TEXT("========================================"));
	UE_LOG(LogCraftingDataSubsystem, Display, TEXT("테스트 완료"));
	UE_LOG(LogCraftingDataSubsystem, Display, TEXT("========================================"));
#endif
}
#pragma endregion

#pragma region InternalCachingFunctions
//========================================
// Internal Caching Functions
//========================================

// 제작 레시피 데이터를 캐시에 저장
void UCraftingDataSubsystem::CacheCraftingData()
{
	// TableAsset 또는 CraftingDataTable이 유효하지 않으면 경고 후 종료
	if (!TableAsset || !TableAsset->CraftingDataTable)
	{
		UE_LOG(LogCraftingDataSubsystem, Warning, TEXT("CacheCraftingData: 유효하지 않은 CraftingDataTable"));
		return;
	}
	
	const UDataTable* DataTable = TableAsset->CraftingDataTable;
	TArray<FCraftingData*> AllRows;
	// 데이터테이블에서 모든 레시피 데이터 가져오기
	DataTable->GetAllRows<FCraftingData>(TEXT("CacheCraftingData"), AllRows);
	
	for (FCraftingData* RowData : AllRows)
	{
		// 유효한 레시피 ID를 가진 데이터만 캐시에 추가
		if (RowData && RowData->RecipeID > 0)
		{
			CraftingDataCache.Add(RowData->RecipeID, *RowData);
		}
		else
		{
			UE_LOG(LogCraftingDataSubsystem, Warning, TEXT("유효하지 않은 CraftingData 행 (ID: %d)"), 
				RowData ? RowData->RecipeID : -1);
		}
	}
	
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	UE_LOG(LogCraftingDataSubsystem, Log, TEXT("총 %d개의 제작 레시피 캐싱 완료"), CraftingDataCache.Num());
#endif
}

// 모든 캐시 데이터를 초기화
void UCraftingDataSubsystem::ClearAllCaches()
{
	// 레시피 캐시 비우기
	CraftingDataCache.Empty();
	
	UE_LOG(LogCraftingDataSubsystem, Log, TEXT("All caches cleared"));
}

// TableAsset이 유효한지 검증
bool UCraftingDataSubsystem::ValidateTableAsset(const UCraftingTableAsset* InTableAsset) const
{
	// TableAsset이 null인지 확인
	if (!InTableAsset)
	{
		UE_LOG(LogCraftingDataSubsystem, Error, TEXT("TableAsset이 nullptr입니다"));
		return false;
	}
	
	bool bIsValid = true;
	
	// 제작 레시피 데이터 테이블 존재 여부 확인
	if (!InTableAsset->CraftingDataTable)
	{
		UE_LOG(LogCraftingDataSubsystem, Error, TEXT("TableAsset에 CraftingDataTable이 설정되어 있지 않습니다!"));
		bIsValid = false;
	}

	return bIsValid;
}
#pragma endregion