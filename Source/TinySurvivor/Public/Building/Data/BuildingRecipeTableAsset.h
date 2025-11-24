// BuildingRecipeTableAsset.h
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Engine/DataTable.h"
#include "BuildingRecipeTableAsset.generated.h"

/*
	건축물 레시피 데이터 에셋
*/
UCLASS()
class TINYSURVIVOR_API UBuildingRecipeTableAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	// 건축물 레시피 데이터 테이블
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="DataTables",
		meta=(DisplayName="Building Recipe DataTable", ToolTip="건축물 레시피 데이터 테이블"))
	TObjectPtr<UDataTable> BuildingRecipeDataTable;
	
#if WITH_EDITOR
	// 에디터에서 데이터 테이블 유효성 검증
	virtual EDataValidationResult IsDataValid(TArray<FText>& ValidationErrors) override
	{
		EDataValidationResult Result = EDataValidationResult::Valid;
		
		if (!BuildingRecipeDataTable)
		{
			ValidationErrors.Add(FText::FromString(TEXT("BuildingRecipeDataTable is not set!")));
			Result = EDataValidationResult::Invalid;
		}
		
		return Result;
	}
#endif
};
