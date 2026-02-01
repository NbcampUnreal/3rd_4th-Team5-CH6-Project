// CraftingData.cpp
#include "Crafting/Data/CraftingData.h"

// 로그 카테고리 정의
DEFINE_LOG_CATEGORY_STATIC(LogFCraftingData, Log, All);

void FCraftingData::PrintDebugInfo() const
{
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	UE_LOG(LogFCraftingData, Display, TEXT("============= Crafting Recipe Debug Info ============="));
	
	// Identifier
	UE_LOG(LogFCraftingData, Display, TEXT("---[Identifier]"));
	UE_LOG(LogFCraftingData, Display, TEXT("RecipeID: %d"), RecipeID);
	UE_LOG(LogFCraftingData, Display, TEXT("Category: %s"), *UEnum::GetValueAsString(Category));
	UE_LOG(LogFCraftingData, Display, TEXT("ResultName_KR: %s"), *ResultName_KR.ToString());
	UE_LOG(LogFCraftingData, Display, TEXT("ResultName_EN: %s"), *ResultName_EN.ToString());
	
	// Result
	UE_LOG(LogFCraftingData, Display, TEXT("\n---[Result]"));
	UE_LOG(LogFCraftingData, Display, TEXT("ResultItemID: %d"), ResultItemID);
	UE_LOG(LogFCraftingData, Display, TEXT("ResultCount: %d"), ResultCount);
	
	// Crafting Info
	UE_LOG(LogFCraftingData, Display, TEXT("\n---[Crafting Info]"));
	UE_LOG(LogFCraftingData, Display, TEXT("CraftTime: %.2f seconds"), CraftTime);
	UE_LOG(LogFCraftingData, Display, TEXT("Ingredients Count: %d"), Ingredients.Num());
	
	for (int32 i = 0; i < Ingredients.Num(); ++i)
	{
		const FIngredientData& Ingredient = Ingredients[i];
		UE_LOG(LogFCraftingData, Display, TEXT("  [%d] MaterialID: %d, Count: %d"), 
			i, Ingredient.MaterialID, Ingredient.Count);
	}
	
	// Requirements
	//UE_LOG(LogFCraftingData, Display, TEXT("\n---[Requirements]"));
	// UE_LOG(LogFCraftingData, Display, TEXT("RequiredCraftingStation: %d %s"), 
	// 	RequiredCraftingStation, 
	// 	RequiredCraftingStation == 0 ? TEXT("(Hand Crafting)") : TEXT(""));
	// UE_LOG(LogFCraftingData, Display, TEXT("RequiredLevel: %d"), RequiredLevel);
	// UE_LOG(LogFCraftingData, Display, TEXT("Unlocked: %s"), bUnlocked ? TEXT("True") : TEXT("False"));
	
	// Helper Checks
	UE_LOG(LogFCraftingData, Display, TEXT("\n---[Helper Checks]"));
	UE_LOG(LogFCraftingData, Display, TEXT("HasValidIngredients: %s"), HasValidIngredients() ? TEXT("True") : TEXT("False"));
	//UE_LOG(LogFCraftingData, Display, TEXT("RequiresCraftingStation: %s"), RequiresCraftingStation() ? TEXT("True") : TEXT("False"));
	//UE_LOG(LogFCraftingData, Display, TEXT("IsLocked: %s"), IsLocked() ? TEXT("True") : TEXT("False"));
	UE_LOG(LogFCraftingData, Display, TEXT("GetIngredientTypeCount: %d"), GetIngredientTypeCount());
	UE_LOG(LogFCraftingData, Display, TEXT("CanCraft: %s"), IsValidRecipe() ? TEXT("True") : TEXT("False"));
	UE_LOG(LogFCraftingData, Display, TEXT("ValidateRecipeData: %s"), ValidateRecipeData() ? TEXT("True") : TEXT("False"));
	
	UE_LOG(LogFCraftingData, Display, TEXT("======================================================"));
#endif
}