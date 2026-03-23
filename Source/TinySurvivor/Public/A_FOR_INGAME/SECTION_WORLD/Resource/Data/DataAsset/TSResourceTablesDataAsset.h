// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "TSResourceTablesDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class TINYSURVIVOR_API UTSResourceTablesDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	
	// 기본 정보 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TS | Resource")
	TObjectPtr<UDataTable> ResourceBaseInfoTable;
	
	// UI 정보
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TS | Resource")
	TObjectPtr<UDataTable> ResourceUIInfoTable;
	
	// 루팅 정보 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TS | Resource")
	TObjectPtr<UDataTable> ResourceLootInfoTable;
};
