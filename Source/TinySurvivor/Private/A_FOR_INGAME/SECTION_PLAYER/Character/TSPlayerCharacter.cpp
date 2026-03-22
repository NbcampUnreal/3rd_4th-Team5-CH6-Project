// All CopyRight From YulRyongGameStudio //


#include "A_FOR_INGAME/SECTION_PLAYER/Character/TSPlayerCharacter.h"

#include "A_FOR_INGAME/SECTION_ITEM/Inventory/Inventory/Comp/BackPack/TSBackPackInventoryComponent.h"
#include "A_FOR_INGAME/SECTION_ITEM/Inventory/Inventory/Comp/Body/TSBodyInventoryComponent.h"
#include "A_FOR_INGAME/SECTION_ITEM/Inventory/Inventory/Comp/HotKey/TSHotKeyInventoryComponent.h"

//======================================================================================================================	
#pragma region 라이프_사이클
	
	
	//━━━━━━━━━━━━━━━━━━━━
	// 라이프 사이클
	//━━━━━━━━━━━━━━━━━━━━	

ATSPlayerCharacter::ATSPlayerCharacter()
{
	// 인벤토리 
	HotKeyInventoryComponent = CreateDefaultSubobject<UTSHotKeyInventoryComponent>(TEXT("HotKeyInventoryComponent"));
	BackpackInventoryComponent = CreateDefaultSubobject<UTSBackPackInventoryComponent>(TEXT("BackpackInventoryComponent"));
	EquipmentInventoryComponent = CreateDefaultSubobject<UTSBodyInventoryComponent>(TEXT("EquipmentInventoryComponent"));
}

#pragma endregion
//======================================================================================================================
