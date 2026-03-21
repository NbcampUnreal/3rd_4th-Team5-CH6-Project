// All CopyRight From YulRyongGameStudio //


#include "A_FOR_COMMON/Library/System/TSDecayLibrary.h"
#include "A_FOR_COMMON/Library/System/TSTimeLibrary.h"

double UTSDecayLibrary::CalculateExpirationTime(const UObject* InWorldContextObject, double InCurrentExpirationTime, int32 InCurrentStack, int32 InNewItemStack, float InDecayRate)
{
	if (!IsValid(InWorldContextObject)) return 0.0;
	
	double NewItemExpirationTime = static_cast<double>(UTSTimeLibrary::GetCurrentTime(InWorldContextObject)) + InDecayRate;
	return (InCurrentExpirationTime * InCurrentStack + NewItemExpirationTime * InNewItemStack) / (InCurrentStack + InNewItemStack);
}

float UTSDecayLibrary::CalculateDecayPercent(const UObject* InWorldContextObject, double InCurrentExpirationTime, float InDecayRate)
{
	if (!IsValid(InWorldContextObject)) return 0.0f;
	
	double CurrentTime = (double)UTSTimeLibrary::GetCurrentTime(InWorldContextObject);
	return InDecayRate > 0 ? static_cast<float>((InCurrentExpirationTime - CurrentTime) / InDecayRate) : 0.f;
}
