// BuildingRecipeDataSubsystem.h
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Building/Data/BuildingRecipeData.h"
#include "Building/Data/BuildingRecipeTableAsset.h"
#include "BuildingRecipeDataSubsystem.generated.h"

/*
	건축물 레시피 데이터를 관리하고 캐싱하는 GameInstance Subsystem
	
	주요 기능:
	- 데이터 테이블에서 레시피 데이터를 초기화하고 캐싱
	- Blueprint 및 코드에서 쉽게 접근 가능하도록 Getter 제공
	- 레시피 유효성 검사 기능 포함
	- 건축 가능 여부 확인 기능 제공
	
	초기화 방식:
	- GameMode 또는 ProjectSettings에서 InitializeFromAsset() 호출
	- 리슨 서버 환경: 서버에서 초기화, 클라이언트도 동일 에셋 로드
	
	네트워크 고려사항:
	- 모든 Getter 함수는 const로 선언되어 로컬 캐시만 조회
	- 서버/클라이언트 구분 없이 안전하게 사용 가능
	- 복제(Replication) 불필요 - 각 머신이 동일한 데이터를 로컬에 캐싱
*/
UCLASS()
class TINYSURVIVOR_API UBuildingRecipeDataSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
#pragma region ConstructorAndLifecycle
public:
	UBuildingRecipeDataSubsystem();
	
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
	TObjectPtr<UBuildingRecipeTableAsset> TableAsset;
#pragma endregion
	
#pragma region CachedData
protected:
	//========================================
	// Cached Data
	//========================================
	
	// 캐싱된 건축물 레시피 데이터 (Key: RecipeID)
	UPROPERTY()
	TMap<int32, FBuildingRecipeData> BuildingRecipeDataCache;
	
	// 초기화 완료 여부 플래그
	bool bIsInitialized;
#pragma endregion
	
#pragma region StaticAccessHelper
public:
	//========================================
	// Static Access Helper
	//========================================
	
	/*
		편의성 함수: GameInstance에서 BuildingRecipeDataSubsystem 가져오기
		Blueprint와 C++ 모두에서 안전하게 접근 가능
		
		@param WorldContext 월드 컨텍스트 (Character, GameMode, Widget 등)
		@return BuildingRecipeDataSubsystem 인스턴스 (nullptr 가능)
		
		사용 예시 (C++):
			if (UBuildingRecipeDataSubsystem* BuildingRecipeSystem = UBuildingRecipeDataSubsystem::GetBuildingRecipeDataSubsystem(this))
			{
				FBuildingRecipeData RecipeData;
				if (BuildingRecipeSystem->GetBuildingRecipeDataSafe(RecipeID, RecipeData))
				{
					// 레시피 사용
				}
			}
		
		사용 예시 (Blueprint):
			Get Building Recipe Data Subsystem → Branch (Is Valid) → Get Building Recipe Data Safe
	*/
	UFUNCTION(BlueprintPure, Category="BuildingRecipeData", 
		meta=(WorldContext="WorldContext", DisplayName="Get Building Recipe Data Subsystem"))
	static UBuildingRecipeDataSubsystem* GetBuildingRecipeDataSubsystem(const UObject* WorldContext);
#pragma endregion
	
#pragma region Initialization
public:
	//========================================
	// Initialization
	//========================================
	
	/*
		데이터 에셋으로부터 시스템 초기화
		GameInstance::Init()에서 호출되어야 함
		
		@param InTableAsset 건축물 레시피 데이터 테이블을 포함한 데이터 에셋
		@return 초기화 성공 여부
	*/
	UFUNCTION(BlueprintCallable, Category="BuildingRecipeData")
	bool InitializeFromAsset(UBuildingRecipeTableAsset* InTableAsset);
	
	/*
		초기화 완료 여부 확인
		@return 초기화되었으면 true
	*/
	UFUNCTION(BlueprintPure, Category="BuildingRecipeData")
	bool IsInitialized() const { return bIsInitialized; }
#pragma endregion
	
