// All CopyRight From YulRyongGameStudio //


#include "A_FOR_INGAME/SECTION_UI/Interact/TSResourceInteractUI.h"
#include "Components/TextBlock.h"

void UTSResourceInteractUI::SetResourceRemainStackInfo(int32 InRemainStack, int32 InMaxStack)
{
	if (!IsValid(ResourceRemainStackInfoTextBlock)) return;
	
	// 0이 들어와서 Divide by Zero가 발생하는 것을 방지
	if (InMaxStack <= 0) 
	{
		ResourceRemainStackInfoTextBlock->SetText(FText::FromString(TEXT("0%")));
		return;
	}
	
	// 1. 퍼센트 계산 (0.7 같은 형태)
	float Percentage = static_cast<float>(InRemainStack) / static_cast<float>(InMaxStack);
    
	// 2. 100을 곱해서 정수로 변환 (70)
	int32 PercentInt = FMath::RoundToInt(Percentage * 100.f);

	// 3. 포맷을 사용하여 "70%" 형태로 제작
	FText DisplayText = FText::Format(FText::FromString(TEXT("{0}%")), FText::AsNumber(PercentInt));

	ResourceRemainStackInfoTextBlock->SetText(DisplayText);
}
