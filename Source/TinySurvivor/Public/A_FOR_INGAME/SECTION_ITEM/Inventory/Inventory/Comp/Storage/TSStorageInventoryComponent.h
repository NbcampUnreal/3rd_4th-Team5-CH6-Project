// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "A_FOR_INGAME/SECTION_ITEM/Inventory/Inventory/Comp/A_Master/TSInventoryMasterComponent.h"
#include "TSStorageInventoryComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TINYSURVIVOR_API UTSStorageInventoryComponent : public UTSInventoryMasterComponent
{
	GENERATED_BODY()

public:
	UTSStorageInventoryComponent();
};
