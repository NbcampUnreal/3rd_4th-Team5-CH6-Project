// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "RYAudioOptionSettings.h"
#include "Struct_RYSoundOption.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "RYOptionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class RYGAMESCORESYSTEM_API URYOptionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	
	// 오디오 세팅 초기화 API
	UFUNCTION(BlueprintCallable, Category = "RY | Core | Option")
	static void InitializeAudioSettings(const UObject* Object);
	
	// 새 오디오 세팅 요청 API
	UFUNCTION(BlueprintCallable, Category = "RY | Core | Option")
	static void RequestSetNewAudioSettings(const UObject* Object, TArray<FRYSoundOption> NewSoundOptions);
	
	// 오디오 디폴트 세팅 API
	UFUNCTION(BlueprintCallable, Category = "RY | Core | Option")
	static TArray<FRYSoundOption> RequestDefaultAudioSettings();
	
	// 오디오 세팅 게터 API (TMap)
	UFUNCTION(BlueprintCallable, Category = "RY | Core | Option")
	static TMap<FName, FRYSoundOption> RequestGetSoundOptionMap();
	
	// 오디오 세팅 게터 API (Array)
	UFUNCTION(BlueprintCallable, Category = "RY | Core | Option")
	static TArray<FRYSoundOption> GetAudioSettings();
	
	// 최초 설정 확인용 API
	UFUNCTION(BlueprintCallable, Category = "RY | Core | Option")
	static bool IsFirstOptionSetting();
	
	// 스트리머 체크 API
	UFUNCTION(BlueprintCallable, Category = "RY | Core | Option")
	static void ShouldStreamerModeIsOn(bool WantModeOn);

	// 스트리머 모드 확인 API
	UFUNCTION(BlueprintCallable, Category = "RY | Core | Option")
	static bool IsStreamerModeIsOn();
	
};
