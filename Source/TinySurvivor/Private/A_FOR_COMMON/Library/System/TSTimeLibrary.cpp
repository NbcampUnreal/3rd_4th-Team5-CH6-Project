// All CopyRight From YulRyongGameStudio //


#include "A_FOR_COMMON/Library/System/TSTimeLibrary.h"

float UTSTimeLibrary::GetCurrentTime(const UObject* InWorldContextObject)
{
	if (!IsValid(InWorldContextObject)) return 0.0f;
	if (!IsValid(InWorldContextObject->GetWorld())) return 0.0f;
	return InWorldContextObject->GetWorld()->GetTimeSeconds();
}
