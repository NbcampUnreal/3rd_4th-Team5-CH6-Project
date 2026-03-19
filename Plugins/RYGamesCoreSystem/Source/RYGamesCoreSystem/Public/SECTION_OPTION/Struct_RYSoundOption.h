// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "Sound/SoundMix.h"
#include "Sound/SoundClass.h"
#include "Struct_RYSoundOption.generated.h"

USTRUCT(BlueprintType)
struct FRYSoundOption
{
	GENERATED_BODY()
	
	// 사운드 옵션 이름
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category="RY | Core | Option | SoundOption", DisplayName="사운드 옵션 이름")
	FName SoundOptionName = NAME_None;
	
	// 볼륨 크기
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category="RY | Core | Option | SoundOption", DisplayName="볼륨 크기")
	float SoundVolumeSize = 1.f;
	
	// 사운드 옵션 믹스
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category="RY | Core | Option | SoundOption", DisplayName="사운드 옵션 믹스")
	TSoftObjectPtr<USoundMix> SoundMixClass = nullptr;

	// 사운드 옵션 클래스
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category="RY | Core | Option | SoundOption", DisplayName="사운드 옵션 클래스")
	TSoftObjectPtr<USoundClass> SoundClass = nullptr;
	
};
