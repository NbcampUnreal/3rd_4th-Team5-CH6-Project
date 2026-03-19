// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "RYLogMessageData.generated.h"

USTRUCT(BlueprintType)
struct FGameLogEntry
{
	GENERATED_BODY()

	// 인게임 시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString RealTime; 

	// 실행 로그 카운트
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 LogCount = 0;

	// 실행 로그 카테고리
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Category = {}; 

	// 실행 로그 단계
	FString WarningStep = {};
	
	// 실행 로그 내역
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Message = {};
	
};

USTRUCT(BlueprintType)
struct FGameLogContainer
{
	GENERATED_BODY()

	// 여러 로그를 담을 배열
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FGameLogEntry> Logs = {}; 
};