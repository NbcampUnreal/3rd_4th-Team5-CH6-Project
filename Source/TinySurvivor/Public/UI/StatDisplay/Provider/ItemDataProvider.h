#pragma once
#include "IDisplayDataProvider.h"

struct FItemData;

class TINYSURVIVOR_API FItemDataProvider : public IDisplayDataProvider
{
public:
	FItemDataProvider(const FItemData& ItemData)
		: ItemData(ItemData)
	{
	}
	virtual	FGameplayTag GetCategoryTag() const override;
	virtual float GetStatValue(const FGameplayTag& StatTag) const override;
	virtual FGameplayTagContainer GetTagContainer(const FGameplayTag& ContainerTag) const override;
	virtual FGameplayTag GetEffectTag() const override;
	virtual int32 GetEnumValue(const FGameplayTag& EnumTag) const override;
	

private:
	const FItemData& ItemData;
};
