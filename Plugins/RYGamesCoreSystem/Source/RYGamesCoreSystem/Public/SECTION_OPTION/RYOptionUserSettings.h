// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "Struct_RYSoundOption.h"
#include "GameFramework/GameUserSettings.h"
#include "RYOptionUserSettings.generated.h"

/**
 * 
 */
UCLASS()
class RYGAMESCORESYSTEM_API URYOptionUserSettings : public UGameUserSettings
{
	GENERATED_BODY()
	
public:
	
	// 사운드 옵션들
	UPROPERTY(Config, VisibleAnywhere, BlueprintReadOnly)
	TArray<FRYSoundOption> SoundOptionList = {};

	// 최초 확인용 옵션
	UPROPERTY(Config, VisibleAnywhere, BlueprintReadOnly)
	bool IsFirstOptionSetting = true;
	
	// 스트리머용 체크 옵션 
	UPROPERTY(Config, VisibleAnywhere, BlueprintReadOnly)
	bool bShouldTurnOnStreamerMode = false;
	
	
};
