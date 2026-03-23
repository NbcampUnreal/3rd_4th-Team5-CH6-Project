// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "A_FOR_INGAME/SECTION_WORLD/Resource/Actor/A_Base/TSResourceActorBase.h"
#include "TSSkeletalMeshResource.generated.h"

UCLASS()
class TINYSURVIVOR_API ATSSkeletalMeshResource : public ATSResourceActorBase
{
	GENERATED_BODY()

//======================================================================================================================	
#pragma region 라이프_사이클
	
	
	//━━━━━━━━━━━━━━━━━━━━
	// 라이프 사이클
	//━━━━━━━━━━━━━━━━━━━━	
public:
	ATSSkeletalMeshResource();
	virtual void BeginPlay() override;
	
#pragma endregion
//======================================================================================================================	
#pragma region 컴포넌트
	
	
	//━━━━━━━━━━━━━━━━━━━━
	// 컴포넌트 
	//━━━━━━━━━━━━━━━━━━━━	
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TS | Resource")
	TObjectPtr<USkeletalMeshComponent> ResourceSkeletalMeshComp = nullptr;
	
	
#pragma endregion
//======================================================================================================================	
	
};
