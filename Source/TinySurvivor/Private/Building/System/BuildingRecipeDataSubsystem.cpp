// BuildingRecipeDataSubsystem.cpp
#include "Building/System/BuildingRecipeDataSubsystem.h"

#include "Building/System/BuildingRecipeSystemSettings.h"
#include "Engine/Engine.h"

// 로그 카테고리 정의
DEFINE_LOG_CATEGORY_STATIC(LogBuildingRecipeDataSubsystem, Log, All);

//========================================
// Static Empty Data (빈 데이터 반환용)
//========================================
namespace
{
	static const FBuildingRecipeData EmptyBuildingRecipeData;
}

//========================================
// Constructor
//========================================
UBuildingRecipeDataSubsystem::UBuildingRecipeDataSubsystem()
	: bIsInitialized(false)
{
}

#pragma region Lifecycle
//========================================
// Subsystem Lifecycle
//========================================
void UBuildingRecipeDataSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	UE_LOG(LogBuildingRecipeDataSubsystem, Log, TEXT("BuildingRecipeDataSubsystem Initialized"));
	
	// 네트워크 환경 확인 (디버그용)
	if (const UWorld* World = GetWorld())
	{
		const ENetMode NetMode = World->GetNetMode();
		UE_LOG(LogBuildingRecipeDataSubsystem, Log, TEXT("  NetMode: %s"),
			NetMode == NM_Standalone ? TEXT("Standalone") :
			NetMode == NM_DedicatedServer ? TEXT("DedicatedServer") :
			NetMode == NM_ListenServer ? TEXT("ListenServer") :
			NetMode == NM_Client ? TEXT("Client") : TEXT("Unknown"));
	}
#endif
	
	// Project Settings에서 자동 로드
	const UBuildingRecipeSystemSettings* Settings = UBuildingRecipeSystemSettings::Get();
	if (!Settings)
	{
		UE_LOG(LogBuildingRecipeDataSubsystem, Error, TEXT("BuildingRecipeSystemSettings를 찾을 수 없습니다!"));
		return;
	}
	
	// Soft Reference 동기 로드
	UBuildingRecipeTableAsset* LoadedAsset = Settings->BuildingRecipeTableAsset.LoadSynchronous();
	if (!LoadedAsset)
	{
		UE_LOG(LogBuildingRecipeDataSubsystem, Error, TEXT("BuildingRecipeTableAsset 로드 실패! Project Settings > Building Recipe System에서 설정하세요."));
		return;
	}
	
	// 자동 초기화
	if (InitializeFromAsset(LoadedAsset))
	{
		UE_LOG(LogBuildingRecipeDataSubsystem, Log, TEXT("BuildingRecipeDataSubsystem 자동 초기화 성공"));
	}
	else
	{
		UE_LOG(LogBuildingRecipeDataSubsystem, Error, TEXT("BuildingRecipeDataSubsystem 초기화 실패"));
	}
}

