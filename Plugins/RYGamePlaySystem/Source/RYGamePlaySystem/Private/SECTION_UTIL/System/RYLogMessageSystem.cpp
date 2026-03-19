// All CopyRight From YulRyongGameStudio //


#include "SECTION_UTIL/System/RYLogMessageSystem.h"
#include "SECTION_UTIL/Macro/RYLogMacro.h"
#include "SECTION_UTIL/System/RYLogDataNetworkSystem.h"

//======================================================================================================================
#pragma region Getter
	
	//--------------------
	// 게터
	//--------------------
	
URYLogMessageSystem* URYLogMessageSystem::Get(const UObject* Object)
{
	// 시도하려는 오브젝트 확인
	if (IsValid(Object) == false)
	{
		LOGMESSAGESYSTEM_WARN(TEXT("Get : Object fail"));
		return nullptr;
	}
	
	// 월드 컨텍스트로부터 월드를 가져옵니다.
	const UWorld* World = Object->GetWorld();
	if (IsValid(World) == false)
	{
		LOGMESSAGESYSTEM_WARN_RECORD(Object, TEXT("Get : World fail"));
		return nullptr;
	}
	
	// 월드로부터 게임 인스턴스를 가져옵니다.
	const UGameInstance* GameInstance = World->GetGameInstance();
	if (IsValid(GameInstance) == false)
	{
		LOGMESSAGESYSTEM_WARN_RECORD(World, TEXT("Get : GameInstance fail"));
		return nullptr;
	}
	
	// 게임 인스턴스로부터 게임 인스턴스 서브 시스템을 가져옵니다.
	URYLogMessageSystem* LogSystem = GameInstance->GetSubsystem<URYLogMessageSystem>();
	if (IsValid(LogSystem) == false)
	{
		LOGMESSAGESYSTEM_WARN_RECORD(GameInstance, TEXT("Get : GetSubsystem Fail"));
		return nullptr;
	}
	
	LOGMESSAGESYSTEM_LOG_RECORD(LogSystem, TEXT("Get : Get Success"));
	return LogSystem;
}

#pragma endregion
//======================================================================================================================
#pragma region Life_Cycle
	
	//--------------------
	// 라이프 사이클 
	//--------------------

bool URYLogMessageSystem::ShouldCreateSubsystem(UObject* Outer) const
{
	return true;
}

void URYLogMessageSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	// 로그 데이터 저장 배열 100개 : 100개 이상차면 자동으로 Json 으로 빼기.
	int32 MaxLogCount = 100;
	LogContainer.Logs.Reserve(100);
	
	// 헤더 설정 json 파일
	SetSessionHeader();
	
	LOGMESSAGESYSTEM_LOG(TEXT("Initialize : Success"));
}

void URYLogMessageSystem::Deinitialize()
{
	// 게임 종료시 남은 배열도 전부 저장 
	AppendLogMessageToJson();
	
	LOGMESSAGESYSTEM_LOG(TEXT("Initialize : Success"));
	
	Super::Deinitialize();
}


#pragma endregion
//======================================================================================================================
#pragma region 로그_메시지
	
	//--------------------
	// 로그 메시지
	//--------------------

void URYLogMessageSystem::AddLogMessage(const FString& Category, const FString& Step, const FString& NewLogMessage)
{
	// 로그 카운트 증가
	++LogCount;
	
	// 메세지 데이터 생성
	FGameLogEntry NewMessage;
	NewMessage.RealTime = FDateTime::Now().ToString();
	NewMessage.LogCount = LogCount;
	NewMessage.Category = Category;
	NewMessage.WarningStep = Step;
	NewMessage.Message = NewLogMessage;
	
	// 메시지 배열에 추가 
	LogContainer.Logs.Add(NewMessage);
	
	// 100개 넘을 경우 json 에 추가 후 배열 초기화
	if (LogContainer.Logs.Num() >= 100)
	{
		AppendLogMessageToJson();
		LogContainer.Logs.Reset();
	}
}

void URYLogMessageSystem::SetSessionHeader()
{
	// 포맷대로 문자열 구성 (아래 예시처럼)
	// >>> 2026.03.10-17.20.45 <<<
	// -------------------------------------------------------------------------------- 
	
	//	현재 날짜와 시간 가져오기
	FString CurrentTime = FDateTime::Now().ToString();
	FString SessionHeader = FString::Printf(TEXT("\n>>> %s <<<\n"), *CurrentTime);
	SessionHeader += TEXT("--------------------------------------------------------------------------------\n");

	// 파일 경로 설정
	FString LogFilePath = FPaths::ProjectSavedDir() / TEXT("RYGameLog.log");

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

void URYLogMessageSystem::SaveLogMessageToJson()
{
	FString FormattedLogs = "";
	for (const FGameLogEntry& Entry : LogContainer.Logs)
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
	FString LogFilePath = FPaths::ProjectSavedDir() / TEXT("RYGameLog.log");
	
	// 파일로 저장
	FFileHelper::SaveStringToFile(FormattedLogs, *LogFilePath);
}

void URYLogMessageSystem::AppendLogMessageToJson()
{
	FString FormattedLogs = "";
	for (const FGameLogEntry& Entry : LogContainer.Logs)
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
	FString LogFilePath = FPaths::ProjectSavedDir() / TEXT("RYGameLog.log");
	
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

