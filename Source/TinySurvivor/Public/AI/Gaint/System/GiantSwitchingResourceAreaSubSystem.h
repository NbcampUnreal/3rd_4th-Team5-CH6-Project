#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "TSGiantSwitchingAreaDataAsset.h"
#include "Subsystems/WorldSubsystem.h"
#include "GiantSwitchingResourceAreaSubSystem.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(GiantSwitchingResourceAreaSubSystem, Log, All)

class AGiantSwitchingResourceArea;
/**
 * 
 */
UCLASS()
class TINYSURVIVOR_API UGiantSwitchingResourceAreaSubSystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	

	//------------------------------
	// UGiantSwitchingResourceAreaSubSystem 라이프 사이클 
	//------------------------------
	
public:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	

	//------------------------------
	// UGiantSwitchingResourceAreaSubSystem 외부 유틸 
	//------------------------------
	
public:
	UFUNCTION(BlueprintCallable)
	static UGiantSwitchingResourceAreaSubSystem* GetGiantSwitchingResourceAreaSubSystem(const UObject* WorldContextObject);
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	

	//------------------------------
	// UGiantSwitchingResourceAreaSubSystem 섹터 관리 시스템
	//------------------------------

public:
	// 섹터 할당
	void AddResourceArea(const FGameplayTag& ResourceAreaIDTag, TWeakObjectPtr<AGiantSwitchingResourceArea> ResourceArea);

	// 가장 가까운 섹터 찾기
	AGiantSwitchingResourceArea* GetNearestResourceArea(const FVector& Location) const;
	
protected:
	// 섹터 지역 데이터
	// key : 섹터 태그 Value : 섹텨
	TMap<FGameplayTag, TWeakObjectPtr<AGiantSwitchingResourceArea> > ResourceAreaMap;
	
	// 섹터 클래스 데이터 에셋
	UPROPERTY()
	TObjectPtr<UTSGiantSwitchingAreaDataAsset> GiantSwitchingAreaData;

	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	

	//------------------------------
	// UGiantSwitchingResourceAreaSubSystem 관심 지역 시스템
	//------------------------------
	
public:
	// 관심 지역 추가
	UFUNCTION(BlueprintCallable)
	void AddIntersectArea(const FVector& Location);
	
	// 관심 지역 제거
	UFUNCTION(BlueprintCallable)
	void RemoveIntersectArea(const FVector& Location);

protected:
	// 관심 지역 데이터
	// Key : 위치 Value : 섹터 태그
	TMap<FVector, FGameplayTag> CurrentIntersectAreaMap;
};
