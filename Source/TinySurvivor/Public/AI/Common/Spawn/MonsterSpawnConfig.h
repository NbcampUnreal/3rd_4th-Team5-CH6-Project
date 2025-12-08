// MonsterSpawnConfig.h

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AI/Common/TSAICharacter.h"
#include "MonsterSpawnConfig.generated.h"

// 몬스터 리스트 래퍼 구조체
USTRUCT(BlueprintType)
struct FMonsterSpawnGroup
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, Category = "Spawn")
	TArray<TSubclassOf<ATSAICharacter>> MonsterClasses;
};

// 어떤 몬스터를 스폰할 지 정의하는 데이터 에셋
UCLASS()
class TINYSURVIVOR_API UMonsterSpawnConfig : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	// 지역 키(FName)에 따른 몬스터 그룹 매핑
	UPROPERTY(EditDefaultsOnly, Category = "Spawn Setting")
	TMap<FName, FMonsterSpawnGroup> RegionSpawnTable;
	
	// 지역 키에 맞는 랜덤 몬스터 클래스 반환
	TSubclassOf<ATSAICharacter> GetRandomMonsterClass(FName RegionKey) const
	{
		// 해당 지역 키로 검색
		if (const FMonsterSpawnGroup* Group = RegionSpawnTable.Find(RegionKey))
		{
			if (Group->MonsterClasses.Num() > 0)
			{
				int32 RandIdx = FMath::RandRange(0, Group->MonsterClasses.Num() - 1);
				return Group->MonsterClasses[RandIdx];
			}
		}
		
		// 없으면 Default 키로 재검색 (안전장치)
		if (RegionKey != FName("Default"))
		{
			return GetRandomMonsterClass(FName("Default"));
		}
		
		return nullptr;
	}
};
