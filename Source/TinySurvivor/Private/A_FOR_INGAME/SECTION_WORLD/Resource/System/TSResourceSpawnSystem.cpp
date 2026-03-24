// All CopyRight From YulRyongGameStudio //


#include "A_FOR_INGAME/SECTION_WORLD/Resource/System/TSResourceSpawnSystem.h"
#include "A_FOR_INGAME/SECTION_WORLD/Resource/Actor/A_Base/TSResourceActorBase.h"
#include "A_FOR_INGAME/SECTION_WORLD/Resource/System/TSResourceDataSystem.h"
#include "A_FOR_INGAME/SECTION_WORLD/ResourceSpawn/Actor/ResourceNode/TSResourceNodeActor.h"

//======================================================================================================================	
#pragma region 게터
	
	
	//━━━━━━━━━━━━━━━━━━━━
	// 게터
	//━━━━━━━━━━━━━━━━━━━━	

UTSResourceSpawnSystem* UTSResourceSpawnSystem::Get(const UObject* InWorldContextObject)
{
	if (!IsValid(InWorldContextObject)) return nullptr;
	
	UWorld* World = InWorldContextObject->GetWorld();
	if (!IsValid(World)) return nullptr;
			
	UGameInstance* GameInstance = World->GetGameInstance();

	UTSResourceSpawnSystem* ResourceSpawnSystem = GameInstance->GetSubsystem<UTSResourceSpawnSystem>();
	if (!IsValid(ResourceSpawnSystem)) return nullptr;

	return ResourceSpawnSystem;
}

#pragma endregion
//======================================================================================================================
#pragma region 스폰_API
	
	//━━━━━━━━━━━━━━━━━━━━
	// 스폰 API
	//━━━━━━━━━━━━━━━━━━━━	
	
bool UTSResourceSpawnSystem::SpawnNewResource(int32 InResourceID, FVector InSpawnLocation, FRotator InSpawnRotator, ATSResourceNodeActor* InSpawnNode)
{
	UTSResourceDataSystem* ResourceDataSystem = UTSResourceDataSystem::Get(this);
	if (!IsValid(ResourceDataSystem)) return false;
	
	FTSResourceStaticData* ResourceStaticData = ResourceDataSystem->GetResourceStaticData(InResourceID);
	if (!ResourceStaticData) return false;
	
	return SpawnNewResource_Internal(*ResourceStaticData, ResourceStaticData->ResourceBaseInfoTable.ResourceClass, InSpawnLocation, InSpawnRotator, InSpawnNode);
}

bool UTSResourceSpawnSystem::SpawnNewResource_Internal(FTSResourceStaticData& InResourceStaticData, TSubclassOf<ATSResourceActorBase> ResourceClass, FVector& InSpawnLocation, FRotator& InSpawnRotator, ATSResourceNodeActor* InSpawnNode)
{
	if (!IsValid(ResourceClass)) return false;
	if (!IsValid(GetWorld())) return false;
	if (!IsValid(InSpawnNode)) return false;
	
	// 1. 스폰 파라미터 설정
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	// 2. 스폰 실시 
	ATSResourceActorBase* SpawnedResource = GetWorld()->SpawnActor<ATSResourceActorBase>(ResourceClass, InSpawnLocation, InSpawnRotator, SpawnParams);
	if (!IsValid(SpawnedResource)) return false;

	// 체력, 스폰 가능한 수량, 스태틱 ID 설정 
	FTSResourceRuntimeData NewResourceRuntimeData;
	NewResourceRuntimeData.DynamicData.CurrentAmount = InResourceStaticData.ResourceLootInfoTable.TotalCount;
	NewResourceRuntimeData.DynamicData.CurrentHealth = InResourceStaticData.ResourceLootInfoTable.Health;
	NewResourceRuntimeData.StaticDataID = InResourceStaticData.ResourceBaseInfoTable.ResourceID;

	// 런타임 데이터 설정 및 스폰 노드에 등록 
	SpawnedResource->SetResourceRuntimeData(NewResourceRuntimeData);
	InSpawnNode->RegisterResource(SpawnedResource);
	
	return true;
}

#pragma endregion
//======================================================================================================================	
