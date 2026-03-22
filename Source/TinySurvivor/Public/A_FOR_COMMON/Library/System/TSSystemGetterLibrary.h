// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "A_FOR_INGAME/SECTION_ITEM/Item/Runtime/ItemDecaySubSystem.h"
#include "A_FOR_INGAME/SECTION_ITEM/Item/System/ItemDataSubsystem.h"
#include "A_FOR_INGAME/SECTION_ITEM/Item/System/WorldItemPoolSubsystem.h"
#include "A_FOR_INGAME/SECTION_ITEM/Item_New/System/TSNewItemDataSubSystem.h"
#include "A_FOR_INGAME/SECTION_UI/TagDisplay/System/GameplayDisplaySubSystem.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "TSSystemGetterLibrary.generated.h"

/**
 * 시스템 게터 라이브러리 
 * 헤더 파일 중복 및 헤더 파일 선언 코드 줄 과도하게 생기는 것 방지용 게터 라이브러러리 
 */
UCLASS()
class TINYSURVIVOR_API UTSSystemGetterLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	
	// 아이템 데이터 서브 시스템 게터 (OLD)
	UFUNCTION(BlueprintCallable, Category = "TSLibrary | ASCLibrary")
	static UItemDataSubsystem* GetItemDataSubsystem(const UObject* InWorldContextObject);

	// 아이템 데이터 서브 시스템 게터 (New)
	UFUNCTION(BlueprintCallable, Category = "TSLibrary | ASCLibrary")
	static UTSNewItemDataSubSystem* GetNewItemDataSubsystem(const UObject* InWorldContextObject); 
	
	
	// 풀링 시스템 게터
	UFUNCTION(BlueprintCallable, Category = "TSLibrary | ASCLibrary")
	static UWorldItemPoolSubsystem* GetWorldItemPoolSubsystem(const UObject* InWorldContextObject);
	
	// 부패도 시스템 게터
	UFUNCTION(BlueprintCallable, Category = "TSLibrary | ASCLibrary")
	static UItemDecaySubSystem* GetDecayManager(const UObject* InWorldContextObject);
	
	// 태그 디스플레이 서브 시스템 게터
	UFUNCTION(BlueprintCallable, Category = "TSLibrary | ASCLibrary")
	static UGameplayTagDisplaySubsystem* GetGameplayTagDisplaySubsystem(const UObject* InWorldContextObject);
	
	
};
