// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "TSItemTablesDataAsset.generated.h"

class UDataTable;
/**
 * 각 섹션마다 아이템 데이터 테이블을 한 곳에 모은 데이터 에셋
 */
UCLASS()
class TINYSURVIVOR_API UTSItemTablesDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
    
	// 아이템 기본 정보 테이블 모음 (F를 포함한 이름으로 지정)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TS | Item")
	TObjectPtr<UDataTable> ItemBaseInfoTable;

	// 아이템 인벤토리 정보 테이블 모음
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TS | Item")
	TObjectPtr<UDataTable> ItemInventoryInfoTable;

	// 아이템 UI 정보 테이블 모음 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TS | Item")
	TObjectPtr<UDataTable> ItemUIInfoTable;

};