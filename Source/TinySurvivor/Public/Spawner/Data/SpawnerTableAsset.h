// SpawnerTableAsset.h
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Engine/DataTable.h"
#include "SpawnerTableAsset.generated.h"

/*
	스폰 데이터 에셋
*/
UCLASS()
class TINYSURVIVOR_API USpawnerTableAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	// 스폰 데이터 테이블
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="DataTables",
		meta=(DisplayName="Spawn DataTable", ToolTip="스폰 데이터 테이블"))
	TObjectPtr<UDataTable> SpawnerDataTable;

#if WITH_EDITOR
	// 에디터에서 데이터 테이블 유효성 검증
	virtual EDataValidationResult IsDataValid(TArray<FText>& ValidationErrors) override
	{
		EDataValidationResult Result = EDataValidationResult::Valid;

		if (!SpawnerDataTable)
		{
			ValidationErrors.Add(FText::FromString(TEXT("SpawnerDataTable is not set!")));
			Result = EDataValidationResult::Invalid;
		}

		return Result;
	}
#endif
};
