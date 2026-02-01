// ItemInstance.cpp
#include "Item/Runtime/ItemInstance.h"

FString FItemInstance::ToString() const
{
	return FString::Printf(
		TEXT("[ItemInstance] StaticDataID:%d, CreationTime:%.2f"),
		StaticDataID,
		CreationServerTime
	);
}