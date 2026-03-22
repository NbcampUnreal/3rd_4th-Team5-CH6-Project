// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "TSItemUIInfoTable.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FTSItemUIInfoTable : public FTableRowBase
{
	GENERATED_BODY()
	
	// 아이템 ID 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = " TS | Item")
	int32 ItemID = -1;
	
	// 아이템 아이콘 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TS | Item")
	TSoftObjectPtr<UTexture2D> ItemIcon = nullptr;
	
	// 아이템 이름 (영어)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TS | Item")
	FText ItemName = FText::FromString(TEXT("No Name Edit"));
	
	// 아이템 설명 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TS | Item")
	FText ItemDescription = FText::FromString(TEXT("No Description Edit"));
	
};
