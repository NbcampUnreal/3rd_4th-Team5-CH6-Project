// All CopyRight From YulRyongGameStudio //


#include "A_FOR_INGAME/SECTION_UI/Interact/TSItemInteractUI.h"
#include "Components/TextBlock.h"

void UTSItemInteractUI::SetItemRemainStackInfo(int32 InRemainStack)
{
	if (!IsValid(ItemRemainStackInfoTextBlock)) return;
	ItemRemainStackInfoTextBlock->SetText(FText::AsNumber(InRemainStack));
}
