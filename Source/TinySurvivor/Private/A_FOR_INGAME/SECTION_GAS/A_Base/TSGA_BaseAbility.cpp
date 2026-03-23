// All CopyRight From YulRyongGameStudio //


#include "A_FOR_INGAME/SECTION_GAS/A_Base/TSGA_BaseAbility.h"
#include "A_FOR_COMMON/Tag/State/TSStateTag.h"

UTSGA_BaseAbility::UTSGA_BaseAbility()
{
	ActivationBlockedTags.AddTag(StateTag::PrimarayState::TS_State_Dead);
	ActivationBlockedTags.AddTag(StateTag::PrimarayState::TS_State_Down);
}
