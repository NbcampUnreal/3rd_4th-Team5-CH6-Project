// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "TSResourceUIInfoTable.generated.h"

/**
 * 
 */
USTRUCT(blueprinttype)
struct TINYSURVIVOR_API FTSResourceUIInfoTable : public FTableRowBase
{
	GENERATED_BODY()

	// 자원 ID
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = " TS | Resource")
	int32 ResourceID = -1;
	
	// 아이콘 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TS | Resource")
	TSoftObjectPtr<UTexture2D> ResourceIcon = nullptr;

	// 이름 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TS | Resource")
	FText ResourceName = FText::FromString(TEXT("No Name Edit"));
	
	// 설명
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TS | Resource")
	FText ResourceDescription = FText::FromString(TEXT("No Description Edit"));
	
};
