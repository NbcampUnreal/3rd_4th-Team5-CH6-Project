// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "A_FOR_INGAME/SECTION_ITEM/Item/Data/ItemData.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "TSItemHelperLibrary.generated.h"

/**
 * 아이템 관련 헬퍼 API 라이브러리  
 */
UCLASS()
class TINYSURVIVOR_API UTSItemHelperLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	// 아이템 데이터 게터 (레퍼런스로 주고 받음)
	UFUNCTION(BlueprintCallable, Category = "TSLibrary | ItemHelperLibrary")
	static bool GetItemData(const UObject* InWorldContextObject, int32 InStaticDataID, FItemData& OutData);










};
