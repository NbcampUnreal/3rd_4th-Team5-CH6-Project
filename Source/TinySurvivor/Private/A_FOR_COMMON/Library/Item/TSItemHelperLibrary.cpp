// All CopyRight From YulRyongGameStudio //


#include "A_FOR_COMMON/Library/Item/TSItemHelperLibrary.h"
#include "A_FOR_COMMON/Library/System/TSSystemGetterLibrary.h"

bool UTSItemHelperLibrary::GetItemData(const UObject* WorldContextObject, int32 InStaticDataID, FItemData& OutData)
{
	if (InStaticDataID == 0) return false;
	if (!IsValid(WorldContextObject)) return false;
	if (!IsValid(UTSSystemGetterLibrary::GetItemDataSubsystem(WorldContextObject))) return false;
	
	return UTSSystemGetterLibrary::GetItemDataSubsystem(WorldContextObject)->GetItemDataSafe(InStaticDataID, OutData);
}
