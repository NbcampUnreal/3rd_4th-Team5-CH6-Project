// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "A_FOR_COMMON/GameplayMessage/Data/Struct/TSGameplayMessageData.h"
#include "A_FOR_COMMON/Tag/Test/GameplayMessageTestTag.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameplayMessageTestSendActor.generated.h"

UCLASS()
class TINYSURVIVOR_API AGameplayMessageTestSendActor : public AActor
{
	GENERATED_BODY()

//======================================================================================================================	
#pragma region 라이프_사이클
	
	//--------------------
	// 라이프 사이클
	//--------------------
	
public:
	AGameplayMessageTestSendActor();
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
#pragma endregion 
//======================================================================================================================	
#pragma region 게임_플레이_메시지
	
	//--------------------
	// 게임 플레이 메시지
	//--------------------

protected:
	// 메시지 구독 함수 
	void SubscribeToTestMessage();
	
	// 메시지 수신 함수 
	void OnTestMessageReceived(FGameplayTag InChannelTag, const FTSGameplayMessageData& OutPayload);
	
	// 메시지 구독 해제 함수 
	void UnsubscribeFromTestMessage();
	
	// 메시 소통 채널 태그 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameplayMessage")
	FGameplayTag ChannelTag = GameplayMessageTest::GAMEPLAYMESSAGETESTTAG;;
	
	// 메시지 시스템 핸들
	FGameplayMessageListenerHandle ListenerHandle;
	
#pragma endregion 
//======================================================================================================================	
	
};
