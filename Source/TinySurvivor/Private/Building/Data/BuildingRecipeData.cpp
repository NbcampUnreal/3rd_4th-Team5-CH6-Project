// BuildingRecipeData.cpp
#include "Building/Data/BuildingRecipeData.h"

// 로그 카테고리 정의
DEFINE_LOG_CATEGORY_STATIC(LogFBuildingRecipeData, Log, All);

void FBuildingRecipeData::PrintDebugInfo() const
{
	UE_LOG(LogFBuildingRecipeData, Display, TEXT("============= Building Recipe Debug Info ============="));
	
	// Identifier
	UE_LOG(LogFBuildingRecipeData, Display, TEXT("---[Identifier]"));
	UE_LOG(LogFBuildingRecipeData, Display, TEXT("RecipeID: %d"), RecipeID);
	UE_LOG(LogFBuildingRecipeData, Display, TEXT("Category: %s"), *UEnum::GetValueAsString(Category));
	UE_LOG(LogFBuildingRecipeData, Display, TEXT("Name_KR: %s"), *Name_KR.ToString());
	UE_LOG(LogFBuildingRecipeData, Display, TEXT("Name_EN: %s"), *Name_EN.ToString());
	
	// Result
	UE_LOG(LogFBuildingRecipeData, Display, TEXT("\n---[Result]"));
	UE_LOG(LogFBuildingRecipeData, Display, TEXT("ResultItemID: %d"), ResultItemID);
	UE_LOG(LogFBuildingRecipeData, Display, TEXT("ResultCount: %d"), ResultCount);
	
	// Building Info
	UE_LOG(LogFBuildingRecipeData, Display, TEXT("\n---[Building Info]"));
	UE_LOG(LogFBuildingRecipeData, Display, TEXT("BuildTime: %.2f seconds"), BuildTime);
	UE_LOG(LogFBuildingRecipeData, Display, TEXT("Ingredients Count: %d"), Ingredients.Num());
	
	for (int32 i = 0; i < Ingredients.Num(); ++i)
	{
		const FBuildingIngredientData& Ingredient = Ingredients[i];
		UE_LOG(LogFBuildingRecipeData, Display, TEXT("  [%d] MaterialID: %d, Count: %d"), 
			i, Ingredient.MaterialID, Ingredient.Count);
	}
	
	// Helper Checks
	UE_LOG(LogFBuildingRecipeData, Display, TEXT("\n---[Helper Checks]"));
	UE_LOG(LogFBuildingRecipeData, Display, TEXT("HasValidIngredients: %s"), HasValidIngredients() ? TEXT("True") : TEXT("False"));
	UE_LOG(LogFBuildingRecipeData, Display, TEXT("GetIngredientTypeCount: %d"), GetIngredientTypeCount());
	UE_LOG(LogFBuildingRecipeData, Display, TEXT("IsValidRecipe: %s"), IsValidRecipe() ? TEXT("True") : TEXT("False"));
	UE_LOG(LogFBuildingRecipeData, Display, TEXT("ValidateRecipeData: %s"), ValidateRecipeData() ? TEXT("True") : TEXT("False"));
	
	UE_LOG(LogFBuildingRecipeData, Display, TEXT("======================================================"));
}