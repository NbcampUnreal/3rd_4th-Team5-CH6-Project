// All CopyRight From YulRyongGameStudio //


#include "SECTION_UTIL/RYCoreLogMessageSystem.h"
#include "SECTION_UTIL/Marco/RYCoreLogMarco.h"
#include "Kismet/GameplayStatics.h"
#include "RYLog.h"
#include "SECTION_CYCLE/RYGameCycleSubSystem.h"

//======================================================================================================================
#pragma region Getter
	
	//--------------------
	// 게터
	//--------------------
	
URYCoreLogMessageSystem* URYCoreLogMessageSystem::Get(const UObject* WorldContextObject)
{
	const UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(WorldContextObject);
	if (IsValid(GameInstance) == false)
	{
		// Fail 로그
		CORELOGMESSAGESYSTEM_WARN(TEXT("GameInstance Fail"));
		return nullptr;
	}
	
	URYCoreLogMessageSystem* LogSystem = GameInstance->GetSubsystem<URYCoreLogMessageSystem>();
	if (IsValid(LogSystem) == false)
	{
		// Fail 로그
		CORELOGMESSAGESYSTEM_WARN(TEXT("GetSubsystem Fail"));
		return nullptr;
	}
	
	// Success 로그
	CORELOGMESSAGESYSTEM_LOG(TEXT("Get Success"));
	
	return LogSystem;
}

#pragma endregion
//======================================================================================================================
#pragma region Life_Cycle
	
	//--------------------
	// 라이프 사이클 
	//--------------------

bool URYCoreLogMessageSystem::ShouldCreateSubsystem(UObject* Outer) const
{
	return true;
}

void URYCoreLogMessageSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	// 게임 사이클 시스템 초기화 이후 생성
	Collection.InitializeDependency(URYGameCycleSubSystem::StaticClass());
	
	// 로그 데이터 저장 배열 100개 : 100개 이상차면 자동으로 Json 으로 빼기.
	int32 MaxLogCount = 100;
	CoreLogContainer.Logs.Reserve(100);
	
	// 헤더 설정 json 파일
	SetCoreSessionHeader();
}

void URYCoreLogMessageSystem::Deinitialize()
{
	// 게임 종료시 남은 배열도 전부 저장 
	AppendCoreLogMessageToJson();
	
	Super::Deinitialize();
}


#pragma endregion
//======================================================================================================================
#pragma region 로그_메시지
	
	//--------------------
	// 로그 메시지
	//--------------------

void URYCoreLogMessageSystem::AddCoreLogMessage(const FString& Category, const FString& Step, const FString& NewLogMessage)
{
	// 로그 카운트 증가
	++CoreLogCount;
	
	// 메세지 데이터 생성
	FCoreLogEntry NewMessage;
	NewMessage.RealTime = FDateTime::Now().ToString();
	NewMessage.LogCount = CoreLogCount;
	NewMessage.Category = Category;
	NewMessage.WarningStep = Step;
	NewMessage.Message = NewLogMessage;
	
	// 메시지 배열에 추가 
	CoreLogContainer.Logs.Add(NewMessage);
	
	// 100개 넘을 경우 json 에 추가 후 배열 초기화
	if (CoreLogContainer.Logs.Num() >= 100)
	{
		AppendCoreLogMessageToJson();
		CoreLogContainer.Logs.Reset();
	}
}

void URYCoreLogMessageSystem::SetCoreSessionHeader()
{
	// 포맷대로 문자열 구성 (아래 예시처럼)
	// >>> 2026.03.10-17.20.45 <<<
	// -------------------------------------------------------------------------------- 
	
	//	현재 날짜와 시간 가져오기
	FString CurrentTime = FDateTime::Now().ToString();
	FString SessionHeader = FString::Printf(TEXT("\n>>> %s <<<\n"), *CurrentTime);
	SessionHeader += TEXT("--------------------------------------------------------------------------------\n");

	// 파일 경로 설정
	FString LogFilePath = FPaths::ProjectSavedDir() / TEXT("RYCoreLog.log");

	// 4. 파일 끝에 추가 (Append 모드)
	FFileHelper::SaveStringToFile
	(
		SessionHeader, 
		*LogFilePath, 
		FFileHelper::EEncodingOptions::AutoDetect, 
		&IFileManager::Get(), 
		FILEWRITE_Append
	);
}

void URYCoreLogMessageSystem::SaveCoreLogMessageToJson()
{
	FString FormattedLogs = "";
	for (const FCoreLogEntry& Entry : CoreLogContainer.Logs)
	{
		// [인게임시간][카운트]카테고리: 메시지 형태 구성
		FString Line = FString::Printf(TEXT("[%s][%d] %s - %s - : %s\n"), 
			*Entry.RealTime, 
			Entry.LogCount, 
			*Entry.Category, 
			*Entry.WarningStep,
			*Entry.Message);
        
		FormattedLogs += Line;
	}

	// 파일 경로 설정 (Saved 폴더 안에 저장됨)
	FString LogFilePath = FPaths::ProjectSavedDir() / TEXT("RYCoreLog.log");
	
	// 파일로 저장
	FFileHelper::SaveStringToFile(FormattedLogs, *LogFilePath);
}

void URYCoreLogMessageSystem::AppendCoreLogMessageToJson()
{
	FString FormattedLogs = "";
	for (const FCoreLogEntry& Entry : CoreLogContainer.Logs)
	{
		// [인게임시간][카운트]카테고리: 메시지 형태 구성
		FString Line = FString::Printf(TEXT("[%s][%d] %s - %s - : %s\n"), 
			*Entry.RealTime, 
			Entry.LogCount, 
			*Entry.Category, 
			*Entry.WarningStep,
			*Entry.Message);
        
		FormattedLogs += Line;
	}
	
	// 파일 경로 설정 (Saved 폴더 안에 저장됨)
	FString LogFilePath = FPaths::ProjectSavedDir() / TEXT("RYCoreLog.log");
	
	// 파일 끝에 추가 (Append 모드)
	FFileHelper::SaveStringToFile
	(
		FormattedLogs, 
		*LogFilePath, 
		FFileHelper::EEncodingOptions::AutoDetect, 
		&IFileManager::Get(), 
		FILEWRITE_Append
	);
}

#pragma endregion
//======================================================================================================================

