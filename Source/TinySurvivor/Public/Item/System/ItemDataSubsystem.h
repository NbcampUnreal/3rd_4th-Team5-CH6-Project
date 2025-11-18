// ItemDataSubsystem.h
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Item/Data/ItemData.h"
#include "Item/Data/BuildingData.h"
#include "Item/Data/ResourceData.h"
#include "Item/Data/ItemTableAsset.h"
#include "ItemDataSubsystem.generated.h"

/*
	아이템, 건물, 자원 데이터를 관리하고 캐싱하는 GameInstance Subsystem
	
	주요 기능:
	- 데이터 테이블에서 데이터를 초기화하고 캐싱
	- Blueprint 및 코드에서 쉽게 접근 가능하도록 Getter 제공
	- 데이터 유효성 검사 기능 포함
	
	초기화 방식:
	- GameMode 또는 ProjectSettings에서 InitializeFromAsset() 호출
	- 리슨 서버 환경: 서버에서 초기화, 클라이언트도 동일 에셋 로드
	
	네트워크 고려사항:
	- 모든 Getter 함수는 const로 선언되어 로컬 캐시만 조회
	- 서버/클라이언트 구분 없이 안전하게 사용 가능
	- 복제(Replication) 불필요 - 각 머신이 동일한 데이터를 로컬에 캐싱
*/
UCLASS()
class TINYSURVIVOR_API UItemDataSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

#pragma region ConstructorAndLifecycle
public:
	UItemDataSubsystem();

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
	TObjectPtr<UItemTableAsset> TableAsset;
#pragma endregion

#pragma region CachedData
protected:
	//========================================
	// Cached Data
	//========================================
	
	// 캐싱된 아이템 데이터 (Key: ItemID)
	UPROPERTY()
	TMap<int32, FItemData> ItemDataCache;
	
	// 캐싱된 건축물 데이터 (Key: BuildingID)
	UPROPERTY()
	TMap<int32, FBuildingData> BuildingDataCache;
	
	// 캐싱된 자원 원천 데이터 (Key: ResourceID)
	UPROPERTY()
	TMap<int32, FResourceData> ResourceDataCache;

	// 초기화 완료 여부 플래그
	bool bIsInitialized;
#pragma endregion

#pragma region StaticAccessHelper
public:
	//========================================
	// Static Access Helper
	//========================================
	
	/*
		편의성 함수: GameInstance에서 ItemDataSubsystem 가져오기
		Blueprint와 C++ 모두에서 안전하게 접근 가능
		
		@param WorldContext 월드 컨텍스트 (Character, GameMode, Widget 등)
		@return ItemDataSubsystem 인스턴스 (nullptr 가능)
		
		사용 예시 (C++):
			if (UItemDataSubsystem* ItemSystem = UItemDataSubsystem::GetItemDataSubsystem(this))
			{
				FItemData ItemData;
				if (ItemSystem->GetItemDataSafe(ItemID, ItemData))
				{
					// 아이템 사용
				}
			}
		
		사용 예시 (Blueprint):
			Get Item Data Subsystem → Branch (Is Valid) → Get Item Data Safe
	*/
	UFUNCTION(BlueprintPure, Category="ItemData", 
		meta=(WorldContext="WorldContext", DisplayName="Get Item Data Subsystem"))
	static UItemDataSubsystem* GetItemDataSubsystem(const UObject* WorldContext);
#pragma endregion
	
#pragma region Initialization
public:
	//========================================
	// Initialization
	//========================================
	/*
		데이터 에셋으로부터 시스템 초기화
		GameInstance::Init()에서 호출되어야 함
		
		@param InTableAsset 아이템/건축물/자원 데이터 테이블을 포함한 데이터 에셋
		@return 초기화 성공 여부
	*/
	UFUNCTION(BlueprintCallable, Category="ItemData")
	bool InitializeFromAsset(UItemTableAsset* InTableAsset);

	/*
		초기화 완료 여부 확인
		@return 초기화되었으면 true
	*/
	UFUNCTION(BlueprintPure, Category="ItemData")
	bool IsInitialized() const { return bIsInitialized; }
