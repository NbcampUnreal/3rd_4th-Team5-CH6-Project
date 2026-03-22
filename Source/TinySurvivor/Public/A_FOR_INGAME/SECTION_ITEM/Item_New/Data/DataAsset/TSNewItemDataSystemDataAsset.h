// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "TSNewItemDataSystemDataAsset.generated.h"

class UTSNewItemTablesDataAsset;
/**
 * 아이템 데이터 매니저가 세팅으로부터 불러오는 데이터 에셋 
 */
UCLASS()
class TINYSURVIVOR_API UTSNewItemDataSystemDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:

	// 아이템 정적 데이터 데이터 에셋 모음
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TS | Item")
	TArray<TObjectPtr<UTSNewItemTablesDataAsset>> ItemInfoDataAssetArray;
	
};
