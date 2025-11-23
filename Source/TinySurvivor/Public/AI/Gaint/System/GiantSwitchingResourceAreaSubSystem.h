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
	
public:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable)
	static UGiantSwitchingResourceAreaSubSystem* GetGiantSwitchingResourceAreaSubSystem(const UObject* WorldContextObject);
	
	void AddResourceArea(const FGameplayTag& ResourceAreaIDTag, TWeakObjectPtr<AGiantSwitchingResourceArea> ResourceArea);

	AGiantSwitchingResourceArea* GetNearestResourceArea(const FVector& Location) const;
	
	UFUNCTION(BlueprintCallable)
	void AddIntersectArea(const FVector& Location);
	
	UFUNCTION(BlueprintCallable)
	void RemoveIntersectArea(const FVector& Location);
	
protected:
	TMap<FGameplayTag /*ResourceAreaIDTag*/, TWeakObjectPtr<AGiantSwitchingResourceArea> /*지역*/> ResourceAreaMap;
	TMap<FVector /*Location*/, FGameplayTag  /*ResourceAreaIDTag*/> CurrentIntersectAreaMap;
	
	UPROPERTY()
	TObjectPtr<UTSGiantSwitchingAreaDataAsset> GiantSwitchingAreaData;
};