#pragma endregion

#pragma region ItemDataGetters
public:
	//========================================
	// Item Data Getters
	//========================================

	/*
		아이템 데이터 조회 (ID가 확실히 존재할 때만 사용)
		주의: 존재하지 않는 ID 조회 시 빈 데이터 반환
		안전한 조회가 필요하면 GetItemDataSafe() 사용 권장
		
		@param ItemID 조회할 아이템 ID
		@return 아이템 데이터 (존재하지 않으면 빈 데이터)
	*/
	UFUNCTION(BlueprintPure, Category="ItemData")
	const FItemData& GetItemData(int32 ItemID) const;

	/*
		아이템 데이터 조회 (안전 버전 - 권장)
		@param ItemID 조회할 아이템 ID
		@param OutData 출력될 아이템 데이터
		@return 데이터가 존재하면 true, 없으면 false
	*/
	UFUNCTION(BlueprintPure, Category="ItemData")
	bool GetItemDataSafe(int32 ItemID, FItemData& OutData) const;

	/*
		모든 아이템 ID 목록 조회
		@return 캐싱된 모든 아이템 ID 배열
	*/
	UFUNCTION(BlueprintPure, Category="ItemData")
	TArray<int32> GetAllItemIDs() const;

	/*
		카테고리별 아이템 ID 필터링
		@param Category 필터링할 카테고리
		@return 해당 카테고리의 아이템 ID 배열
	*/
	UFUNCTION(BlueprintPure, Category="ItemData")
	TArray<int32> GetItemIDsByCategory(EItemCategory Category) const;
#pragma endregion

#pragma region BuildingDataGetters
public:
	//========================================
	// Building Data Getters
	//========================================
	
	/*
		건축물 데이터 조회 (ID가 확실히 존재할 때만 사용)
		@param BuildingID 조회할 건축물 ID
		@return 건축물 데이터 (존재하지 않으면 빈 데이터)
	*/
	UFUNCTION(BlueprintPure, Category="ItemData")
	const FBuildingData& GetBuildingData(int32 BuildingID) const;
	
	/*
		건축물 데이터 조회 (안전 버전 - 권장)
		@param BuildingID 조회할 건축물 ID
		@param OutData 출력될 건축물 데이터
		@return 데이터가 존재하면 true, 없으면 false
	*/
	UFUNCTION(BlueprintPure, Category="ItemData")
	bool GetBuildingDataSafe(int32 BuildingID, FBuildingData& OutData) const;
	
	/*
		모든 건축물 ID 목록 조회
		@return 캐싱된 모든 건축물 ID 배열
	*/
	UFUNCTION(BlueprintPure, Category="ItemData")
	TArray<int32> GetAllBuildingIDs() const;
	
	/*
		건축물 타입별 필터링
		@param BuildingType 필터링할 건축물 타입
		@return 해당 타입의 건축물 ID 배열
	*/
	UFUNCTION(BlueprintPure, Category="ItemData")
	TArray<int32> GetBuildingIDsByType(EBuildingType BuildingType) const;
#pragma endregion

#pragma region ResourceDataGetters
public:
	//========================================
	// Resource Data Getters
	//========================================
	
	/*
		자원 원천 데이터 조회 (ID가 확실히 존재할 때만 사용)
		@param ResourceID 조회할 자원 원천 ID
		@return 자원 원천 데이터 (존재하지 않으면 빈 데이터)
	*/
	UFUNCTION(BlueprintPure, Category="ItemData")
	const FResourceData& GetResourceData(int32 ResourceID) const;
	
	/*
		자원 원천 데이터 조회 (안전 버전 - 권장)
		@param ResourceID 조회할 자원 원천 ID
		@param OutData 출력될 자원 원천 데이터
		@return 데이터가 존재하면 true, 없으면 false
	*/
	UFUNCTION(BlueprintPure, Category="ItemData")
	bool GetResourceDataSafe(int32 ResourceID, FResourceData& OutData) const;
	
	/*
		모든 자원 원천 ID 목록 조회
		@return 캐싱된 모든 자원 원천 ID 배열
	*/
	UFUNCTION(BlueprintPure, Category="ItemData")
	TArray<int32> GetAllResourceIDs() const;
	
	/*
		자원 원천 타입별 필터링
		@param NodeType 필터링할 자원 원천 타입
		@return 해당 타입의 자원 원천 ID 배열
	*/
	UFUNCTION(BlueprintPure, Category="ItemData")
	TArray<int32> GetResourceIDsByNodeType(ENodeType NodeType) const;
