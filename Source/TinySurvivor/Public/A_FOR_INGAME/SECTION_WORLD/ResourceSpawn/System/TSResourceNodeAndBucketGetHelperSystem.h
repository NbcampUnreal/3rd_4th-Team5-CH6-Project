// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "A_FOR_INGAME/SECTION_WORLD/ResourceSpawn/Data/Struct/System/FTSResourceBucketArray.h"
#include "A_FOR_INGAME/SECTION_WORLD/ResourceSpawn/Data/Struct/System/FTSResourceNodeArray.h"
#include "Subsystems/WorldSubsystem.h"
#include "TSResourceNodeAndBucketGetHelperSystem.generated.h"

class ATSResourceBucketActor;
class ATSResourceNodeActor;
/**
 * 버킷, 노드 찾아서 전달해주는 시스템 : 자원 스폰 컨트롤 시스템의 지시를 받아 현재 존재하는 버킷, 노드를 반환해줌.
 * 세이브 로드와 상관없이 자원 노드와 버킷은 post에서 이 시스템에게 자신을 등록해야함.
 */
UCLASS()
class TINYSURVIVOR_API UTSResourceNodeAndBucketGetHelperSystem : public UWorldSubsystem
{
	
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
	
public:
	
	// 버킷, 노드 초기화 실시 함수 
	void InitResourceNodeAndBucketList();
	
	// 버킷, 노드 초기화 완료 함수 
	void InitResourceNodeAndBucketListComplete();
	
#pragma endregion
//======================================================================================================================		
};
