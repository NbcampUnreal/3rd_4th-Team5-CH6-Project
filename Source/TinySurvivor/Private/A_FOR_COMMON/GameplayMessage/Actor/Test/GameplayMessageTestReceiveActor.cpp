// All CopyRight From YulRyongGameStudio //


#include "A_FOR_COMMON/GameplayMessage/Actor/Test/GameplayMessageTestReceiveActor.h"
#include "A_FOR_COMMON/GameplayMessage/Data/Struct/TSGameplayMessageData.h"
#include "GameFramework/GameplayMessageSubsystem.h"


//======================================================================================================================	
#pragma region 라이프_사이클
	
	//--------------------
	// 라이프 사이클
	//--------------------
	
AGameplayMessageTestReceiveActor::AGameplayMessageTestReceiveActor()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AGameplayMessageTestReceiveActor::BeginPlay()
{
	Super::BeginPlay();
	
	if (!IsValid(GetWorld())) return;
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AGameplayMessageTestReceiveActor::SendTestGameplayMessage, 5.0f, false);
	
}

void AGameplayMessageTestReceiveActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

#pragma endregion
//======================================================================================================================	
#pragma region 게임_플레이_메시지
	
	//--------------------
	// 게임 플레이 메시지
	//--------------------

void AGameplayMessageTestReceiveActor::SendTestGameplayMessage()
{
	// 서브시스템 가져오기
	if (!IsValid(GetWorld())) return;
	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(GetWorld());

	// 데이터 구성
	FTSGameplayMessageData MessagePayload;
	MessagePayload.MessageID = 99;

	// 발송
	MessageSubsystem.BroadcastMessage(ChannelTag, MessagePayload);
}

#pragma endregion 
//======================================================================================================================	
	