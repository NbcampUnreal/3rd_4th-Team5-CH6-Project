// CraftingDataSubsystem.h
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Crafting/Data/CraftingData.h"
#include "Crafting/Data/CraftingTableAsset.h"
#include "CraftingDataSubsystem.generated.h"

/*
	제작 레시피 데이터를 관리하고 캐싱하는 GameInstance Subsystem
	
	주요 기능:
	- 데이터 테이블에서 레시피 데이터를 초기화하고 캐싱
	- Blueprint 및 코드에서 쉽게 접근 가능하도록 Getter 제공
	- 레시피 유효성 검사 기능 포함
	- 제작 가능 여부 확인 기능 제공
	
	초기화 방식:
	- GameMode 또는 ProjectSettings에서 InitializeFromAsset() 호출
	- 리슨 서버 환경: 서버에서 초기화, 클라이언트도 동일 에셋 로드
	
	네트워크 고려사항:
	- 모든 Getter 함수는 const로 선언되어 로컬 캐시만 조회
	- 서버/클라이언트 구분 없이 안전하게 사용 가능
	- 복제(Replication) 불필요 - 각 머신이 동일한 데이터를 로컬에 캐싱
*/
UCLASS()
class TINYSURVIVOR_API UCraftingDataSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
#pragma region ConstructorAndLifecycle
public:
	UCraftingDataSubsystem();

	//========================================
	// Subsystem Lifecycle
	//========================================
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
#pragma endregion

#pragma region DataAssets
protected:
	//========================================
	// Data Assets
	//========================================
	// 데이터 테이블을 보유한 데이터 에셋 참조
	UPROPERTY()
	TObjectPtr<UCraftingTableAsset> TableAsset;
#pragma endregion

#pragma region CachedData
protected:
	//========================================
	// Cached Data
	//========================================
	
	// 캐싱된 제작 레시피 데이터 (Key: RecipeID)
	UPROPERTY()
	TMap<int32, FCraftingData> CraftingDataCache;

	// 초기화 완료 여부 플래그
	bool bIsInitialized;
#pragma endregion

#pragma region StaticAccessHelper
public:
	//========================================
	// Static Access Helper
	//========================================
	
	/*
		편의성 함수: GameInstance에서 CraftingDataSubsystem 가져오기
		Blueprint와 C++ 모두에서 안전하게 접근 가능
		
		@param WorldContext 월드 컨텍스트 (Character, GameMode, Widget 등)
		@return CraftingDataSubsystem 인스턴스 (nullptr 가능)
		
		사용 예시 (C++):
			if (UCraftingDataSubsystem* CraftingDataSystem = UCraftingDataSubsystem::GetCraftingDataSubsystem(this))
			{
				FCraftingData RecipeData;
				if (CraftingDataSystem->GetCraftingDataSafe(RecipeID, RecipeData))
				{
					// 레시피 사용
				}
			}
		
		사용 예시 (Blueprint):
			Get Crafting Data Subsystem → Branch (Is Valid) → Get Crafting Data Safe
	*/
	UFUNCTION(BlueprintPure, Category="CraftingData", 
		meta=(WorldContext="WorldContext", DisplayName="Get Crafting Data Subsystem"))
	static UCraftingDataSubsystem* GetCraftingDataSubsystem(const UObject* WorldContext);
#pragma endregion
	
#pragma region Initialization
public:
	//========================================
	// Initialization
	//========================================
	/*
		데이터 에셋으로부터 시스템 초기화
		GameInstance::Init()에서 호출되어야 함
		
		@param InTableAsset 제작 레시피 데이터 테이블을 포함한 데이터 에셋
		@return 초기화 성공 여부
	*/
	UFUNCTION(BlueprintCallable, Category="CraftingData")
	bool InitializeFromAsset(UCraftingTableAsset* InTableAsset);

	/*
		초기화 완료 여부 확인
		@return 초기화되었으면 true
	*/
	UFUNCTION(BlueprintPure, Category="CraftingData")
	bool IsInitialized() const { return bIsInitialized; }
#pragma endregion

