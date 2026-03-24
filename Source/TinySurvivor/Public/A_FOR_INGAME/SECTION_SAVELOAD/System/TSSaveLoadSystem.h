// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "A_FOR_INGAME/SECTION_SAVELOAD/Data/Struct/TSSaveMasterData.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "TSSaveLoadSystem.generated.h"

/**
 * 
 */
UCLASS()
class TINYSURVIVOR_API UTSSaveLoadSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
//======================================================================================================================	
#pragma region 게터
	
	//━━━━━━━━━━━━━━━━━━━━
	// 게터
	//━━━━━━━━━━━━━━━━━━━━	
public:

	static UTSSaveLoadSystem* Get(const UObject* InWorldContextObject);

#pragma endregion
//======================================================================================================================	
#pragma region 라이프_사이클
	
	//━━━━━━━━━━━━━━━━━━━━
	// 라이프 사이클
	//━━━━━━━━━━━━━━━━━━━━	
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	
#pragma endregion
//======================================================================================================================	
#pragma region 세이브로드_API
	
	//━━━━━━━━━━━━━━━━━━━━
	// 세이브로드 API
	//━━━━━━━━━━━━━━━━━━━━	
		
public:
	// index 값에 따른 save 데이터 로드 요청 (새로 시작, 이어하기, 불러오기할 때만 써야하는 API)
	bool RequestLoadSaveData(int32 InSaveID);
	
	// 현재 로드한 데이터 가져오기 
	FORCEINLINE FTSSaveMasterData* RequestCurrentLoadedDataPtr() { return &CurrentLoadedData;};
	
	// 저장할 데이터 공간 요청 
	FORCEINLINE FTSSaveMasterData* RequestDataStructForSaving() { return &UesThisForSaveData;};
	
	// 저장 요청 
	bool RequestSaveData(int32 InSaveID);
	
	// 마지막으로 로드 요청한 슬롯 ID
	FORCEINLINE int32 GetLastRequestedLoadSaveID() { return LastRequestedLoadSaveID;};
	
#pragma endregion
//======================================================================================================================	
#pragma region 데이터
	
	//━━━━━━━━━━━━━━━━━━━━
	// 데이터
	//━━━━━━━━━━━━━━━━━━━━		
	
protected:
	
	// 마지막으로 로드 요청했던 세이브 게임 데이터의 인덱스 autoSave 제외
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TS | Save")
	int32 LastRequestedLoadSaveID = -1;
	
	// 현재 로드된 데이터
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TS | Save")
	FTSSaveMasterData CurrentLoadedData;
	
	// 저장할 때 쓰는 데이터 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TS | Save")
	FTSSaveMasterData UesThisForSaveData;

#pragma endregion
//======================================================================================================================	
	
};
