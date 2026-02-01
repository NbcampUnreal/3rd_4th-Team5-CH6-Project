// Fill out your copyright notice in the Description page of Project Settings.

#include "System/Erosion/ErosionConfigData.h"

FPrimaryAssetId UErosionConfigData::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(TEXT("ErosionConfigData"), TEXT("DA_ErosionConfig_Default"));
}
