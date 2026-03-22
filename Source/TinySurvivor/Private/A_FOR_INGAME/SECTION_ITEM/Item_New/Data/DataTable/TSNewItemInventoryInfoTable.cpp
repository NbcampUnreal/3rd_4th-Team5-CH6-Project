// All CopyRight From YulRyongGameStudio //


#include "A_FOR_INGAME/SECTION_ITEM/Item_New/Data/DataTable/TSNewItemInventoryInfoTable.h"

FTSNewItemInventoryInfoTable::FTSNewItemInventoryInfoTable()
{
	// 인벤토리 타입
	//-----------------------------------------------------------------
	// 플레이어 (장비 제외)
	MatchInventoryCompType.Add(ETSNewInventoryCompType::PlayerHotKey);
	MatchInventoryCompType.Add(ETSNewInventoryCompType::PlayerBackPack);
	
	// AI (장비 제외)
	MatchInventoryCompType.Add(ETSNewInventoryCompType::AIBackPack);
	
	// 보관함 (기본)
	MatchInventoryCompType.Add(ETSNewInventoryCompType::Storage);
	//-----------------------------------------------------------------
	
	
	// 인벤토리 슬롯 타입
	//-----------------------------------------------------------------
	MatchInventorySlotType.Add(ETSNewInventorySlotType::Common);
	//-----------------------------------------------------------------

}
