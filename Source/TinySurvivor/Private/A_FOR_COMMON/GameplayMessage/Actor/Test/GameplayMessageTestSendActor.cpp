// All CopyRight From YulRyongGameStudio //


#include "A_FOR_COMMON/GameplayMessage/Actor/Test/GameplayMessageTestSendActor.h"
#include "GameFramework/GameplayMessageSubsystem.h"

//======================================================================================================================	
#pragma region 라이프_사이클
	
	//--------------------
	// 라이프 사이클
	//--------------------

AGameplayMessageTestSendActor::AGameplayMessageTestSendActor()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AGameplayMessageTestSendActor::BeginPlay()
{
	Super::BeginPlay();
	
	SubscribeToTestMessage();
}

void AGameplayMessageTestSendActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnsubscribeFromTestMessage();
	
	Super::EndPlay(EndPlayReason);
}


#pragma endregion
//======================================================================================================================	
#pragma region 게임_플레이_메시지
	
	//--------------------
	// 게임 플레이 메시지
	//--------------------

void AGameplayMessageTestSendActor::SubscribeToTestMessage()
{
	// 게임 플레이 메시지 시스템 가져오기
	if (!IsValid(GetWorld())) return;
	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(GetWorld());
	
	// 구독 및 핸들 보관
	ListenerHandle = MessageSubsystem.RegisterListener<FTSGameplayMessageData>(ChannelTag, this, &AGameplayMessageTestSendActor::OnTestMessageReceived);
}

void AGameplayMessageTestSendActor::OnTestMessageReceived(FGameplayTag InChannelTag,const FTSGameplayMessageData& OutPayload)
{
	UE_LOG(LogTemp, Log, TEXT("멤버 함수 호출됨! : 메시지 ID : %d"), OutPayload.MessageID);
}

void AGameplayMessageTestSendActor::UnsubscribeFromTestMessage()
{
	// 핸들을 사용하여 구독 해지
	if (IsValid(GetWorld()) && ListenerHandle.IsValid())
	{
		UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(GetWorld());
		MessageSubsystem.UnregisterListener(ListenerHandle);
	}
}

#pragma endregion 
//======================================================================================================================	
	