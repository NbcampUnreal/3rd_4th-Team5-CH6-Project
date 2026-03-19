// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SECTION_CYCLE/Enum_RYGameCycle.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "RYSaveLoadSubSystem.generated.h"

/**
 * 세이브 로드 시스템
 * : 메인 역할 :
 *  1. 
 *  2. 
 */
UCLASS()
class RYGAMESCORESYSTEM_API URYSaveLoadSubSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
//======================================================================================================================
#pragma region GetSystem
	//--------------------
	// Get System
	//--------------------

public:
	static URYSaveLoadSubSystem* Get(const UObject* Object);
	
#pragma endregion
//======================================================================================================================
#pragma region LifeCycle
	//--------------------
	// Life Cycle
	//--------------------

public:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	
#pragma endregion
//======================================================================================================================
#pragma region GameCycle
	//--------------------
	// Game Cycle
	//--------------------
	
public:
	// 게임 사이클 메인 바인딩 함수
	UFUNCTION(BlueprintCallable, Category = "RY | Core | SaveLoad")
	void CallOnGameCycleMode(ERYGameCycleMode NewGameCycleMode);
	
	// 수동 세이브일 때 
	UFUNCTION(BlueprintCallable, Category = "RY | Core | SaveLoad")
	void CallOnManualSaveMode();
	
	// 오토 세이브일 때
	UFUNCTION(BlueprintCallable, Category = "RY | Core | SaveLoad")
	void CallOnAutoSaveMode();
	
#pragma endregion
//======================================================================================================================
#pragma region SaveLoad
	//--------------------
	// Save Load
	//--------------------
	
protected:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RY | Core | SaveLoad")
	TSubclassOf<USaveGame> SaveGameClass = nullptr;

#pragma endregion
//======================================================================================================================
};


#pragma region DetailedDescription
// 상세 설명 
/*
 * < 개요 > 
 * 세이브 로드 시스템은 
 *
 * < 시스템의 역할 >
 *
 *
 * - Ex - 
 * 
 * 
 * 
 * - 구체적 설명 -
 * - 구체적 설명 -
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * ##[주의]##
 * 
 * 
 * 
 * 
 * 
 * Last Update 2026.02.24
 * 
 */
#pragma endregion