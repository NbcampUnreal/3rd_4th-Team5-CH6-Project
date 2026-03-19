// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "A_FOR_COMMON/Tag/Test/GameplayMessageTestTag.h"
#include "GameplayMessageTestReceiveActor.generated.h"

UCLASS()
class TINYSURVIVOR_API AGameplayMessageTestReceiveActor : public AActor
{
	GENERATED_BODY()

//======================================================================================================================	
#pragma region 라이프_사이클
	
	//--------------------
	// 라이프 사이클
	//--------------------
	
public:
	AGameplayMessageTestReceiveActor();
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

#pragma endregion 
//======================================================================================================================	
#pragma region 게임_플레이_메시지
	
	//--------------------
	// 게임 플레이 메시지
	//--------------------

protected:
	
	// 게임 플레이 메시지 발신 함수
	void SendTestGameplayMessage();
	
	// 메시 소통 채널 태그 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameplayMessage")
	FGameplayTag ChannelTag = GameplayMessageTest::GAMEPLAYMESSAGETESTTAG;;
	
#pragma endregion 
//======================================================================================================================	
		
};
