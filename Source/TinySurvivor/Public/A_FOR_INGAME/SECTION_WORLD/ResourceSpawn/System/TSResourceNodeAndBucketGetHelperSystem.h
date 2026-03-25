// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Subsystems/WorldSubsystem.h"
#include "A_FOR_INGAME/SECTION_INGAMECYCLE/Data/Enum/TSInGameCycleMode.h"
#include "A_FOR_INGAME/SECTION_SAVELOAD/Data/Struct/TSSaveMasterData.h"
#include "A_FOR_INGAME/SECTION_WORLD/ResourceSpawn/Data/Struct/System/FTSResourceBucketArray.h"
#include "A_FOR_INGAME/SECTION_WORLD/ResourceSpawn/Data/Struct/System/FTSResourceNodeArray.h"
#include "TSResourceNodeAndBucketGetHelperSystem.generated.h"

class ATSResourceBucketActor;
class ATSResourceNodeActor;
/**
 * 버킷, 노드 찾아서 전달해주는 도움 시스템 : 자원 스폰 컨트롤 시스템의 지시를 받아 현재 존재하는 버킷, 노드를 반환해줌.
 * 세이브 로드와 상관없이 자원 노드와 버킷은 post에서 이 시스템에게 자신을 등록해야함.
 */
UCLASS()
class TINYSURVIVOR_API UTSResourceNodeAndBucketGetHelperSystem : public UWorldSubsystem
{
	friend class UTSResourceSpawnControlSystem;
	
	GENERATED_BODY()
	
//======================================================================================================================	
#pragma region 게터
	
	//━━━━━━━━━━━━━━━━━━━━
	// 게터
	//━━━━━━━━━━━━━━━━━━━━	
public:
	static UTSResourceNodeAndBucketGetHelperSystem* Get(const UObject* InWorldContextObject);
	
#pragma endregion
//======================================================================================================================	
#pragma region 라이프_사이클
	
	//━━━━━━━━━━━━━━━━━━━━
	// 라이프 사이클
	//━━━━━━━━━━━━━━━━━━━━
public:
	UTSResourceNodeAndBucketGetHelperSystem();
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	virtual void Deinitialize() override;
	
#pragma endregion
//======================================================================================================================	
#pragma region 인게임_사이클
	
	//━━━━━━━━━━━━━━━━━━━━
	// 인게임_사이클 (자원 초기화 이후 로직)
	//━━━━━━━━━━━━━━━━━━━━	
	
protected:
	// 인 게임 사이클 델리게이트 바인딩 함수 
	UFUNCTION()
	void OnReceivedInGameCycleDelegate_internal(ETSInGameCycleMode InGameCycleMode, FTSSaveMasterData& InData);
	
	// new 호출 시
	void CallWhenNewModeIsCalled_internal();
	
	// load 호출 시
	void CallWhenLoadModeIsCalled_internal(FTSSaveMasterData& InData);
	
	// play 호출 시 
	void CallWhenPlayModeIsCalled_internal();
	
#pragma endregion
//======================================================================================================================	
#pragma region 노드_버킷_섹션
	
	//━━━━━━━━━━━━━━━━━━━━
	// 노드_버킷_섹션
	//━━━━━━━━━━━━━━━━━━━━
	
public:
	void RequestRegisterNode(ATSResourceNodeActor* InNode);
	void RequestRegisterBucket(ATSResourceBucketActor* InBucket);
	
protected:
	
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite, Category = "TS | Resource")
	TMap<FGameplayTag, FTSResourceNodeArray> NodeList;
	
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite, Category = "TS | Resource")
	TMap<FGameplayTag, FTSResourceBucketArray> BucketList;
	
	
#pragma endregion
//======================================================================================================================	
#pragma region 자원_초기화
	
	//━━━━━━━━━━━━━━━━━━━━
	// 자원_초기화
	//━━━━━━━━━━━━━━━━━━━━
	
protected:
	
	// 버킷, 노드 초기화 실시 함수 
	void InitResourceNodeAndBucketList();
	
	// 버킷, 노드 초기화 완료 함수 
	void InitResourceNodeAndBucketListComplete();
	
#pragma endregion
//======================================================================================================================		

	
};
