// ItemTableAsset.h
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Engine/DataTable.h"
#include "ItemTableAsset.generated.h"

/*
	아이템 데이터 에셋
*/
UCLASS()
class TINYSURVIVOR_API UItemTableAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	// 아이템 데이터 테이블
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="DataTables")
	TObjectPtr<UDataTable> ItemDataTable;

	// 건축물 데이터 테이블
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="DataTables")
	TObjectPtr<UDataTable> BuildingDataTable;

	// 자원 원천 데이터 테이블
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="DataTables")
	TObjectPtr<UDataTable> ResourceDataTable;

#if WITH_EDITOR
	// 에디터에서 데이터 테이블 유효성 검증
	virtual EDataValidationResult IsDataValid(TArray<FText>& ValidationErrors) override
	{
		EDataValidationResult Result = EDataValidationResult::Valid;

		if (!ItemDataTable)
		{
			ValidationErrors.Add(FText::FromString(TEXT("ItemDataTable is not set!")));
			Result = EDataValidationResult::Invalid;
		}

		if (!BuildingDataTable)
		{
			ValidationErrors.Add(FText::FromString(TEXT("BuildingDataTable is not set!")));
			Result = EDataValidationResult::Invalid;
		}

		if (!ResourceDataTable)
		{
			ValidationErrors.Add(FText::FromString(TEXT("ResourceDataTable is not set!")));
			Result = EDataValidationResult::Invalid;
		}

		return Result;
	}
#endif
};