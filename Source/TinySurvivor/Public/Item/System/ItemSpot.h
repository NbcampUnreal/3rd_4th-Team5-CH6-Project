// ItemSpot.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemSpot.generated.h"

UCLASS()
class AItemSpot : public AActor
{
	GENERATED_BODY()
	
public:	
	AItemSpot();

protected:
	virtual void BeginPlay() override;

public:
	// 에디터에서 어떤 아이템인지 설정
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 ItemID = 1;
    
	// 에디터 시각 확인용 메시
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* MeshComp;
};