#pragma region BuildingRecipeDataGetters
public:
	//========================================
	// Building Recipe Data Getters
	//========================================
	
	/*
		건축물 레시피 데이터 조회 (ID가 확실히 존재할 때만 사용)
		주의: 존재하지 않는 ID 조회 시 빈 데이터 반환
		안전한 조회가 필요하면 GetBuildingRecipeDataSafe() 사용 권장
		
		@param RecipeID 조회할 레시피 ID
		@return 레시피 데이터 (존재하지 않으면 빈 데이터)
	*/
	UFUNCTION(BlueprintPure, Category="BuildingRecipeData")
	const FBuildingRecipeData& GetBuildingRecipeData(int32 RecipeID) const;
	
	/*
		건축물 레시피 데이터 조회 (안전 버전 - 권장)
		@param RecipeID 조회할 레시피 ID
		@param OutData 출력될 레시피 데이터
		@return 데이터가 존재하면 true, 없으면 false
	*/
	UFUNCTION(BlueprintPure, Category="BuildingRecipeData")
	bool GetBuildingRecipeDataSafe(int32 RecipeID, FBuildingRecipeData& OutData) const;
	
	/*
		모든 레시피 ID 목록 조회
		@return 캐싱된 모든 레시피 ID 배열
	*/
	UFUNCTION(BlueprintPure, Category="BuildingRecipeData")
	TArray<int32> GetAllRecipeIDs() const;
	
	/*
		카테고리별 레시피 ID 필터링
		@param Category 필터링할 카테고리
		@return 해당 카테고리의 레시피 ID 배열
	*/
	UFUNCTION(BlueprintPure, Category="BuildingRecipeData")
	TArray<int32> GetRecipeIDsByCategory(EBuildingCategory Category) const;
	
	/*
		특정 아이템을 결과물로 하는 레시피 찾기
		@param ResultItemID 결과물 아이템 ID
		@return 해당 아이템을 만드는 레시피 ID 배열
	*/
	UFUNCTION(BlueprintPure, Category="BuildingRecipeData")
	TArray<int32> GetRecipeIDsByResultItem(int32 ResultItemID) const;
	
	/*
		특정 재료를 사용하는 레시피 찾기
		@param MaterialID 재료 아이템 ID
		@return 해당 재료를 사용하는 레시피 ID 배열
	*/
	UFUNCTION(BlueprintPure, Category="BuildingRecipeData")
	TArray<int32> GetRecipeIDsByMaterial(int32 MaterialID) const;
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
	UFUNCTION(BlueprintPure, Category="BuildingRecipeData")
	bool IsBuildingRecipeDataValid(int32 RecipeID) const;
	
	/*
		레시피가 건축 가능한지 기본 검증 (레시피 존재 여부)
		@param RecipeID 확인할 레시피 ID
		@return 건축 가능하면 true
	*/
	UFUNCTION(BlueprintPure, Category="BuildingRecipeData")
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
	UFUNCTION(BlueprintPure, Category="BuildingRecipeData")
	void GetCacheStatistics(int32& OutRecipeCount) const;
	
	/*
		디버그용 캐시 정보 로그 출력
	*/
	UFUNCTION(BlueprintCallable, Category="BuildingRecipeData|Debug")
	void PrintCacheDebugInfo() const;
	
	/*
		디버그용 레시피 정보 로그 출력
	*/
	UFUNCTION(BlueprintCallable, Category="BuildingRecipeData|Debug")
	void PrintRecipeDebugInfo(int32 RecipeID) const;
	
#if WITH_EDITOR
	/*
		에디터 전용: 캐시 리프레시
		데이터 테이블 변경 후 에디터에서 실시간 반영용
	*/
	UFUNCTION(CallInEditor, Category="BuildingRecipeData|Editor")
	void RefreshCache();
	
	/*
		에디터 전용: 초기화 후 자동 테스트
		InitializeFromAsset() 완료 후 호출됨
	*/
	void RunInitializationTests();
#endif
#pragma endregion
	
#pragma region InternalCachingFunctions
private:
	//========================================
	// Internal Caching Functions
	//========================================
	
	/*
		건축물 레시피 데이터 테이블을 TMap으로 캐싱
	*/
	void CacheBuildingRecipeData();
	
	/*
		모든 캐시 초기화
	*/
	void ClearAllCaches();
	
	/*
		데이터 에셋 유효성 검증
		@param InTableAsset 검증할 데이터 에셋
		@return 유효하면 true
	*/
	bool ValidateTableAsset(const UBuildingRecipeTableAsset* InTableAsset) const;
#pragma endregion
};
