// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "A_FOR_COMMON/Tag/Interact/TSInteractChannelTag.h"
#include "A_FOR_COMMON/Tag/Invnetory/TSHotKeyChannelTag.h"
#include "TSGameplayChannelSetting.generated.h"

/**
 * 
 */
UCLASS(Config = Game, DefaultConfig, meta=(DisplayName="TS 게임 플레이 메시 채널 설정 섹션"))
class TINYSURVIVOR_API UTSGameplayChannelSetting : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:
	
	//---------------------
	// 플레이어
	//---------------------
	
	// 상호작용 소통 채널 
	UPROPERTY(Config, EditAnywhere, Category = "Player | PlayerinteractReceiveChannel")
	FGameplayTag RequestCurrentWatchingActorChannelTag = InteractChannelTag::TS_Channel_Interact_RequestCurrentActor;
	UPROPERTY(Config, EditAnywhere, Category = "Player | PlayerinteractReceiveChannel")
	FGameplayTag SendCurrentWatchingActorChannelTag = InteractChannelTag::TS_Channel_Interact_SendCurrentActor;

	// 활성화 핫키 소통 채널 
	UPROPERTY(Config, EditAnywhere, Category = "Player | PlayerinteractReceiveChannel")
	FGameplayTag RequestCurrentActiveHotkeyChannelTag = HotKeyChannelTag::TS_Channel_Inventory_RequestCurrentActiveHotKey;
	UPROPERTY(Config, EditAnywhere, Category = "Player | PlayerinteractReceiveChannel")
	FGameplayTag SendCurrentActiveHotKeyChannelTag = HotKeyChannelTag::TS_Channel_Inventory_SendCurrentActiveHotKey;
	
};
