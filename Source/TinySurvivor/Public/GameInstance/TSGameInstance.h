// TSGameInstance.h
#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "TSGameInstance.generated.h"

class UTSAbilityDataAsset;
// 전방선언
class UItemDataSubsystem;
class UItemTableAsset;
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
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ability")
	TObjectPtr<UTSAbilityDataAsset> AbilityAsset;
public:
	UTSGameInstance();

	//========================================
	// Lifecycle
	//========================================
	virtual void Init() override;
	virtual void Shutdown() override;

protected:
	//========================================
	// Item System Configuration
	//========================================
	
	/*
		아이템 시스템 초기화에 사용할 데이터 에셋
		Project Settings 또는 GameInstance BP에서 설정
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="ItemSystem",
		meta=(DisplayName="Item Table Asset"))
	TObjectPtr<UItemTableAsset> ItemTableAsset;

private:
	/*
		아이템 데이터 시스템 초기화
		@return 초기화 성공 여부
	*/
	bool InitializeItemDataSystem();

#if WITH_EDITOR
	/*
		에디터 전용: 아이템 시스템 테스트
		@param ItemSystem 테스트할 서브시스템
	*/
	void TestItemDataSystem(UItemDataSubsystem* ItemSystem);
#endif
};
