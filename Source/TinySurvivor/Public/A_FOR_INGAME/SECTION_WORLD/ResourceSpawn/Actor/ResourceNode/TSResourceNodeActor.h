// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "A_FOR_INGAME/SECTION_WORLD/ResourceSpawn/Data/Struct/ResourceNode/TSResourceNodeRuntimeData.h"
#include "GameFramework/Actor.h"
#include "TSResourceNodeActor.generated.h"

class UBoxComponent;

UCLASS()
class TINYSURVIVOR_API ATSResourceNodeActor : public AActor
{
	GENERATED_BODY()

//======================================================================================================================	
#pragma region 라이프_사이클
	
	//━━━━━━━━━━━━━━━━━━━━
	// 라이프 사이클
	//━━━━━━━━━━━━━━━━━━━━
public:
	ATSResourceNodeActor();
	virtual void BeginPlay() override;
	
#pragma endregion
//======================================================================================================================	
#pragma region 컴포넌트 
	
	//━━━━━━━━━━━━━━━━━━━━
	// 라이프 사이클
	//━━━━━━━━━━━━━━━━━━━━
public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ResourceNode")
	TObjectPtr<USceneComponent> NodeRootComponent = nullptr;
	
	// 트랜스폼 줄 때 바닥 경계면 맞기 하기 위해 존재함. (레벨 배치시에 이거가 바닥 안 파고들면 제대로 된 배치인 것.)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ResourceNode")
	TObjectPtr<UBoxComponent> FloorCheckBoxComponent = nullptr;
	
#pragma endregion
//======================================================================================================================	
#pragma region 데이터
	
	//━━━━━━━━━━━━━━━━━━━━
	// 데이터
	//━━━━━━━━━━━━━━━━━━━━

public:
	
	FORCEINLINE FTSResourceNodeRunTimeData& GetNodeData() { return NodeData;};
	FORCEINLINE void SetNodeData(FTSResourceNodeRunTimeData& InNodeData) { NodeData = InNodeData;};
	FORCEINLINE void RegisterResource(ATSResourceActorBase* InResource) { NodeData.CurrentSpawnedResourcePtr = InResource; NodeData.DynamicData.bIsSpawnedAnyResource = true; }
	FORCEINLINE void UnRegisterResource() { NodeData.CurrentSpawnedResourcePtr = nullptr; NodeData.DynamicData.bIsSpawnedAnyResource = false; }	
protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TS | ResourceSpawn")
	FTSResourceNodeRunTimeData NodeData;
	
#pragma endregion
//======================================================================================================================	
};
