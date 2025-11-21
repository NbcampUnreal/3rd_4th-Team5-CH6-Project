// CraftingTableAsset.h
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Engine/DataTable.h"
#include "CraftingTableAsset.generated.h"

/*
	크래프팅 레시피 데이터 에셋
*/
UCLASS()
class TINYSURVIVOR_API UCraftingTableAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	// 크래프팅 레시피 데이터 테이블
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="DataTables",
		meta=(DisplayName="Crafting Recipe DataTable", ToolTip="크래프팅 레시피 데이터 테이블"))
	TObjectPtr<UDataTable> CraftingDataTable;

#if WITH_EDITOR
	// 에디터에서 데이터 테이블 유효성 검증
	virtual EDataValidationResult IsDataValid(TArray<FText>& ValidationErrors) override
	{
		EDataValidationResult Result = EDataValidationResult::Valid;

		if (!CraftingDataTable)
		{
			ValidationErrors.Add(FText::FromString(TEXT("CraftingDataTable is not set!")));
			Result = EDataValidationResult::Invalid;
		}

		return Result;
	}
#endif
};
