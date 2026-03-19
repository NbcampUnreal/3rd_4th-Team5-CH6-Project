// All CopyRight From YulRyongGameStudio //


#include "SECTION_OPTION/RYOptionSubsystem.h"
#include "SECTION_OPTION/RYAudioOptionSettings.h"
#include "SECTION_OPTION/RYOptionUserSettings.h"
#include "SECTION_CYCLE/RYGameCycleSubSystem.h"
#include "SECTION_UTIL/RYCoreLogMessageSystem.h"
#include "SECTION_UTIL/Marco/RYCoreLogMarco.h"
#include "Kismet/GameplayStatics.h"
#include "RYLog.h"

//======================================================================================================================
#pragma region GetSystem
	//--------------------
	// Get System
	//--------------------	

URYOptionSubsystem* URYOptionSubsystem::Get(const UObject* Object)
{
	if (IsValid(Object) == false)
	{
		// 로그
		OPTIONSYSTEM_LOG(TEXT("Get : Object fail"));
		return nullptr;
	}
	
	// 월드 컨텍스트로부터 월드를 가져옵니다.
	const UWorld* World = Object->GetWorld();
	if (IsValid(World) == false)
	{
		// 로그
		OPTIONSYSTEM_WARN_RECORD(World, TEXT("Get : GameInstance fail"));
		return nullptr;
	}
	
	// 월드로부터 게임 인스턴스를 가져옵니다.
	const UGameInstance* GameInstance = World->GetGameInstance();
	if (IsValid(GameInstance) == false)
	{
		// 로그
		OPTIONSYSTEM_WARN_RECORD(World, TEXT("Get : GameInstance fail"));
		return nullptr;
	}
	
	// 게임 인스턴스로부터 게임 인스턴스 서브 시스템을 가져옵니다.
	URYOptionSubsystem* OptionSubsystem = GameInstance->GetSubsystem<URYOptionSubsystem>();
	if (IsValid(OptionSubsystem) == false)
	{
		OPTIONSYSTEM_WARN_RECORD(GameInstance, TEXT("Get : OptionSubsystem fail"));
		return nullptr;
	}
	
	OPTIONSYSTEM_LOG_RECORD(OptionSubsystem, TEXT("Get : Success"));
	return OptionSubsystem;
}

#pragma endregion
//======================================================================================================================
#pragma region LifeCycle
	//--------------------
	// Life Cycle
	//--------------------

bool URYOptionSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	return true;
}

void URYOptionSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// 게임 사이클 시스템 초기화 이후 생성
	Collection.InitializeDependency(URYGameCycleSubSystem::StaticClass());
	
	OPTIONSYSTEM_LOG(TEXT("Initialize : Success"));
}

void URYOptionSubsystem::Deinitialize()
{
	OPTIONSYSTEM_LOG(TEXT("Deinitialize : Success"));
	
	Super::Deinitialize();
}

#pragma endregion
//======================================================================================================================
#pragma region Option
	//--------------------
	// Option
	//--------------------

void URYOptionSubsystem::InitializeAudioSettings()
{
	if (IsValid(GEngine) == false)
	{
		OPTIONSYSTEM_WARN_RECORD(this, TEXT("InitializeAudioSettings : GetEngine fail"));
		return;
	}
	
	if (IsValid(GEngine->GetGameUserSettings()) == false)
	{
		OPTIONSYSTEM_WARN_RECORD(this, TEXT("InitializeAudioSettings : GetGameUserSettings fail"));
		return;
	}
	
	URYOptionUserSettings* UserSettings = Cast<URYOptionUserSettings>(GEngine->GetGameUserSettings());
	if (IsValid(UserSettings) == false)
	{
		OPTIONSYSTEM_WARN_RECORD(this, TEXT("InitializeAudioSettings : Cast fail"));
		return;
	}
	
	for (auto& [SoundOptionName, SoundVolumeSize, SoundMixClass, SoundClass] : UserSettings->SoundOptionList)
	{
		if (SoundMixClass.IsNull())
		{
			OPTIONSYSTEM_LOG_RECORD(this, TEXT("InitializeAudioSettings : SoundMixClass.IsNull"))
			continue;
		}
		
		if (SoundClass.IsNull())
		{
			OPTIONSYSTEM_LOG_RECORD(this, TEXT("InitializeAudioSettings : SoundClass.IsNull"))
			continue;
		}
		
		USoundMix* LoadedSoundMix = SoundMixClass.LoadSynchronous();
		if (IsValid(LoadedSoundMix) == false)
		{
			OPTIONSYSTEM_LOG_RECORD(this, TEXT("InitializeAudioSettings : IsValid(LoadedSoundMix) == false"))
			continue;
		}
		
		USoundClass* LoadedSoundClass = SoundClass.LoadSynchronous();
		if (IsValid(LoadedSoundClass) == false)
		{
			OPTIONSYSTEM_LOG_RECORD(this, TEXT("InitializeAudioSettings : IsValid(LoadedSoundClass) == false"))
			continue;
		}
		
		UGameplayStatics::SetSoundMixClassOverride(GetWorld(), LoadedSoundMix, LoadedSoundClass, SoundVolumeSize);
		UGameplayStatics::PushSoundMixModifier(GetWorld(), LoadedSoundMix);
	}
	
	OPTIONSYSTEM_LOG_RECORD(this, TEXT("InitializeAudioSettings : Success"));
}

