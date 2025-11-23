#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "GameplayTagContainer.h"
#include "Item/Data/ResourceData.h"
#include "TSResourceControlSubSystem.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(ResourceControlSystem, Log, All);

class ATSResourcePoint;
struct FGameplayTag;

USTRUCT(BlueprintType)
struct FResourcePoints
{
	GENERATED_BODY()
	
	// 포인트 타입
	ENodeType NodeType = ENodeType::NONE;
	
	// 포인트들
	TArray<TObjectPtr<ATSResourcePoint>> CachedResourcePoints = {};
};

USTRUCT(BlueprintType)
struct FResourcePointBucket
{
	GENERATED_BODY()
	
	// 포인트 버킷들 (각 섹터마다 존재하는 다른 타입의 노드 바구니들)
	TArray<FResourcePoints> ResourcePointBucket = {};
};


/**
 * 
 */
UCLASS()
class TINYSURVIVOR_API UTSResourceControlSubSystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	

	//------------------------------
	// UTSResourceControlSubSystem 라이프 사이클 
	//------------------------------
	
public:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	virtual void Deinitialize() override;
	
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	

	//------------------------------
	// UTSResourceControlSubSystem 외부 유틸
	//------------------------------
	
public:
	UFUNCTION(BlueprintCallable)
	static UTSResourceControlSubSystem* GetUTSResourceControlSubSystem(const UObject* WorldContextObject);
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	

	//------------------------------
	// UTSResourceControlSubSystem 자원 포인트
	//------------------------------
	
public:
	// 자원 포인트들이 버킷에 등록하기 위해 호출하는 API
	void AssignReSourcePointToSector(const TObjectPtr<ATSResourcePoint> ResourcePoint);
	
protected:
	// 자원 포인트들
	// Key : 섹터명  value : 각 섹터마다 존재하는 다른 타입의 노드 바구니들
	TMap<FGameplayTag, FResourcePointBucket> ResourcePointMap;
	
	// 버킷 확인용 디버깅 함수
	void DebugPrintAllBuckets();
};
