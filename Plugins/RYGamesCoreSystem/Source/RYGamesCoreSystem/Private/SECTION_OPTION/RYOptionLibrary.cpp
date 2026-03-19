// All CopyRight From YulRyongGameStudio //


#include "SECTION_OPTION/RYOptionLibrary.h"

#include "SECTION_OPTION/RYAudioOptionSettings.h"
#include "SECTION_OPTION/RYOptionSubsystem.h"
#include "SECTION_OPTION/RYOptionUserSettings.h"

void URYOptionLibrary::InitializeAudioSettings(const UObject* Object)
{
	// NOTE : 오브젝트 검사는 Get 에서 하고 있다.
	
	URYOptionSubsystem* OptionSubsystem = URYOptionSubsystem::Get(Object);
	if (!IsValid(OptionSubsystem))
	{
		// 로그
		return;
	}
	
	OptionSubsystem->InitializeAudioSettings();
}

void URYOptionLibrary::RequestSetNewAudioSettings(const UObject* Object, TArray<FRYSoundOption> NewSoundOptions)
{
	// NOTE : 오브젝트 검사는 Get 에서 하고 있다.
	
	URYOptionSubsystem* OptionSubsystem = URYOptionSubsystem::Get(Object);
	if (!IsValid(OptionSubsystem))
	{
		// 로그
		return;
	}
	
	OptionSubsystem->RequestSetNewAudioSettings(NewSoundOptions);
}

TArray<FRYSoundOption> URYOptionLibrary::RequestDefaultAudioSettings()
{
	const URYAudioOptionSettings* OptionSettings = GetDefault<URYAudioOptionSettings>();
	if (!IsValid(OptionSettings))
	{
		// 로그
		return {};
	}
	
	return OptionSettings->SoundOptionList;
	
}

TMap<FName, FRYSoundOption> URYOptionLibrary::RequestGetSoundOptionMap()
{
	TMap<FName, FRYSoundOption> SoundOptionMap;
	
	for (auto& AudioSetting : RequestDefaultAudioSettings())
	{
		SoundOptionMap.Add(AudioSetting.SoundOptionName, AudioSetting);
	}
	
	return SoundOptionMap;
}

TArray<FRYSoundOption> URYOptionLibrary::GetAudioSettings()
{
	
	const URYOptionUserSettings* OptionUserSettings = GetDefault<URYOptionUserSettings>();
	if (!IsValid(OptionUserSettings))
	{
		// 로그
		return {};
	}
	
	return OptionUserSettings->SoundOptionList;
}

bool URYOptionLibrary::IsFirstOptionSetting()
{
	const URYOptionUserSettings* OptionUserSettings = GetDefault<URYOptionUserSettings>();
	if (!IsValid(OptionUserSettings))
	{
		// 로그
		return false;
	}
	
	return  OptionUserSettings->IsFirstOptionSetting;
}

void URYOptionLibrary::ShouldStreamerModeIsOn(bool WantModeOn)
{
	if (!IsValid(GEngine))
	{
		// 로그
		return;
	}
	
	URYOptionUserSettings* UserSettings = Cast<URYOptionUserSettings>(GEngine->GetGameUserSettings());
	if (!IsValid(UserSettings))
	{
		// 로그
		return;
	}
	
	UserSettings->IsFirstOptionSetting = false;
	UserSettings->bShouldTurnOnStreamerMode = WantModeOn;
	UserSettings->SaveSettings();
}

bool URYOptionLibrary::IsStreamerModeIsOn()
{
	const URYOptionUserSettings* OptionUserSettings = GetDefault<URYOptionUserSettings>();
	if (!IsValid(OptionUserSettings))
	{
		// 로그
		return false;
	}
	
	return  OptionUserSettings->bShouldTurnOnStreamerMode;
}
