// LootComponent.h

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Item/Data/ItemData.h" // 기존 데이터 구조체 헤더
#include "LootComponent.generated.h"

// 개별 드랍 규칙 구조체
USTRUCT(BlueprintType)
struct FLootRule
{
	GENERATED_BODY()

	// 떨굴 아이템 ID
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ItemID = 0;

	// 드랍 확률 (0.0 ~ 1.0, 1.0이면 100%)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0.0", ClampMax="1.0"))
	float DropChance = 1.0f;

	// 최소/최대 개수
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MinCount = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxCount = 1;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TINYSURVIVOR_API ULootComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	ULootComponent();

	// 에디터에서 설정할 드랍 목록 (몬스터마다 다르게 설정 가능)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loot")
	TArray<FLootRule> LootTable;
	// 아이템이 흩뿌려질 반경
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loot")
	float ScatterRadius = 100.0f;

	// 드랍 테이블을 보고 확률적으로 떨구기 (죽을 때 사용)
	UFUNCTION(BlueprintCallable)
	bool SpawnLoot(FTransform& SpawnTransform, const FVector& PlayerLocation);
	// 특정 아이템을 지정된 개수만큼 즉시 떨구기 (채집 시 사용)
	UFUNCTION(BlueprintCallable)
	bool SpawnSpecificLoot(int32 ItemID, int32 Count);
	
private:
	FVector GetClosestPlayerLocation(const FVector& OriginOriginLocation) const;
};