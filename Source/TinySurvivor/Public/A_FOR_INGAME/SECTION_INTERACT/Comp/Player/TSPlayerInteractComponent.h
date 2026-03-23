// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "A_FOR_COMMON/Tag/Interact/TSInteractChannelTag.h"
#include "A_FOR_COMMON/GameplayMessage/Data/Struct/Interact/FTSInteractMessageData.h"
#include "TSPlayerInteractComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TINYSURVIVOR_API UTSPlayerInteractComponent : public UActorComponent
{
	GENERATED_BODY()

//======================================================================================================================	
#pragma region 라이프_사이클
	
	
	//━━━━━━━━━━━━━━━━━━━━
	// 라이프 사이클
	//━━━━━━━━━━━━━━━━━━━━	
	
public:
	UTSPlayerInteractComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;	
	
	
#pragma endregion 
//======================================================================================================================	
#pragma region 인터렉트_내부_동작_API
	
	//━━━━━━━━━━━━━━━━━━━━
	// 인터렉트 내부 동작 API
	//━━━━━━━━━━━━━━━━━━━━
	
protected:

	//--------------------
	// 상호작용 
	//--------------------
	
	// 인터렉트 tick 함수
	void TickInteract_internal();
	
	// 오너 플레이어 컨트롤러 캐싱 확인 함수 
	bool HasCachingOwnerPlayerController_internal();
	
	// 라인 트레이스 실시 함수 
	AActor* LineTrace_internal();
	
	// 라인 트레이스 디버깅 함수 
	void DrawInteractionDebugLine_internal(const FVector& Start, const FVector& End, const FHitResult& HitResult, bool bHit);
	
	// 후 처리 함수
	void DoInteractUILogicAfterLineTrace();

	//--------------------
	// 게임 플레이 메시지 
	//--------------------
	
	// 게임 플레이 메시지 구독 함수
	void SubscribeInteract_Internal();

	// 수신 시 동작 함수 
	void OnRequestCurrentActorChannelGameplayMessageReceived(FGameplayTag InChannelTag, const FTSInteractMessageData& OutPayload);
	
	// 게임 플레이 메시지 구독 해제 함수
	void UnsubscribeInteract_Internal();
	
	
#pragma endregion
//======================================================================================================================	
#pragma region 인터렉트_데이터
	
	//━━━━━━━━━━━━━━━━━━━━
	// 라이프 사이클
	//━━━━━━━━━━━━━━━━━━━━	
	
protected:

	//--------------------
	// 상호작용 
	//--------------------

	// 상호작용 가능한 거리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interact")
	float InteractDistance = 500.0f;
	
	// 캐싱한 플레이어 컨트롤러 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TS | Interact")
	TWeakObjectPtr<APlayerController> OwnerPlayerController = nullptr;
	
	// 현재 보는 액터
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TS | Interact")
	TWeakObjectPtr<AActor> CurrentInteractActor = nullptr;

	// 마지막으로 본 액터
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TS | Interact")
	TWeakObjectPtr<AActor> LastInteractActor = nullptr;

	
	//--------------------
	// 게임 플레이 메시지 
	//--------------------
	
	// 게임 플레이 메시지 받는 채널 (current actor 발신 요청 채널) 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameplayMessage")
	FGameplayTag RequestCurrentActorChannelTag = InteractChannelTag::TS_Channel_Interact_RequestCurrentActor;
	
	// 게임 플레이 보내는 채널 (current actor 발신 채널)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameplayMessage")
	FGameplayTag SendCurrentActorChannelTag = InteractChannelTag::TS_Channel_Interact_SendCurrentActor;
	
	// 게임 플레이 메시지 구독 핸들 
	FGameplayMessageListenerHandle ListenerHandle;

#pragma endregion 
//======================================================================================================================	

};
