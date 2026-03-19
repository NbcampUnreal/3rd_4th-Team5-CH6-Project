// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "Struct_RYSoundOption.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "RYOptionSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class RYGAMESCORESYSTEM_API URYOptionSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
//======================================================================================================================
#pragma region GetSystem
	//--------------------
	// Get System
	//--------------------	
	
public:
	static URYOptionSubsystem* Get(const UObject* Object);

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
#pragma region GameCycle(None)
	//--------------------
	// Game Cycle
	//--------------------	

#pragma endregion
//======================================================================================================================
#pragma region Option
	//--------------------
	// Option
	//--------------------	
	
public:
	// 오디오 세팅 초기화 API
	UFUNCTION(BlueprintCallable, Category = "RY | Core | Option")
	void InitializeAudioSettings();
	
	// 오디오 세팅 API
	UFUNCTION(BlueprintCallable, Category = "RY | Core | Option")
	void RequestSetNewAudioSettings(const TArray<FRYSoundOption>& NewSoundOptions);

#pragma endregion
//======================================================================================================================
};
