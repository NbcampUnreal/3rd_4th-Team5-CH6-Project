// BuildingRecipeData.h

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "BuildingRecipeData.generated.h"

#pragma region Enums
/*
	건축물 레시피의 카테고리를 나타내는 Enum
*/
UENUM(BlueprintType)
enum class EBuildingCategory : uint8
{
	NONE     UMETA(DisplayName = "None", ToolTip="미분류"),
	CRAFTING UMETA(DisplayName = "Crafting", ToolTip="제작"),
	LIGHT    UMETA(DisplayName = "Light", ToolTip="조명"),
	STRUCT   UMETA(DisplayName = "Struct", ToolTip="구조물"),
	STORAGE  UMETA(DisplayName = "Storage", ToolTip="보관함")
};
#pragma endregion

#pragma region Ingredient
/*
	건축 재료 정보
*/
USTRUCT(BlueprintType)
struct FBuildingIngredientData
{
	GENERATED_BODY()
	
public:
	FBuildingIngredientData()
		: MaterialID(0)
		, Count(1)
	{}
	
	FBuildingIngredientData(int32 InMaterialID, int32 InCount)
		: MaterialID(InMaterialID)
		, Count(InCount)
	{}
	
	// 재료 아이템 ID
	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		meta = (DisplayName="MaterialID (재료 아이템 ID)", ToolTip="필요한 재료의 아이템 ID"))
	int32 MaterialID;
	
	// 필요 개수
	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		meta = (ClampMin="1", DisplayName="Count (필요 개수)", ToolTip="해당 재료의 필요 개수"))
	int32 Count;
	
	// 유효성 검증
	bool IsValid() const
	{
		return MaterialID > 0 && Count > 0;
	}
};
#pragma endregion

#pragma region FBuildingRecipeData
/*
	건축물 레시피 데이터
*/
USTRUCT(BlueprintType)
struct FBuildingRecipeData : public FTableRowBase
{
	GENERATED_BODY()

public:
	// 생성자: FBuildingRecipeData 구조체 기본값 초기화
	FBuildingRecipeData()
		: RecipeID(0)                                      // 레시피 고유 ID 초기화 (0 = 기본/미지정)
		, Category(EBuildingCategory::NONE)                // 건축 카테고리 초기화 (기본값: NONE)
		, Name_KR(FText::FromString(TEXT("")))             // 건축물 한글명 초기화 (빈 문자열)
		, Name_EN(FText::FromString(TEXT("")))             // 건축물 영문명 초기화 (빈 문자열)
		, ResultItemID(0)                                  // 결과물 아이템 ID 초기화
		, ResultCount(1)                                   // 결과물 개수 초기화 (기본: 1)
		, BuildTime(1.0f)                                  // 건축 시간 초기화 (기본: 1초)
		, Ingredients()                                    // TArray 기본 생성자 호출 (배열 초기화)
	{}
	
#pragma region Identifier
	//==================================
	// 식별자
	//==================================
	// 레시피 고유 ID
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Identifier",
		meta=(ClampMin="1", ClampMax="9999", DisplayName="RecipeID (레시피 ID)", ToolTip="레시피 고유 ID"))
	int32 RecipeID;
	
	// 건축물 카테고리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Identifier",
		meta = (DisplayName="Category (카테고리)", ToolTip="레시피가 속한 카테고리"))
	EBuildingCategory Category;
	
	// 건축물 한글명
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Identifier",
		meta = (DisplayName="Name_KR (건축물 한글명)", ToolTip="건축물의 한글명"))
	FText Name_KR;
	
	// 건축물 영문명
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Identifier",
		meta = (DisplayName="Name_EN (건축물 영문명)", ToolTip="건축물의 영문명"))
	FText Name_EN;
#pragma endregion

#pragma region Result
	//==================================
	// 결과물
	//==================================
	// 결과물 아이템 ID
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Result",
		meta = (ClampMin="1", DisplayName="ResultItemID (결과물 아이템 ID)", ToolTip="건축 완료 시 생성되는 아이템 ID"))
	int32 ResultItemID;
	
	// 결과물 개수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Result",
		meta = (ClampMin="1", DisplayName="ResultCount (결과물 개수)", ToolTip="건축 완료 시 생성되는 아이템 개수"))
	int32 ResultCount;
#pragma endregion

#pragma region BuildingInfo
	//==================================
	// 건축 정보
	//==================================
	// 건축 시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BuildingInfo",
		meta = (ClampMin="0.1", DisplayName="BuildTime (건축 시간)", ToolTip="건축 완료까지 걸리는 시간(초 단위)"))
	float BuildTime;
	
	// 필요 재료 목록
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BuildingInfo",
		meta = (DisplayName="Ingredients (필요 재료 목록)", ToolTip="건축에 필요한 재료와 개수 목록"))
	TArray<FBuildingIngredientData> Ingredients;
#pragma endregion
	
#pragma region HelperFunctions
	//========================================
	// 헬퍼 함수
	//========================================
	
public:
	/*
		재료 목록이 유효한지 확인
		@return 재료가 하나 이상 있고 모두 유효하면 true
	*/
	bool HasValidIngredients() const
	{
		if (Ingredients.Num() == 0)
		{
			return false;
		}
		
		for (const FBuildingIngredientData& Ingredient : Ingredients)
		{
			if (!Ingredient.IsValid())
			{
				return false;
			}
		}
		return true;
	}
	
	/*
		특정 재료가 레시피에 포함되어 있는지 확인
		@param InMaterialID 확인할 재료 ID
		@return 포함되어 있으면 true
	*/
	bool ContainsMaterial(int32 InMaterialID) const
	{
		return GetRequiredCount(InMaterialID) > 0;
	}
	
	/*
		특정 재료의 필요 개수 조회
		@param InMaterialID 조회할 재료 ID
		@return 필요 개수 (해당 재료가 없으면 0 반환)
	*/
	int32 GetRequiredCount(int32 InMaterialID) const
	{
		for (const FBuildingIngredientData& Ingredient : Ingredients)
		{
			if (Ingredient.MaterialID == InMaterialID)
			{
				return Ingredient.Count;
			}
		}
		return 0;
	}
	
	/*
		총 필요 재료 종류 개수 조회
		@return 필요한 재료 종류 수
	*/
	int32 GetIngredientTypeCount() const
	{
		return Ingredients.Num();
	}
	
	/*
		레시피 데이터가 올바른지 검증
		@return 기본 데이터가 모두 유효하면 true
	*/
	bool IsValidRecipe() const
	{
		return RecipeID > 0 
			&& ResultItemID > 0 
			&& ResultCount > 0 
			&& BuildTime > 0.0f 
			&& HasValidIngredients();
	}
#pragma endregion
	
#pragma region Validation
	/*
		레시피 데이터 유효성 검증
		@return 레시피 데이터가 올바르게 설정되어 있으면 true
	*/
	bool ValidateRecipeData() const
	{
		// 기본 ID 검증
		if (RecipeID <= 0 || ResultItemID <= 0)
		{
			return false;
		}
		
		// 결과물 개수 검증
		if (ResultCount <= 0)
		{
			return false;
		}
		
		// 건축 시간 검증
		if (BuildTime <= 0.0f)
		{
			return false;
		}
		
		// 재료 목록 검증
		if (!HasValidIngredients())
		{
			return false;
		}
		
		return true;
	}
#pragma endregion
	
#pragma region Debug
	/*
		디버그용 상세 정보 로그 출력
	*/
	void PrintDebugInfo() const;
#pragma endregion
};
#pragma endregion