void URYOptionSubsystem::RequestSetNewAudioSettings(const TArray<FRYSoundOption>& NewSoundOptions)
{
	URYAudioOptionSettings* AudioOptionSettings = GetMutableDefault<URYAudioOptionSettings>();
	if (IsValid(AudioOptionSettings) == false)
	{
		OPTIONSYSTEM_LOG_RECORD(this, TEXT("RequestSetNewAudioSettings : IsValid(AudioOptionSettings) == false"))
		return;
	}
	
	for (auto& [SoundOptionName, SoundVolumeSize, SoundMixClass, SoundClass] : AudioOptionSettings->SoundOptionList)
	{
		if (SoundMixClass.IsNull())
		{
			OPTIONSYSTEM_LOG_RECORD(this, TEXT("RequestSetNewAudioSettings : SoundMixClass.IsNull"))
			continue;
		}
		
		if (SoundClass.IsNull())
		{
			OPTIONSYSTEM_LOG_RECORD(this, TEXT("RequestSetNewAudioSettings : SoundClass.IsNull"))
			continue;
		}
		
		const FRYSoundOption* FoundSoundOption = NewSoundOptions.FindByPredicate([&](const FRYSoundOption& Option){return Option.SoundOptionName == SoundOptionName;});
		if (FoundSoundOption == nullptr)
		{
			OPTIONSYSTEM_LOG_RECORD(this, TEXT("RequestSetNewAudioSettings : FoundSoundOption == nullptr"))
			continue;
		}
		
		USoundMix* LoadedSoundMix = SoundMixClass.LoadSynchronous();
		if (IsValid(LoadedSoundMix) == false)
		{
			OPTIONSYSTEM_LOG_RECORD(this, TEXT("RequestSetNewAudioSettings : IsValid(LoadedSoundMix) == false"))
			continue;
		}
		
		USoundClass* LoadedSoundClass = SoundClass.LoadSynchronous();
		if (IsValid(LoadedSoundClass) == false)
		{
			OPTIONSYSTEM_LOG_RECORD(this, TEXT("RequestSetNewAudioSettings : IsValid(LoadedSoundClass) == false"))
			continue;
		}
		
		UGameplayStatics::SetSoundMixClassOverride(GetWorld(), LoadedSoundMix, LoadedSoundClass, FoundSoundOption->SoundVolumeSize);
		UGameplayStatics::PushSoundMixModifier(GetWorld(), LoadedSoundMix);
	}

	if (IsValid(GEngine) == false)
	{
		// 로그
		OPTIONSYSTEM_WARN_RECORD(this, TEXT("RequestSetNewAudioSettings : GetEngine fail"));
		return;
	}
	
	if (IsValid(GEngine->GetGameUserSettings()) == false)
	{
		OPTIONSYSTEM_WARN_RECORD(this, TEXT("RequestSetNewAudioSettings : GetGameUserSettings fail"));
		return;
	}
	
	URYOptionUserSettings* UserSettings = Cast<URYOptionUserSettings>(GEngine->GetGameUserSettings());
	if (IsValid(UserSettings) == false)
	{
		OPTIONSYSTEM_WARN_RECORD(this, TEXT("RequestSetNewAudioSettings : Cast fail"));
		return;
	}
	
	UserSettings->SoundOptionList = NewSoundOptions;
	UserSettings->SaveSettings();
	
}

#pragma endregion
//======================================================================================================================