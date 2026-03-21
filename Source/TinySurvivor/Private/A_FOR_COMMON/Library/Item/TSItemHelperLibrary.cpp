// All CopyRight From YulRyongGameStudio //


#include "A_FOR_COMMON/Library/Item/TSItemHelperLibrary.h"
#include "A_FOR_COMMON/Library/System/TSSystemGetterLibrary.h"

bool UTSItemHelperLibrary::GetItemData(const UObject* WorldContextObject, int32 InStaticDataID, FItemData& OutData)
{
	if (!IsValid(UTSSystemGetterLibrary::GetItemDataSubsystem(WorldContextObject))) return false;
	return UTSSystemGetterLibrary::GetItemDataSubsystem(WorldContextObject)->GetItemDataSafe(InStaticDataID, OutData);
}

int32 UTSItemHelperLibrary::GetDecayedItemID(const UObject* InWorldContextObject)
{
	if (!IsValid(UTSSystemGetterLibrary::GetDecayManager(InWorldContextObject))) return -1;
	return UTSSystemGetterLibrary::GetDecayManager(InWorldContextObject)->GetDecayItemID();
}

bool UTSItemHelperLibrary::GetDecayedItemData(const UObject* InWorldContextObject, FItemData& OutData)
{
	return GetItemData(InWorldContextObject, GetDecayedItemID(InWorldContextObject), OutData);
}
