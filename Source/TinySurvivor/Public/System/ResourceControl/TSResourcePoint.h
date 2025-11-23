#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "Item/Data/ResourceData.h"
#include "TSResourcePoint.generated.h"

class ATSResourceBaseActor;

UCLASS()
class TINYSURVIVOR_API ATSResourcePoint : public AActor
{
	GENERATED_BODY()
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	

	//------------------------------
	// ATSResourcePoint 라이프 사이클 
	//------------------------------
	
public:
	ATSResourcePoint();
	virtual void BeginPlay() override;
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//------------------------------
	// ATSResourcePoint 자원 원천 포인트
	//------------------------------
	
public:
	FORCEINLINE FGameplayTag GetSectionTag() const { return SectionTag; };
	FORCEINLINE ENodeType GetResourceItemType() const { return ResourceItemType; };
	FORCEINLINE bool IsAllocated() const { return bIsAllocated; }
	FORCEINLINE ATSResourceBaseActor* GetAllocatedResource() const { return AllocatedResource.Get(); }
	void SetAllocatedResource(const TWeakObjectPtr<ATSResourceBaseActor> NewResource);
	FORCEINLINE void DeleteAllocatedResource() { AllocatedResource.Reset(); bIsAllocated = false; }
	
protected:
	// 이 포인트가 할당되어야 하는 섹터 태그 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ResourcePoint")
	FGameplayTag SectionTag;

	// 이 포인트에 들어올 수 있는 자원 원천 타입
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ResourcePoint")
	ENodeType ResourceItemType = ENodeType::JUNK;
	
	// 자원 원천이 할당되어 있는가?
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ResourcePoint")
	bool bIsAllocated = false;
	
	// 할당되어 있는 자원
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ResourcePoint")
	TWeakObjectPtr<ATSResourceBaseActor> AllocatedResource;
	
};