#pragma endregion

#pragma region Validation
public:
	//========================================
	// Validation
	//========================================
	
	/*
		아이템 데이터 유효성 확인
		@param ItemID 확인할 아이템 ID
		@return 유효한 데이터가 존재하면 true
	*/
	UFUNCTION(BlueprintPure, Category="ItemData")
	bool IsItemDataValid(int32 ItemID) const;
	
	/*
		건축물 데이터 유효성 확인
		@param BuildingID 확인할 건축물 ID
		@return 유효한 데이터가 존재하면 true
	*/
	UFUNCTION(BlueprintPure, Category="ItemData")
	bool IsBuildingDataValid(int32 BuildingID) const;
	
	/*
		자원 원천 데이터 유효성 확인
		@param ResourceID 확인할 자원 원천 ID
		@return 유효한 데이터가 존재하면 true
	*/
	UFUNCTION(BlueprintPure, Category="ItemData")
	bool IsResourceDataValid(int32 ResourceID) const;
#pragma endregion
	
#pragma region Debug
public:
	//========================================
	// Statistics & Debug
	//========================================
	
	/*
		캐싱된 데이터 통계 정보 조회
		@param OutItemCount 아이템 개수
		@param OutBuildingCount 건축물 개수
		@param OutResourceCount 자원 원천 개수
	*/
	UFUNCTION(BlueprintPure, Category="ItemData")
	void GetCacheStatistics(int32& OutItemCount, int32& OutBuildingCount, int32& OutResourceCount) const;
	
	/*
		디버그용 캐시 정보 로그 출력
	*/
	UFUNCTION(BlueprintCallable, Category="ItemData|Debug")
	void PrintCacheDebugInfo() const;
	
	/*
		디버그용 아이템 정보 로그 출력
	*/
	UFUNCTION(BlueprintCallable, Category="ItemData|Debug")
	void PrintItemDebugInfo(int32 ItemID) const;
	
	/*
		디버그용 건축물 정보 로그 출력
	*/
	UFUNCTION(BlueprintCallable, Category="ItemData|Debug")
	void PrintBuildingDebugInfo(int32 BuildingID) const;
	
	/*
		디버그용 자원 원천 정보 로그 출력
	*/
	UFUNCTION(BlueprintCallable, Category="ItemData|Debug")
	void PrintResourceDebugInfo(int32 ResourceID) const;
	
#if WITH_EDITOR
	/*
		에디터 전용: 캐시 리프레시
		데이터 테이블 변경 후 에디터에서 실시간 반영용
	*/
	UFUNCTION(CallInEditor, Category="ItemData|Editor")
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
		아이템 데이터 테이블을 TMap으로 캐싱
	*/
	void CacheItemData();

	/*
		건축물 데이터 테이블을 TMap으로 캐싱
	*/
	void CacheBuildingData();

	/*
		자원 원천 데이터 테이블을 TMap으로 캐싱
	*/
	void CacheResourceData();

	/*
		모든 캐시 초기화
	*/
	void ClearAllCaches();

	/*
		데이터 에셋 유효성 검증
		@param InTableAsset 검증할 데이터 에셋
		@return 유효하면 true
	*/
	bool ValidateTableAsset(const UItemTableAsset* InTableAsset) const;
#pragma endregion
};