void UBuildingRecipeDataSubsystem::Deinitialize()
{
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	UE_LOG(LogBuildingRecipeDataSubsystem, Log, TEXT("BuildingRecipeDataSubsystem Deinitialized"));
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
UBuildingRecipeDataSubsystem* UBuildingRecipeDataSubsystem::GetBuildingRecipeDataSubsystem(const UObject* WorldContext)
{
	if (!WorldContext)
	{
		UE_LOG(LogBuildingRecipeDataSubsystem, Warning, TEXT("GetBuildingRecipeDataSubsystem: WorldContext가 null입니다"));
		return nullptr;
	}
	
	// WorldContext에서 UWorld 객체 가져오기
	const UWorld* World = GEngine->GetWorldFromContextObject(
		WorldContext, 
		EGetWorldErrorMode::LogAndReturnNull
	);
	
	if (!World)
	{
		UE_LOG(LogBuildingRecipeDataSubsystem, Warning, TEXT("GetBuildingRecipeDataSubsystem: WorldContext에서 UWorld를 가져오지 못했습니다"));
		return nullptr;
	}
	
	// World에서 GameInstance 가져오기
	UGameInstance* GameInstance = World->GetGameInstance();
	if (!GameInstance)
	{
		UE_LOG(LogBuildingRecipeDataSubsystem, Warning, TEXT("GetBuildingRecipeDataSubsystem: GameInstance가 null입니다"));
		return nullptr;
	}
	
	// GameInstance에서 BuildingRecipeDataSubsystem 가져오기
	UBuildingRecipeDataSubsystem* Subsystem = GameInstance->GetSubsystem<UBuildingRecipeDataSubsystem>();
	if (!Subsystem)
	{
		UE_LOG(LogBuildingRecipeDataSubsystem, Error, TEXT("GetBuildingRecipeDataSubsystem: Subsystem을 찾을 수 없습니다!"));
		return nullptr;
	}
	
	// 서브시스템 초기화 확인
	if (!Subsystem->IsInitialized())
	{
		UE_LOG(LogBuildingRecipeDataSubsystem, Warning, 
			TEXT("GetBuildingRecipeDataSubsystem: Subsystem은 찾았지만 아직 초기화되지 않았습니다. 먼저 InitializeFromAsset()를 호출하세요."));
	}
	
	return Subsystem;
}
#pragma endregion

#pragma region Initialization
//========================================
// Initialization
//========================================
bool UBuildingRecipeDataSubsystem::InitializeFromAsset(UBuildingRecipeTableAsset* InTableAsset)
{
	if (!ValidateTableAsset(InTableAsset))
	{// 테이블 에셋 유효성 검사
		UE_LOG(LogBuildingRecipeDataSubsystem, Error, TEXT("초기화 실패: 유효하지 않은 TableAsset"));
		return false;
	}
	
	// 기존 캐시 초기화
	ClearAllCaches();
	
	// 데이터 에셋 저장
	TableAsset = InTableAsset;
	
	// 레시피 데이터 캐싱
	CacheBuildingRecipeData();
	
	// 초기화 완료 플래그 설정
	bIsInitialized = true;
	
#if UE_BUILD_DEVELOPMENT || UE_BUILD_DEBUG
	// 개발/디버그 빌드에서 캐시 상태 로그 출력
	UE_LOG(LogBuildingRecipeDataSubsystem, Log, TEXT("BuildingRecipeDataSubsystem 초기화 성공"));
	UE_LOG(LogBuildingRecipeDataSubsystem, Log, TEXT("- Recipes: %d"), BuildingRecipeDataCache.Num());
	
	// 에디터에서만 자동 테스트 실행

	RunInitializationTests();
#endif
	
	return true;
}
#pragma endregion

#pragma region BuildingRecipeDataGetters
//========================================
// Building Recipe Data Getters
//========================================
const FBuildingRecipeData& UBuildingRecipeDataSubsystem::GetBuildingRecipeData(int32 RecipeID) const
{
	if (!bIsInitialized)
	{// 초기화 여부 확인
		UE_LOG(LogBuildingRecipeDataSubsystem, Warning, TEXT("GetBuildingRecipeData 호출 시점: 아직 초기화되지 않음"));
		return EmptyBuildingRecipeData;
	}
	
	if (const FBuildingRecipeData* Found = BuildingRecipeDataCache.Find(RecipeID))
	{// 캐시에서 레시피 데이터 검색
		return *Found;
	}
	
	// 레시피 ID를 캐시에서 찾지 못했을 경우 경고 로그 출력
	UE_LOG(LogBuildingRecipeDataSubsystem, Warning, TEXT("RecipeID %d를 캐시에서 찾을 수 없음"), RecipeID);
	return EmptyBuildingRecipeData;
}

bool UBuildingRecipeDataSubsystem::GetBuildingRecipeDataSafe(int32 RecipeID, FBuildingRecipeData& OutData) const
{
	if (!bIsInitialized)
	{// 초기화 여부 확인
		UE_LOG(LogBuildingRecipeDataSubsystem, Warning, TEXT("GetBuildingRecipeDataSafe 호출 시점: 아직 초기화되지 않음"));
		return false;
	}
	
	if (const FBuildingRecipeData* Found = BuildingRecipeDataCache.Find(RecipeID))
	{// 캐시에서 레시피 데이터 검색
		OutData = *Found; // 안전하게 OutData에 복사
		return true;
	}
	
	// 레시피 ID를 캐시에서 찾지 못했을 경우 false 반환
	return false;
}

TArray<int32> UBuildingRecipeDataSubsystem::GetAllRecipeIDs() const
{
	// 캐시에 저장된 모든 레시피 ID를 배열로 가져오기
	TArray<int32> RecipeIDs;
	BuildingRecipeDataCache.GetKeys(RecipeIDs);
	return RecipeIDs;
}

TArray<int32> UBuildingRecipeDataSubsystem::GetRecipeIDsByCategory(EBuildingCategory Category) const
{
	// 지정한 카테고리에 속하는 레시피 ID만 필터링하여 반환
	TArray<int32> FilteredIDs;
	
	// 캐시에 저장된 모든 레시피를 순회
	for (const TPair<int32, FBuildingRecipeData>& Pair : BuildingRecipeDataCache)
	{
		if (Pair.Value.Category == Category)
		{// 레시피의 카테고리가 요청한 카테고리와 일치하면 배열에 추가
			FilteredIDs.Add(Pair.Key);
		}
	}
	
	return FilteredIDs;
}

TArray<int32> UBuildingRecipeDataSubsystem::GetRecipeIDsByResultItem(int32 ResultItemID) const
{
	// 특정 아이템을 결과물로 하는 레시피 필터링
	TArray<int32> FilteredIDs;
	
	for (const TPair<int32, FBuildingRecipeData>& Pair : BuildingRecipeDataCache)
	{
		if (Pair.Value.ResultItemID == ResultItemID)
		{
			FilteredIDs.Add(Pair.Key);
		}
	}
	
	return FilteredIDs;
}

TArray<int32> UBuildingRecipeDataSubsystem::GetRecipeIDsByMaterial(int32 MaterialID) const
{
	// 특정 재료를 사용하는 레시피 필터링
	TArray<int32> FilteredIDs;
	
	for (const TPair<int32, FBuildingRecipeData>& Pair : BuildingRecipeDataCache)
	{
		if (Pair.Value.ContainsMaterial(MaterialID))
		{
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

// 특정 레시피 ID가 유효한지 확인
bool UBuildingRecipeDataSubsystem::IsBuildingRecipeDataValid(int32 RecipeID) const
{
	// 초기화 여부 확인 후, 캐시에 해당 ID가 존재하는지 반환
	return bIsInitialized && BuildingRecipeDataCache.Contains(RecipeID);
}

// 레시피 데이터가 올바른지 검증
bool UBuildingRecipeDataSubsystem::IsRecipeAvailable(int32 RecipeID) const
{
	if (!bIsInitialized)
	{
		return false;
	}
	
	if (const FBuildingRecipeData* Found = BuildingRecipeDataCache.Find(RecipeID))
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
void UBuildingRecipeDataSubsystem::GetCacheStatistics(int32& OutRecipeCount) const
{
	OutRecipeCount = BuildingRecipeDataCache.Num(); // 레시피 캐시에 저장된 데이터 수
}

// 캐시 디버그 정보를 콘솔에 출력
void UBuildingRecipeDataSubsystem::PrintCacheDebugInfo() const
{
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	UE_LOG(LogBuildingRecipeDataSubsystem, Display, TEXT("=== BuildingRecipeDataSubsystem Cache Statistics ==="));
	UE_LOG(LogBuildingRecipeDataSubsystem, Display, TEXT("Initialized: %s"), bIsInitialized ? TEXT("Yes") : TEXT("No"));
	UE_LOG(LogBuildingRecipeDataSubsystem, Display, TEXT("Recipes Cached: %d"), BuildingRecipeDataCache.Num());
	
	// 레시피 카테고리별 통계 출력
	if (bIsInitialized && BuildingRecipeDataCache.Num() > 0)
	{
		UE_LOG(LogBuildingRecipeDataSubsystem, Display, TEXT("\n--- Recipe Categories ---"));
		
		TMap<EBuildingCategory, int32> CategoryCounts;
		for (const TPair<int32, FBuildingRecipeData>& Pair : BuildingRecipeDataCache)
		{
			int32& Count = CategoryCounts.FindOrAdd(Pair.Value.Category, 0);
			Count++;
		}
		
		for (const TPair<EBuildingCategory, int32>& Pair : CategoryCounts)
		{
			FString CategoryName;
			switch (Pair.Key)
			{
				case EBuildingCategory::NONE: CategoryName = TEXT("None"); break;
				case EBuildingCategory::CRAFTING: CategoryName = TEXT("Crafting"); break;
				case EBuildingCategory::LIGHTING: CategoryName = TEXT("lighting"); break;
				case EBuildingCategory::DEFENSE: CategoryName = TEXT("Defense"); break;
				case EBuildingCategory::STORAGE: CategoryName = TEXT("Storage"); break;
				default: CategoryName = TEXT("Unknown"); break;
			}
			UE_LOG(LogBuildingRecipeDataSubsystem, Display, TEXT("  %s: %d"), *CategoryName, Pair.Value);
		}
	}
	
	UE_LOG(LogBuildingRecipeDataSubsystem, Display, TEXT("====================================================="));
#endif
}

void UBuildingRecipeDataSubsystem::PrintRecipeDebugInfo(int32 RecipeID) const
{
	if (!bIsInitialized)
	{
		UE_LOG(LogBuildingRecipeDataSubsystem, Warning, TEXT("[PrintRecipeDebugInfo] BuildingRecipeDataSubsystem is NOT initialized."));
		return;
	}
	
	const FBuildingRecipeData* FoundData = BuildingRecipeDataCache.Find(RecipeID);
	
	if (!FoundData)
	{
		UE_LOG(LogBuildingRecipeDataSubsystem, Warning, TEXT("[PrintRecipeDebugInfo] RecipeID %d not found in BuildingRecipeDataCache."), RecipeID);
		return;
	}
	
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	UE_LOG(LogBuildingRecipeDataSubsystem, Display, TEXT("\n========== Debug Info for RecipeID %d =========="), RecipeID);
	FoundData->PrintDebugInfo();
	UE_LOG(LogBuildingRecipeDataSubsystem, Display, TEXT("====================================================\n"));
#endif
}

// 캐시를 새로고침 (TableAsset 기반으로 다시 로드)
void UBuildingRecipeDataSubsystem::RefreshCache()
{
	// TableAsset이 설정되지 않았으면 경고 후 종료
	if (!TableAsset)
	{
		UE_LOG(LogBuildingRecipeDataSubsystem, Warning, TEXT("RefreshCache: TableAsset이 설정되지 않음"));
		return;
	}
	
	UE_LOG(LogBuildingRecipeDataSubsystem, Log, TEXT("TableAsset에서 캐시 새로고침 시작..."));
	
	// 기존 캐시 초기화 후 데이터 재캐싱
	ClearAllCaches();
	CacheBuildingRecipeData();
	
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	UE_LOG(LogBuildingRecipeDataSubsystem, Log, TEXT("캐시 새로고침 완료"));
	// 새로고침 후 캐시 상태 디버그 출력
	PrintCacheDebugInfo();
#endif
}

void UBuildingRecipeDataSubsystem::RunInitializationTests()
{
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	UE_LOG(LogBuildingRecipeDataSubsystem, Display, TEXT("========================================"));
	UE_LOG(LogBuildingRecipeDataSubsystem, Display, TEXT("BuildingRecipeDataSubsystem 초기화 테스트 시작"));
	UE_LOG(LogBuildingRecipeDataSubsystem, Display, TEXT("========================================"));
	
	// 1. 통계 출력
	PrintCacheDebugInfo();
	
	// 2. 레시피 조회 테스트
	UE_LOG(LogBuildingRecipeDataSubsystem, Display, TEXT("\n--- 레시피 조회 테스트 ---"));
	FBuildingRecipeData RecipeData;
	if (GetBuildingRecipeDataSafe(11, RecipeData))
	{
		UE_LOG(LogBuildingRecipeDataSubsystem, Display, TEXT("✅ 레시피 ID 11 찾음:"));
		UE_LOG(LogBuildingRecipeDataSubsystem, Display, TEXT("  - 건축물 한글명: %s"), *RecipeData.Name_KR.ToString());
		UE_LOG(LogBuildingRecipeDataSubsystem, Display, TEXT("  - 건축물 영문명: %s"), *RecipeData.Name_EN.ToString());
		UE_LOG(LogBuildingRecipeDataSubsystem, Display, TEXT("  - 결과물 ItemID: %d"), RecipeData.ResultItemID);
		UE_LOG(LogBuildingRecipeDataSubsystem, Display, TEXT("  - 건축 시간: %.2f초"), RecipeData.BuildTime);
		UE_LOG(LogBuildingRecipeDataSubsystem, Display, TEXT("  - 재료 종류: %d"), RecipeData.GetIngredientTypeCount());
	}
	else
	{
		UE_LOG(LogBuildingRecipeDataSubsystem, Warning, TEXT("❌ 레시피 ID 11을 찾을 수 없습니다"));
	}
	if (GetBuildingRecipeDataSafe(12, RecipeData))
	{
		UE_LOG(LogBuildingRecipeDataSubsystem, Display, TEXT("✅ 레시피 ID 12 찾음:"));
		PrintRecipeDebugInfo(12);
	}
	else
	{
		UE_LOG(LogBuildingRecipeDataSubsystem, Warning, TEXT("❌ 레시피 ID 12를 찾을 수 없습니다"));
	}
	
	// 3. 존재하지 않는 ID 테스트
	UE_LOG(LogBuildingRecipeDataSubsystem, Display, TEXT("\n--- 존재하지 않는 ID 테스트 ---"));
	if (GetBuildingRecipeDataSafe(9999, RecipeData))
	{
		UE_LOG(LogBuildingRecipeDataSubsystem, Display, TEXT("✅ ID 9999 찾음 (이상함!)"));
	}
	else
	{
		UE_LOG(LogBuildingRecipeDataSubsystem, Display, TEXT("❌ ID 9999를 찾을 수 없습니다 (정상)"));
	}
	
	// 4. 카테고리별 필터링 테스트
	UE_LOG(LogBuildingRecipeDataSubsystem, Display, TEXT("\n--- 카테고리별 필터링 테스트 ---"));
	
	// CRAFTING
	TArray<int32> CraftingRecipes = GetRecipeIDsByCategory(EBuildingCategory::CRAFTING);
	UE_LOG(LogBuildingRecipeDataSubsystem, Display, TEXT("제작(CRAFTING) 레시피 개수: %d"), CraftingRecipes.Num());
	
	// DEFENSE
	TArray<int32> DefenseRecipes = GetRecipeIDsByCategory(EBuildingCategory::DEFENSE);
	UE_LOG(LogBuildingRecipeDataSubsystem, Display, TEXT("방어 시설(DEFENSE) 레시피 개수: %d"), DefenseRecipes.Num());
	
	// STORAGE
	TArray<int32> StorageRecipes = GetRecipeIDsByCategory(EBuildingCategory::STORAGE);
	UE_LOG(LogBuildingRecipeDataSubsystem, Display, TEXT("보관함(STORAGE) 레시피 개수: %d"), StorageRecipes.Num());
	
	// LIGHTING
	TArray<int32> LightingRecipes = GetRecipeIDsByCategory(EBuildingCategory::LIGHTING);
	UE_LOG(LogBuildingRecipeDataSubsystem, Display, TEXT("조명(LIGHTING) 레시피 개수: %d"), LightingRecipes.Num());
	
	// 5. 재료/결과물 검색 테스트
	UE_LOG(LogBuildingRecipeDataSubsystem, Display, TEXT("\n--- 재료/결과물 검색 테스트 ---"));
	
	// MaterialID 4를 사용하는 레시피 검색
	TArray<int32> RecipesWithMaterial4 = GetRecipeIDsByMaterial(4);
	UE_LOG(LogBuildingRecipeDataSubsystem, Display, TEXT("MaterialID 4를 사용하는 레시피: %d개"), RecipesWithMaterial4.Num());
	if (RecipesWithMaterial4.Num() > 0)
	{
		UE_LOG(LogBuildingRecipeDataSubsystem, Display, TEXT("  예시: RecipeID %d"), RecipesWithMaterial4[0]);
	}
	
	// ResultItemID 500을 결과물로 하는 레시피 검색
	TArray<int32> RecipesForItem500 = GetRecipeIDsByResultItem(500);
	UE_LOG(LogBuildingRecipeDataSubsystem, Display, TEXT("ResultItemID 500을 만드는 레시피: %d개"), RecipesForItem500.Num());
	if (RecipesForItem500.Num() > 0)
	{
		UE_LOG(LogBuildingRecipeDataSubsystem, Display, TEXT("  예시: RecipeID %d"), RecipesForItem500[0]);
	}
	
	// 6. 건축 가능 여부 검증 테스트
	UE_LOG(LogBuildingRecipeDataSubsystem, Display, TEXT("\n--- 건축 가능 여부 테스트 ---"));
	
	if (IsBuildingRecipeDataValid(11))
	{
		UE_LOG(LogBuildingRecipeDataSubsystem, Display, TEXT("✅ RecipeID 11 유효함"));
		if (IsRecipeAvailable(11))
		{
			UE_LOG(LogBuildingRecipeDataSubsystem, Display, TEXT("  ✅ 건축 가능"));
		}
		else
		{
			UE_LOG(LogBuildingRecipeDataSubsystem, Display, TEXT("  ❌ 건축 불가능 (데이터 오류)"));
		}
	}
	else
	{
		UE_LOG(LogBuildingRecipeDataSubsystem, Warning, TEXT("❌ RecipeID 11 유효하지 않음"));
	}
	
	UE_LOG(LogBuildingRecipeDataSubsystem, Display, TEXT("========================================"));
	UE_LOG(LogBuildingRecipeDataSubsystem, Display, TEXT("테스트 완료"));
	UE_LOG(LogBuildingRecipeDataSubsystem, Display, TEXT("========================================"));
#endif
}
#pragma endregion

#pragma region InternalCachingFunctions
//========================================
// Internal Caching Functions
//========================================
// 건축물 레시피 데이터를 캐시에 저장
void UBuildingRecipeDataSubsystem::CacheBuildingRecipeData()
{
	// TableAsset 또는 BuildingRecipeDataTable이 유효하지 않으면 경고 후 종료
	if (!TableAsset || !TableAsset->BuildingRecipeDataTable)
	{
		UE_LOG(LogBuildingRecipeDataSubsystem, Warning, TEXT("CacheBuildingRecipeData: 유효하지 않은 BuildingRecipeDataTable"));
		return;
	}
	
	const UDataTable* DataTable = TableAsset->BuildingRecipeDataTable;
	TArray<FBuildingRecipeData*> AllRows;
	// 데이터테이블에서 모든 레시피 데이터 가져오기
	DataTable->GetAllRows<FBuildingRecipeData>(TEXT("CacheBuildingRecipeData"), AllRows);
	
	for (FBuildingRecipeData* RowData : AllRows)
	{
		// 유효한 레시피 ID를 가진 데이터만 캐시에 추가
		if (RowData && RowData->RecipeID > 0)
		{
			BuildingRecipeDataCache.Add(RowData->RecipeID, *RowData);
		}
		else
		{
			UE_LOG(LogBuildingRecipeDataSubsystem, Warning, TEXT("유효하지 않은 BuildingRecipeData 행 (ID: %d)"), 
				RowData ? RowData->RecipeID : -1);
		}
	}
	
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	UE_LOG(LogBuildingRecipeDataSubsystem, Log, TEXT("총 %d개의 건축물 레시피 캐싱 완료"), BuildingRecipeDataCache.Num());
#endif
}

// 모든 캐시 데이터를 초기화
void UBuildingRecipeDataSubsystem::ClearAllCaches()
{
	// 레시피 캐시 비우기
	BuildingRecipeDataCache.Empty();
	UE_LOG(LogBuildingRecipeDataSubsystem, Log, TEXT("All caches cleared"));
}

// TableAsset이 유효한지 검증
bool UBuildingRecipeDataSubsystem::ValidateTableAsset(const UBuildingRecipeTableAsset* InTableAsset) const
{
	// TableAsset이 null인지 확인
	if (!InTableAsset)
	{
		UE_LOG(LogBuildingRecipeDataSubsystem, Error, TEXT("TableAsset이 nullptr입니다"));
		return false;
	}
	bool bIsValid = true;
	
	// 건축물 레시피 데이터 테이블 존재 여부 확인
	if (!InTableAsset->BuildingRecipeDataTable)
	{
		UE_LOG(LogBuildingRecipeDataSubsystem, Error, TEXT("TableAsset에 BuildingRecipeDataTable이 설정되어 있지 않습니다!"));
		bIsValid = false;
	}
	
	return bIsValid;
}
#pragma endregion
