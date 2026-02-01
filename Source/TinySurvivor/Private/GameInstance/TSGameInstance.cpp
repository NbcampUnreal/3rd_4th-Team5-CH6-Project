// TSGameInstance.cpp
#include "GameInstance/TSGameInstance.h"
 
// 로그 카테고리 정의
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
	
	UE_LOG(LogTSGameInstance, Log, TEXT("TSGameInstance::Init() - 완료"));
}

// 게임 인스턴스 종료
void UTSGameInstance::Shutdown()
{
	UE_LOG(LogTSGameInstance, Log, TEXT("TSGameInstance::Shutdown()"));
	
	Super::Shutdown();
}
