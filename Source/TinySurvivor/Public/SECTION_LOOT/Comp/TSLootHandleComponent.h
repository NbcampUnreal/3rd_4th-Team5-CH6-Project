// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "A_FOR_INGAME/SECTION_WORLD/Resource/Data/Struct/TSResourceRuntimeData.h"
#include "SECTION_LOOT/TSLootInfo.h"
#include "TSLootHandleComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TINYSURVIVOR_API UTSLootHandleComponent : public UActorComponent
{
	GENERATED_BODY()

//======================================================================================================================	
#pragma region 라이프_사이클
	
	//━━━━━━━━━━━━━━━━━━━━
	// 라이프 사이클
	//━━━━━━━━━━━━━━━━━━━━	
	
public:
	UTSLootHandleComponent();
	
#pragma endregion
//======================================================================================================================	
#pragma region 루팅API
	
	//━━━━━━━━━━━━━━━━━━━━
	// 루팅API
	//━━━━━━━━━━━━━━━━━━━━	
	
public:
	bool RequestSpawnLootFromResource(float& AttackDamage, const FGameplayTag& InteractType, FTSResourceRuntimeData& InResourceRuntimeData, FVector& InHitImpactPoint);
	
protected:
	bool CheckCanInteractWithResource_internal(const FGameplayTag& InInteractType, const TArray<FGameplayTag>& InPossibleInteractTags);
	int32 CalculateLootDropRate_internal(const FTSLootInfo& InDropInfo, FTSResourceRuntimeData& InResourceRuntimeData);
	bool SpawnLootFromResource_Internal(const int32 InResourceID, const int32 InDropNum, const FVector& InHitImpactPoint);
	
#pragma endregion
//======================================================================================================================	
};
