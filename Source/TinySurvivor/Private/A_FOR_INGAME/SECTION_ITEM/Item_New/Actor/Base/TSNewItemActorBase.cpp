// All CopyRight From YulRyongGameStudio //


#include "A_FOR_INGAME/SECTION_ITEM/Item_New/Actor/Base/TSNewItemActorBase.h"


ATSNewItemActorBase::ATSNewItemActorBase()
{
	PrimaryActorTick.bCanEverTick = false;
	
	bReplicates = true;
	SetReplicatingMovement(true);
}


