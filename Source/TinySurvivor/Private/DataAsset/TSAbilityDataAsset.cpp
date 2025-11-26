#include "DataAsset/TSAbilityDataAsset.h"

FPrimaryAssetId UTSAbilityDataAsset::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(TEXT("AbilityData"), TEXT("DA_AbilityData"));
}
