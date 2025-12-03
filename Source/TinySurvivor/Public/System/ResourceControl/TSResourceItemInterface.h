#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Item/Data/Common/ItemCommonEnums.h"
#include "TSResourceItemInterface.generated.h"

class UAbilitySystemComponent;

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
	virtual void GetItemFromResource(UAbilitySystemComponent* ASC, EItemAnimType& RequiredToolType, int32& ATK, FVector& HitPoint, FVector& HitNormal, FVector PlayerLocation, FVector ForwardVector, bool IsLeftMouseCLicked) = 0;
};
