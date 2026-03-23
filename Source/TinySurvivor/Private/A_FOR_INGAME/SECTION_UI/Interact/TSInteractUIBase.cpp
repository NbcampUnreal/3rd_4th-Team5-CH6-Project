// All CopyRight From YulRyongGameStudio //


#include "A_FOR_INGAME/SECTION_UI/Interact/TSInteractUIBase.h"
#include "Components/TextBlock.h"

void UTSInteractUIBase::SetInteractInfo(FText& InInteractActorName, FText& InInteractKey)
{
	if (!IsValid(InteractActorNameTextBlock)) return;
	if (!IsValid(InteractKeyTextBlock)) return;

	FText ActorName = InInteractActorName;
	FText Key = InInteractKey;
	
	InteractActorNameTextBlock->SetText(ActorName);
	InteractKeyTextBlock->SetText(Key);
}
