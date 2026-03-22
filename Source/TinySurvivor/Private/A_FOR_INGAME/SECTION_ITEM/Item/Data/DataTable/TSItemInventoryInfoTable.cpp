// All CopyRight From YulRyongGameStudio //


#include "A_FOR_INGAME/SECTION_ITEM/Item/Data/DataTable/TSItemInventoryInfoTable.h"

FTSItemInventoryInfoTable::FTSItemInventoryInfoTable()
{
	// 인벤토리 타입
	//-----------------------------------------------------------------
	// 플레이어 (장비 제외)
	MatchInventoryCompType.Add(ETSInventoryCompType::PlayerHotKey);
	MatchInventoryCompType.Add(ETSInventoryCompType::PlayerBackPack);
	
	// AI (장비 제외)
	MatchInventoryCompType.Add(ETSInventoryCompType::AIBackPack);
	
	// 보관함 (기본)
	MatchInventoryCompType.Add(ETSInventoryCompType::Storage);
	//-----------------------------------------------------------------
	
	
	// 인벤토리 슬롯 타입
	//-----------------------------------------------------------------
	MatchInventorySlotType.Add(ETSInventorySlotType::Common);
	//-----------------------------------------------------------------

}
