// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "RYCoreLogMessageData.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "RYCoreLogMessageSystem.generated.h"

inline int32 CoreLogCount = 0;

/**
 * 
 */
UCLASS()
class RYGAMESCORESYSTEM_API URYCoreLogMessageSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
		
//======================================================================================================================
#pragma region Getter
	
	//--------------------
	// 게터
	//--------------------
	
public:
	static URYCoreLogMessageSystem* Get(const UObject* WorldContextObject);
	
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
	UFUNCTION(BlueprintCallable, Category = "RY | Core | System | LogMessage")
	void AddCoreLogMessage(const FString& Category, const FString& Step, const FString& NewLogMessage);
	
protected:
	
	// 세션 헤더 API (게임 켜질 때마다 구분해서 보기 위함)
	UFUNCTION(BlueprintCallable, Category = "RY | Core | System | LogMessage")
	void SetCoreSessionHeader();
	
	// 로그 저장 API (덮어씌우기)
	UFUNCTION(BlueprintCallable, Category = "RY | Core | System | LogMessage")
	void SaveCoreLogMessageToJson();
	
	// 로그 저장 API (이어 쓰기)
	UFUNCTION(BlueprintCallable, Category = "RY | Core | System | LogMessage")
	void AppendCoreLogMessageToJson();
	
	
	// 로그 메시지 배열 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RY | Core | System | LogMessage")
	FCoreLogContainer CoreLogContainer;
	
	
#pragma endregion
//======================================================================================================================
};
