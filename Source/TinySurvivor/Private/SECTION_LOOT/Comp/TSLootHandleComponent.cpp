// All CopyRight From YulRyongGameStudio //


#include "SECTION_LOOT/Comp/TSLootHandleComponent.h"
#include "A_FOR_INGAME/SECTION_ITEM/Item/Actor/Base/TSItemActorBase.h"
#include "A_FOR_INGAME/SECTION_ITEM/Item/System/TSITemSpawnSubSystem.h"
#include "A_FOR_INGAME/SECTION_WORLD/Resource/System/TSResourceDataSystem.h"


//======================================================================================================================	
#pragma region 라이프_사이클
	
	//━━━━━━━━━━━━━━━━━━━━
	// 라이프 사이클
	//━━━━━━━━━━━━━━━━━━━━	

UTSLootHandleComponent::UTSLootHandleComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

#pragma endregion
//======================================================================================================================	
#pragma region 루팅API
	
	//━━━━━━━━━━━━━━━━━━━━
	// 라이프 사이클
	//━━━━━━━━━━━━━━━━━━━━	

bool UTSLootHandleComponent::RequestSpawnLootFromResource(float& AttackDamage, const FGameplayTag& InteractType,FTSResourceRuntimeData& InResourceRuntimeData, FVector& InHitImpactPoint)
{
	// 서버에서만 실행
	if (!IsValid(GetOwner()) || !GetOwner()->HasAuthority()) return false;
	
	UTSResourceDataSystem* ResourceDataSubSystem = UTSResourceDataSystem::Get(this);
	if (!IsValid(ResourceDataSubSystem)) return false;
	
	// 자원 데이터 유효성 체크
	FTSResourceStaticData* ResourceStaticData = ResourceDataSubSystem->GetResourceStaticData(InResourceRuntimeData.StaticDataID);
	if (!ResourceStaticData) return false;
	
	// 상호작용 타입 체크 
	if (CheckCanInteractWithResource_internal(InteractType, ResourceStaticData->ResourceLootInfoTable.PossibleInteractTags) == false) return false;
	
	for (auto& DropInfo : ResourceStaticData->ResourceLootInfoTable.LootInfoArray)
	{
		// 드롭 사이즈 계산 
		int32 DropNum = CalculateLootDropRate_internal(DropInfo, InResourceRuntimeData);
		if (DropNum <= 0) continue;
		
		// 스폰 요청 
		bool bIsSpawnSuccess = SpawnLootFromResource_Internal(ResourceStaticData->ResourceBaseInfoTable.ResourceID, DropNum, InHitImpactPoint);
		if (bIsSpawnSuccess == false)
		{
			return false;
		}
		else if (bIsSpawnSuccess == true)
		{
			InResourceRuntimeData.DynamicData.CurrentAmount -= DropNum;
		}
	}
	
	return true;
}

bool UTSLootHandleComponent::CheckCanInteractWithResource_internal(const FGameplayTag& InInteractType,const TArray<FGameplayTag>& InPossibleInteractTags)
{
	for (auto& PossibleInteractTag : InPossibleInteractTags)
	{
		if (InInteractType.MatchesAnyExact(PossibleInteractTag.GetSingleTagContainer())) return true;
	}
	
	return false;
}

int32 UTSLootHandleComponent::CalculateLootDropRate_internal(const FTSLootInfo& InDropInfo, FTSResourceRuntimeData& InResourceRuntimeData)
{
	// 0. 만약 현재 남은 수량이 없다면 무조건 0 
	if (InResourceRuntimeData.DynamicData.CurrentAmount <= 0) return 0;
	
	// 1. 확률 판정 (0.0 ~ 1.0 사이의 난수 생성) -> 드랍 확률보다 작을 경우 소환 X  
	float RandomRoll = FMath::FRand();
	if (RandomRoll <= InDropInfo.LootProbability) return 0;
	
	// 2. 당첨되었다면 최소~최대 개수 사이에서 랜덤하게 선택
	int32 DropCount = FMath::RandRange(InDropInfo.MinDropCount, InDropInfo.MaxDropCount);
	return DropCount;
}

bool UTSLootHandleComponent::SpawnLootFromResource_Internal(const int32 InResourceID, const int32 InDropNum, const FVector& InHitImpactPoint)
{	
	UTSITemSpawnSubSystem* ItemSpawnSubSystem = UTSITemSpawnSubSystem::Get(this);
	if (!IsValid(ItemSpawnSubSystem)) return false;

	// 스폰 요청 
	ATSItemActorBase* SpawnedLootItem = ItemSpawnSubSystem->SpawnNewItem(InResourceID, InDropNum, InHitImpactPoint);
	if (!IsValid(SpawnedLootItem)) return false;
	
	return true;
}

#pragma endregion
//======================================================================================================================	