#pragma region CraftingDataGetters
public:
	//========================================
	// Crafting Data Getters
	//========================================

	/*
		제작 레시피 데이터 조회 (ID가 확실히 존재할 때만 사용)
		주의: 존재하지 않는 ID 조회 시 빈 데이터 반환
		안전한 조회가 필요하면 GetCraftingDataSafe() 사용 권장
		
		@param RecipeID 조회할 레시피 ID
		@return 레시피 데이터 (존재하지 않으면 빈 데이터)
	*/
	UFUNCTION(BlueprintPure, Category="CraftingData")
	const FCraftingData& GetCraftingData(int32 RecipeID) const;

	/*
		제작 레시피 데이터 조회 (안전 버전 - 권장)
		@param RecipeID 조회할 레시피 ID
		@param OutData 출력될 레시피 데이터
		@return 데이터가 존재하면 true, 없으면 false
	*/
	UFUNCTION(BlueprintPure, Category="CraftingData")
	bool GetCraftingDataSafe(int32 RecipeID, FCraftingData& OutData) const;

	/*
		모든 레시피 ID 목록 조회
		@return 캐싱된 모든 레시피 ID 배열
	*/
	UFUNCTION(BlueprintPure, Category="CraftingData")
	TArray<int32> GetAllRecipeIDs() const;

	/*
		카테고리별 레시피 ID 필터링
		@param Category 필터링할 카테고리
		@return 해당 카테고리의 레시피 ID 배열
	*/
	UFUNCTION(BlueprintPure, Category="CraftingData")
	TArray<int32> GetRecipeIDsByCategory(ECraftingCategory Category) const;

	/*
		특정 아이템을 결과물로 하는 레시피 찾기
		@param ResultItemID 결과물 아이템 ID
		@return 해당 아이템을 만드는 레시피 ID 배열
	*/
	UFUNCTION(BlueprintPure, Category="CraftingData")
	TArray<int32> GetRecipeIDsByResultItem(int32 ResultItemID) const;

	/*
		특정 재료를 사용하는 레시피 찾기
		@param MaterialID 재료 아이템 ID
		@return 해당 재료를 사용하는 레시피 ID 배열
	*/
	UFUNCTION(BlueprintPure, Category="CraftingData")
	TArray<int32> GetRecipeIDsByMaterial(int32 MaterialID) const;

	/*
		제작대가 필요 없는 레시피 목록 (손제작 가능)
		@return 손제작 가능한 레시피 ID 배열
	*/
	// UFUNCTION(BlueprintPure, Category="CraftingData")
	// TArray<int32> GetHandCraftableRecipeIDs() const;

	/*
		특정 제작대에서 제작 가능한 레시피 목록
		@param CraftingStationID 제작대 ID
		@return 해당 제작대에서 제작 가능한 레시피 ID 배열
	*/
	// UFUNCTION(BlueprintPure, Category="CraftingData")
	// TArray<int32> GetRecipeIDsByCraftingStation(int32 CraftingStationID) const;

	/*
		잠금 해제된 레시피 목록만 조회
		@return 잠금 해제된 레시피 ID 배열
	*/
	// UFUNCTION(BlueprintPure, Category="CraftingData")
	// TArray<int32> GetUnlockedRecipeIDs() const;
#pragma endregion

#pragma region Validation
public:
	//========================================
	// Validation
	//========================================
	
	/*
		레시피 데이터 유효성 확인
		@param RecipeID 확인할 레시피 ID
		@return 유효한 데이터가 존재하면 true
	*/
	UFUNCTION(BlueprintPure, Category="CraftingData")
	bool IsCraftingDataValid(int32 RecipeID) const;

	/*
		레시피가 잠금 해제되었는지 확인
		@param RecipeID 확인할 레시피 ID
		@return 잠금 해제되었으면 true
	*/
	// UFUNCTION(BlueprintPure, Category="CraftingData")
	// bool IsRecipeUnlocked(int32 RecipeID) const;

	/*
		레시피가 제작 가능한지 기본 검증 (레시피 존재 + 잠금 해제)
		@param RecipeID 확인할 레시피 ID
		@return 제작 가능하면 true
	*/
	UFUNCTION(BlueprintPure, Category="CraftingData")
	bool IsRecipeAvailable(int32 RecipeID) const;
#pragma endregion
	
#pragma region Debug
public:
	//========================================
	// Statistics & Debug
	//========================================
	
	/*
		캐싱된 데이터 통계 정보 조회
		@param OutRecipeCount 레시피 개수
	*/
	UFUNCTION(BlueprintPure, Category="CraftingData")
	void GetCacheStatistics(int32& OutRecipeCount) const;
	
	/*
		디버그용 캐시 정보 로그 출력
	*/
	UFUNCTION(BlueprintCallable, Category="CraftingData|Debug")
	void PrintCacheDebugInfo() const;
	
	/*
		디버그용 레시피 정보 로그 출력
	*/
	UFUNCTION(BlueprintCallable, Category="CraftingData|Debug")
	void PrintRecipeDebugInfo(int32 RecipeID) const;
	
	/*
		캐시 리프레시
		데이터 테이블 변경 후 에디터에서 실시간 반영용
	*/
	UFUNCTION(CallInEditor, Category="CraftingData|Editor")
	void RefreshCache();
	
	/*
		초기화 후 자동 테스트
		InitializeFromAsset() 완료 후 호출됨
	*/
	void RunInitializationTests();
#pragma endregion

#pragma region InternalCachingFunctions
private:
	//========================================
	// Internal Caching Functions
	//========================================
	/*
		제작 레시피 데이터 테이블을 TMap으로 캐싱
	*/
	void CacheCraftingData();

	/*
		모든 캐시 초기화
	*/
	void ClearAllCaches();

	/*
		데이터 에셋 유효성 검증
		@param InTableAsset 검증할 데이터 에셋
		@return 유효하면 true
	*/
	bool ValidateTableAsset(const UCraftingTableAsset* InTableAsset) const;
#pragma endregion
};
