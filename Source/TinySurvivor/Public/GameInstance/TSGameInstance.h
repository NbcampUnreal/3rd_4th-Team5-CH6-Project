// TSGameInstance.h
#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "TSGameInstance.generated.h"

// 전방선언
class UTSAbilityDataAsset;

/*
	게임 인스턴스
	서버와 클라이언트 모두에서 존재하며, 레벨 전환 시에도 유지됩니다.
*/
UCLASS()
class TINYSURVIVOR_API UTSGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UTSGameInstance();
	
	//========================================
	// Lifecycle
	//========================================
	virtual void Init() override;
	virtual void Shutdown() override;
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ability")
	TObjectPtr<UTSAbilityDataAsset> AbilityAsset;
	
};
