// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "SECTION_UTIL/Data/RYLogMessageData.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "RYLogMessageSystem.generated.h"

inline int64 LogCount = 0;

/**
 * 
 */
UCLASS()
class RYGAMEPLAYSYSTEM_API URYLogMessageSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
//======================================================================================================================
#pragma region Getter
	
	//--------------------
	// 게터
	//--------------------
	
public:
	static URYLogMessageSystem* Get(const UObject* Object);
	
#pragma endregion
//======================================================================================================================
#pragma region Life_Cycle
	
	//--------------------
	// 라이프 사이클 
	//--------------------
	
public:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	
#pragma endregion
//======================================================================================================================
#pragma region 로그_메시지
	
	//--------------------
	// 로그 메시지
	//--------------------

public:

	// 로그 메시지 추가
	UFUNCTION(BlueprintCallable, Category = "RY | System | LogMessage")
	void AddLogMessage(const FString& Category, const FString& Step, const FString& NewLogMessage);
	
protected:
	
	// 세션 헤더 API (게임 켜질 때마다 구분해서 보기 위함)
	UFUNCTION(BlueprintCallable, Category = "RY | System | LogMessage")
	void SetSessionHeader();
	
	// 로그 저장 API (덮어씌우기)
	UFUNCTION(BlueprintCallable, Category = "RY | System | LogMessage")
	void SaveLogMessageToJson();
	
	// 로그 저장 API (이어 쓰기)
	UFUNCTION(BlueprintCallable, Category = "RY | System | LogMessage")
	void AppendLogMessageToJson();
	
	
	// 로그 메시지 배열 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RY | System | LogMessage")
	FGameLogContainer LogContainer;
	
	
#pragma endregion
//======================================================================================================================
	
};
