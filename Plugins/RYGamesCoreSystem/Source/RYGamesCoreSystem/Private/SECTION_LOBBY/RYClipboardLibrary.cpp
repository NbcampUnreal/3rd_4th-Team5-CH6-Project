// All CopyRight From YulRyongGameStudio //


#include "SECTION_LOBBY/RYClipboardLibrary.h"
#include "HAL/PlatformApplicationMisc.h"

void URYClipboardLibrary::CopyToClipboard(const FString& Text)
{
	FPlatformApplicationMisc::ClipboardCopy(*Text);
}
