// All CopyRight From YulRyongGameStudio //


#pragma once

#include "CoreMinimal.h"
#include "TSReousrceControlSystemDataTable.h"
#include "Engine/DataAsset.h"
#include "TSResourceControlDataAsset.generated.h"

class UDataTable;
/**
 * 
 */
UCLASS()
class TINYSURVIVOR_API UTSResourceControlDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
	TArray<FResourceControlData*> GetAllRows();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ResourceDataTables")
	TArray<TObjectPtr<UDataTable>> ResourceDataTables;

	// 디버깅
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Erosion | Debug")
	bool bShowDebug = false;
	
};
