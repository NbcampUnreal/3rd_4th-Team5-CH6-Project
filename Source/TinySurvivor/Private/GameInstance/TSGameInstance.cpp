// TSGameInstance.cpp
#include "GameInstance/TSGameInstance.h"

#include "Item/System/ItemDataSubsystem.h"
#include "Item/Data/ItemTableAsset.h"

DEFINE_LOG_CATEGORY_STATIC(LogTSGameInstance, Log, All);

// 게임 인스턴스 생성자
UTSGameInstance::UTSGameInstance()
{
}

// 게임 인스턴스 초기화
void UTSGameInstance::Init()
{
	Super::Init();
	
	UE_LOG(LogTSGameInstance, Log, TEXT("TSGameInstance::Init() - 시작"));
	
	// 아이템 데이터 시스템 초기화
	if (!InitializeItemDataSystem())
	{
		UE_LOG(LogTSGameInstance, Error, TEXT("아이템 데이터 시스템 초기화 실패!"));
	}
	
	UE_LOG(LogTSGameInstance, Log, TEXT("TSGameInstance::Init() - 완료"));
}

// 게임 인스턴스 종료
void UTSGameInstance::Shutdown()
{
	UE_LOG(LogTSGameInstance, Log, TEXT("TSGameInstance::Shutdown()"));
	
	Super::Shutdown();
}

// 아이템 데이터 시스템 초기화
bool UTSGameInstance::InitializeItemDataSystem()
{
	// ItemDataSubsystem 가져오기
	UItemDataSubsystem* ItemSystem = GetSubsystem<UItemDataSubsystem>();
	
	if (!ItemSystem)
	{
		UE_LOG(LogTSGameInstance, Error, TEXT("ItemDataSubsystem을 찾을 수 없습니다!"));
		return false;
	}
	
	// ItemTableAsset 유효성 검사
	if (!ItemTableAsset)
	{
		UE_LOG(LogTSGameInstance, Error, TEXT("GameInstance에 ItemTableAsset이 설정되지 않았습니다!"));
		UE_LOG(LogTSGameInstance, Error, TEXT("GameInstance BP에서 ItemTableAsset을 설정해주세요."));
		return false;
	}
	
	// 초기화 실행
	const bool bSuccess = ItemSystem->InitializeFromAsset(ItemTableAsset);
	
	if (bSuccess)
	{
		UE_LOG(LogTSGameInstance, Log, TEXT("ItemDataSystem이 성공적으로 초기화되었습니다"));
		
#if WITH_EDITOR
		// 에디터에서만 테스트 실행
		TestItemDataSystem(ItemSystem);
#endif
	}
	else
	{
		UE_LOG(LogTSGameInstance, Error, TEXT("ItemDataSystem 초기화 실패"));
	}
	
	return bSuccess;
}

#if WITH_EDITOR
// 아이템 데이터 시스템 테스트 함수 (에디터 전용)
void UTSGameInstance::TestItemDataSystem(UItemDataSubsystem* ItemSystem)
{
	if (!ItemSystem)
	{
		return;
	}
	
	UE_LOG(LogTSGameInstance, Display, TEXT("========================================"));
	UE_LOG(LogTSGameInstance, Display, TEXT("ItemDataSubsystem 테스트 시작"));
	UE_LOG(LogTSGameInstance, Display, TEXT("========================================"));
	
	// 1. 통계 출력
	ItemSystem->PrintCacheDebugInfo();
	
	// 2. 아이템 조회 테스트
	UE_LOG(LogTSGameInstance, Display, TEXT("\n--- 아이템 조회 테스트 ---"));
	FItemData ItemData;
	if (ItemSystem->GetItemDataSafe(1, ItemData))
	{
		UE_LOG(LogTSGameInstance, Display, TEXT("✅ 아이템 ID 1 찾음:"));
		UE_LOG(LogTSGameInstance, Display, TEXT("  - 이름(KR): %s"), *ItemData.Name_KR.ToString());
		UE_LOG(LogTSGameInstance, Display, TEXT("  - 이름(EN): %s"), *ItemData.Name_EN.ToString());
		UE_LOG(LogTSGameInstance, Display, TEXT("  - MaxStack: %d"), ItemData.MaxStack);
	}
	else
	{
		UE_LOG(LogTSGameInstance, Warning, TEXT("❌ 아이템 ID 1을 찾을 수 없습니다"));
	}
	
	// 3. 존재하지 않는 ID 테스트
	UE_LOG(LogTSGameInstance, Display, TEXT("\n--- 존재하지 않는 ID 테스트 ---"));
	if (ItemSystem->GetItemDataSafe(9999, ItemData))
	{
		UE_LOG(LogTSGameInstance, Display, TEXT("✅ ID 9999 찾음 (이상함!)"));
	}
	else
	{
		UE_LOG(LogTSGameInstance, Display, TEXT("❌ ID 9999를 찾을 수 없습니다 (정상)"));
	}
	
	// 4. 건축물 조회 테스트
	UE_LOG(LogTSGameInstance, Display, TEXT("\n--- 건축물 조회 테스트 ---"));
	FBuildingData BuildingData;
	if (ItemSystem->GetBuildingDataSafe(1, BuildingData))
	{
		UE_LOG(LogTSGameInstance, Display, TEXT("✅ 건축물 ID 1 찾음:"));
		UE_LOG(LogTSGameInstance, Display, TEXT("  - 이름(KR): %s"), *BuildingData.Name_KR.ToString());
		UE_LOG(LogTSGameInstance, Display, TEXT("  - 최대 내구도: %d"), BuildingData.MaxDurability);
	}
	else
	{
		UE_LOG(LogTSGameInstance, Warning, TEXT("❌ 건축물 ID 1을 찾을 수 없습니다"));
	}
	
	// 5. 자원 조회 테스트
	UE_LOG(LogTSGameInstance, Display, TEXT("\n--- 자원 원천 조회 테스트 ---"));
	FResourceData ResourceData;
	if (ItemSystem->GetResourceDataSafe(1, ResourceData))
	{
		UE_LOG(LogTSGameInstance, Display, TEXT("✅ 자원 ID 1 찾음:"));
		UE_LOG(LogTSGameInstance, Display, TEXT("  - 이름(KR): %s"), *ResourceData.Name_KR.ToString());
		UE_LOG(LogTSGameInstance, Display, TEXT("  - 총 수량: %d"), ResourceData.TotalYield);
	}
	else
	{
		UE_LOG(LogTSGameInstance, Warning, TEXT("❌ 자원 ID 1을 찾을 수 없습니다"));
	}
	
	// 6. 카테고리별 필터링 테스트
	UE_LOG(LogTSGameInstance, Display, TEXT("\n--- 카테고리별 아이템 필터링 ---"));
	TArray<int32> MaterialIDs = ItemSystem->GetItemIDsByCategory(EItemCategory::MATERIAL);
	UE_LOG(LogTSGameInstance, Display, TEXT("재료(MATERIAL) 개수: %d"), MaterialIDs.Num());
	
	TArray<int32> ToolIDs = ItemSystem->GetItemIDsByCategory(EItemCategory::TOOL);
	UE_LOG(LogTSGameInstance, Display, TEXT("도구(TOOL) 개수: %d"), ToolIDs.Num());
	
	UE_LOG(LogTSGameInstance, Display, TEXT("========================================"));
	UE_LOG(LogTSGameInstance, Display, TEXT("테스트 완료"));
	UE_LOG(LogTSGameInstance, Display, TEXT("========================================"));
}
#endif