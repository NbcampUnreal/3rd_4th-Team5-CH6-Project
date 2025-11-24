// CraftingData.h
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "CraftingData.generated.h"

#pragma region Enums
/*
	크래프팅 레시피의 카테고리를 나타내는 Enum
*/
UENUM(BlueprintType)
enum class ECraftingCategory : uint8
{
	NONE        UMETA(DisplayName = "None", ToolTip="미분류"),
	TOOL        UMETA(DisplayName = "Tool", ToolTip="도구"),
	WEAPON      UMETA(DisplayName = "Weapon", ToolTip="무기"),
	ARMOR       UMETA(DisplayName = "Armor", ToolTip="방어구"),
	CONSUMABLE  UMETA(DisplayName = "Consumable", ToolTip="소모품"),
	//BUILDING    UMETA(DisplayName = "Building", ToolTip="건축")
};
#pragma endregion

#pragma region Ingredient
/*
	크래프팅 재료 정보
*/
USTRUCT(BlueprintType)
struct FIngredientData
{
	GENERATED_BODY()
	
public:
	FIngredientData()
		: MaterialID(0)
		, Count(1)
	{}
	
	FIngredientData(int32 InMaterialID, int32 InCount)
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

#pragma region FCraftingData
/*
	크래프팅 레시피 데이터
*/
USTRUCT(BlueprintType)
struct FCraftingData : public FTableRowBase
{
	GENERATED_BODY()

public:
	// 생성자: FCraftingData 구조체 기본값 초기화
	FCraftingData()
		: RecipeID(0)                                      // 레시피 고유 ID 초기화 (0 = 기본/미지정)
		, Category(ECraftingCategory::NONE)                // 크래프팅 카테고리 초기화 (기본값: NONE)
		, ResultName_KR(FText::FromString(TEXT("")))       // 결과물 한글명 초기화 (빈 문자열)
		, ResultName_EN(FText::FromString(TEXT("")))       // 결과물 영문명 초기화 (빈 문자열)
		, ResultItemID(0)                                  // 결과물 아이템 ID 초기화
		, ResultCount(1)                                   // 결과물 개수 초기화 (기본: 1)
		, CraftTime(1.0f)                                  // 제작 시간 초기화 (기본: 1초)
		, Ingredients()                                    // TArray 기본 생성자 호출 (배열 초기화)
		//, RequiredCraftingStation(0)                       // 필요 제작대 ID 초기화
		//, RequiredLevel(1)                                 // 필요 레벨 초기화 (기본: 1)
		//, bUnlocked(true)                                  // 잠금 여부 초기화 (기본: 해금됨)
	{}

#pragma region Identifier
	//==================================
	// 식별자
	//==================================
	// 레시피 고유 ID
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Identifier",
		meta=(ClampMin="1", ClampMax="9999", DisplayName="RecipeID (레시피 ID)", ToolTip="레시피 고유 ID"))
	int32 RecipeID;

	// 크래프팅 카테고리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Identifier",
		meta = (DisplayName="Category (카테고리)", ToolTip="레시피가 속한 카테고리"))
	ECraftingCategory Category;

	// 결과물 한글명
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Identifier",
		meta = (DisplayName="ResultName_KR (결과물 한글명)", ToolTip="결과물의 한글명"))
	FText ResultName_KR;
	
	// 결과물 영문명
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Identifier",
		meta = (DisplayName="ResultName_EN (결과물 영문명)", ToolTip="결과물의 영문명"))
	FText ResultName_EN;
#pragma endregion

#pragma region Result
	//==================================
	// 결과물
	//==================================
	// 결과물 아이템 ID
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Result",
		meta = (ClampMin="1", DisplayName="ResultItemID (결과물 아이템 ID)", ToolTip="제작 완료 시 생성되는 아이템 ID"))
	int32 ResultItemID;

	// 결과물 개수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Result",
		meta = (ClampMin="1", DisplayName="ResultCount (결과물 개수)", ToolTip="제작 완료 시 생성되는 아이템 개수"))
	int32 ResultCount;
#pragma endregion

#pragma region CraftingInfo
	//==================================
	// 제작 정보
	//==================================
	// 제작 시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CraftingInfo",
		meta = (ClampMin="0.1", DisplayName="CraftTime (제작 시간)", ToolTip="제작 완료까지 걸리는 시간(초 단위)"))
	float CraftTime;

	// 필요 재료 목록
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CraftingInfo",
		meta = (DisplayName="Ingredients (필요 재료 목록)", ToolTip="제작에 필요한 재료와 개수 목록"))
	TArray<FIngredientData> Ingredients;
#pragma endregion

#pragma region Requirements
	//==================================
	// 제작 요구사항
	//==================================
	// // 필요 제작대 ID
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Recipe-Requirements",
	// 	meta = (ClampMin="0", DisplayName="RequiredCraftingStation (필요 제작대 ID)", 
	// 		ToolTip="제작에 필요한 제작대 ID"))
	// int32 RequiredCraftingStation;
	//
	// // 필요 레벨
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Recipe-Requirements",
	// 	meta = (ClampMin="1", DisplayName="RequiredLevel (필요 레벨)", ToolTip="레시피 사용에 필요한 최소 레벨"))
	// int32 RequiredLevel;
	//
	// // 레시피 잠금 여부
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Recipe-Requirements",
	// 	meta = (DisplayName="Unlocked (잠금 해제 여부)", ToolTip="레시피 잠금 해제 여부 (false = 잠김)"))
	// bool bUnlocked;
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
		
		for (const FIngredientData& Ingredient : Ingredients)
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
		for (const FIngredientData& Ingredient : Ingredients)
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
		제작대가 필요한지 확인
		@return 제작대가 필요하면 true
	*/
	// bool RequiresCraftingStation() const
	// {
	// 	return RequiredCraftingStation > 0;
	// }
	
	/*
		레시피가 잠겨있는지 확인
		@return 잠겨있으면 true
	*/
	// bool IsLocked() const
	// {
	// 	return !bUnlocked;
	// }
	
	/*
		레시피 데이터가 올바른지 검증
		@return 기본 데이터가 모두 유효하면 true
	*/
	bool IsValidRecipe() const
	{
		return RecipeID > 0 
			&& ResultItemID > 0 
			&& ResultCount > 0 
			&& CraftTime > 0.0f 
			&& HasValidIngredients();
			//&& bUnlocked;
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
		
		// 제작 시간 검증
		if (CraftTime <= 0.0f)
		{
			return false;
		}
		
		// 재료 목록 검증
		if (!HasValidIngredients())
		{
			return false;
		}
		
		// 필수 레벨 검증
		// if (RequiredLevel <= 0)
		// {
		// 	return false;
		// }
		
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