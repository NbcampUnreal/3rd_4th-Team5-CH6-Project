// TSAISpawner.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TSAISpawner.generated.h"

class ATSAICharacter;

UCLASS()
class TINYSURVIVOR_API ATSAISpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	ATSAISpawner();

protected:
	virtual void BeginPlay() override;

public:	
	// 스폰할 몬스터 클래스
	UPROPERTY(EditAnywhere, Category = "Spawner")
	TSubclassOf<ATSAICharacter> MonsterClass;
	//배회 반경 (이 스포너를 중심으로 값만큼 돌아다님)
	UPROPERTY(EditAnywhere, Category = "Spawner")
	float PatrolRadius = 1500.0f;
	// 리스폰 시간 (0이면 리스폰 안함)
	UPROPERTY(EditAnywhere, Category = "Spawner")
	float RespawnTime = 10.0f;
	
	UFUNCTION()
	void OnMonsterDestroyed(AActor* DestroyedActor);
	
	// 스폰 실행 함수
	void SpawnMonster();
};
