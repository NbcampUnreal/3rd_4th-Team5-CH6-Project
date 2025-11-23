#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TSResourceItemInterface.generated.h"

UINTERFACE()
class UTSResourceItemInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class TINYSURVIVOR_API ITSResourceItemInterface
{
	GENERATED_BODY()

public:
	virtual void GetItemFromResource(int32 RequiredToolID, FVector HitPoint, FVector HitNormal, FVector PlayerLocation, FVector ForwardVector) = 0;
};
