// MonsterSpawnSubsystem.h

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "AI/Common/TSAICharacter.h"
#include "AI/Common/Spawn/TSAISpawner.h"
#include "System/Erosion/TSErosionSubSystem.h"
#include "MonsterSpawnSubsystem.generated.h"

class UMonsterSpawnConfig;

UCLASS()
class TINYSURVIVOR_API UMonsterSpawnSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	virtual void Deinitialize() override;
	
	UFUNCTION(BlueprintCallable, Category = "Spawn")
	void InitializeSpawnConfig(UMonsterSpawnConfig* NewConfig);
	
	// 스포너 등록
	void RegisterSpawner(ATSAISpawner* Spawner);
	// 몬스터가 죽으면 호출 (풀 반납)
	void ReturnMonsterInPool(ATSAICharacter* Monster);
	
protected:
	// 5초마다 호출
	void CheckSpawnRules();
	
	UFUNCTION()
	void OnErosionChanged(float FinalValue);
	
	// 풀링 로직
	ATSAICharacter* GetMonsterFromPool(TSubclassOf<ATSAICharacter> TargetClass);
	
protected:
	
	UPROPERTY()
	TObjectPtr<UMonsterSpawnConfig> MonsterConfig;
	
	// 맵에 있는 스포너들 목록
	UPROPERTY()
	TArray<ATSAISpawner*> ActiveSpawners;
	
	// 대기 중인 몬스터들 (풀)
	UPROPERTY()
	TArray<ATSAICharacter*> InactiveMonsterPool;
	
	// 활동 중인 몬스터들
	UPROPERTY()
	TArray<ATSAICharacter*> ActiveMonsters;
	
	FTimerHandle SpawnTimerHandle;
	
	// 현재 최대 허용 몬스터 수 (침식도에 따라 변함)
	int32 CurrentMaxMonsterCount = 5;
};
