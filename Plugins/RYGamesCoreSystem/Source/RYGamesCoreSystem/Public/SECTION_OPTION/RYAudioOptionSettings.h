// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "Struct_RYSoundOption.h"
#include "Engine/DeveloperSettings.h"
#include "RYAudioOptionSettings.generated.h"

/**
 * 
 */
UCLASS(Config = Game, DefaultConfig, meta=(DisplayName="RY Sound Option Settings"))
class RYGAMESCORESYSTEM_API URYAudioOptionSettings : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:
	
	// 사운드 옵션 구조체 리스트 
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category="RY | Core | MainTitle | SubWidget", DisplayName="사운드 옵션 구조체 리스트")
	TArray<FRYSoundOption> SoundOptionList = {};
	
};
