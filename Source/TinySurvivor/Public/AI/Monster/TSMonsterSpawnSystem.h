#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Subsystems/WorldSubsystem.h"
#include "TSMonsterSpawnSystem.generated.h"

struct FTSMonsterTable;
DECLARE_LOG_CATEGORY_EXTERN(MonsterManager, Log, All);

class UTSMonsterDataAsset;
/**
 * 
 */
UCLASS()
class TINYSURVIVOR_API UTSMonsterSpawnSystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
	
	//---------------------------------------
	// UTSMonsterSpawnSystem 라이프 사이클
	//--------------------------------------
	
public:
	UTSMonsterSpawnSystem();
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void PostInitialize() override;
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	virtual void Deinitialize() override;
	
	// 게터
	UFUNCTION(BlueprintPure, meta = (WorldContext = "WorldContextObject"))
	static UTSMonsterSpawnSystem* Get(const UObject* WorldContextObject);
	
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
	
	//---------------------------------------
	// 스폰 요청 API
	//--------------------------------------
	
public:
	// 몬스터 스폰 요청 API (BP)
	UFUNCTION(BlueprintCallable)
	bool RequestSpawnMonsterWithBP(FTransform SpawnParms, FGameplayTag MonsterTag, AActor* Instigator);
	
	// 몬스터 스폰 요청 API (C++)
	bool RequestMonsterSpawn(FTransform& SpawnParms, FGameplayTag& MonsterTag, AActor* Instigator);
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
	
	//---------------------------------------
	// 데이터 에셋 관련
	//--------------------------------------

protected:
	// 세이브 로드 데이터 에셋 (캐싱)
	UPROPERTY() 
	TObjectPtr<UTSMonsterDataAsset> CachedWorldDropDataAsset;
	
	// 로그 제어 변수 캐싱 (캐싱)
	bool CachedbWantPrintLog;
	
	// 배열 캐싱
	TArray<FTSMonsterTable> CachingRows;
	
	// 조회용 TMAP
	TMap<FGameplayTag, FTSMonsterTable*> CachingMonsterMap;
	
};
