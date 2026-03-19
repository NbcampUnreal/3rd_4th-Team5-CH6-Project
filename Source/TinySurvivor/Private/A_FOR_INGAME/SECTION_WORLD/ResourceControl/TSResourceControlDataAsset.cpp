// All CopyRight From YulRyongGameStudio //


#include "A_FOR_INGAME/SECTION_WORLD/ResourceControl/TSResourceControlDataAsset.h"

FPrimaryAssetId UTSResourceControlDataAsset::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(TEXT("ResourceControlData"), TEXT("DA_ResourceControlData"));
}

TArray<FResourceControlData*> UTSResourceControlDataAsset::GetAllRows()
{
	TArray<FResourceControlData*> Result;

	for (UDataTable* DT : ResourceDataTables)
	{
		if (!DT) continue;

		TArray<FResourceControlData*> Rows;
		DT->GetAllRows(TEXT("ResourceControlData"), Rows);

		Result.Append(Rows);
	}
	return Result;
}
