#pragma once
#include "IDisplayDataProvider.h"

class UItemDataSubsystem;
struct FBuildingData;

class TINYSURVIVOR_API FBuildingDataProvider : public IDisplayDataProvider
{
public:
	FBuildingDataProvider(const FBuildingData& BuildingData)
		: BuildingData(BuildingData)
		  , WorldContext(nullptr)
	{
	}

	virtual FGameplayTag GetCategoryTag() const override;
	virtual float GetStatValue(const FGameplayTag& StatTag) const override;
	virtual FGameplayTagContainer GetTagContainer(const FGameplayTag& ContainerTag) const override;
	virtual FGameplayTag GetEffectTag() const override;
	virtual int32 GetEnumValue(const FGameplayTag& EnumTag) const override;

	virtual FText GetItemName(const FGameplayTag& StatTag) const override;

	virtual void SetWorldContext(const UObject* InWorldContext) override
	{
		WorldContext = InWorldContext;
	}

private:
	const FBuildingData& BuildingData;
	const UObject* WorldContext;
};
