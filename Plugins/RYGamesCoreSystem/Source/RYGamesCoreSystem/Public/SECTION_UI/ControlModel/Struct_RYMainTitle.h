// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "Enum_RYMainTitle.h"
#include "Struct_RYMainTitle.generated.h"

// 서브 위젯이 서브 위젯 델리게이트를 통해 보내는 정보
USTRUCT(BlueprintType)
struct FRYMainTitleSubWidgetMessage
{
	GENERATED_BODY()
	
	// 서브 위젯 타입
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MainTitle | SubWidget", DisplayName="서브 위젯 타입")
	ERYMainTitleNoticeType SubWidgetNoticeType = ERYMainTitleNoticeType::None; 
	
	// 알림 정보 타입
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MainTitle | SubWidget", DisplayName="알림 정보 타입")
	ERYMainTitleNoticeMode SubWidgetNoticeMode = ERYMainTitleNoticeMode::None;
	
	// ** 만약 세이브 관련일 경우 **
	// 삭제 또는 생생해야하는 슬롯 정보 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MainTitle | SubWidget", DisplayName="삭제 또는 생성해야하는 슬롯 정보")
	int32 SlotNumber = -1;
	
};
