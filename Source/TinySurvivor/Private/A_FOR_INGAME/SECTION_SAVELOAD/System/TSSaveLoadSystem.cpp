// All CopyRight From YulRyongGameStudio //


#include "A_FOR_INGAME/SECTION_SAVELOAD/System/TSSaveLoadSystem.h"
#include "A_FOR_INGAME/SECTION_SAVELOAD/TSSaveGame.h"
#include "Kismet/GameplayStatics.h"

//======================================================================================================================	
#pragma region 게터
	
	
	//━━━━━━━━━━━━━━━━━━━━
	// 게터
	//━━━━━━━━━━━━━━━━━━━━	

UTSSaveLoadSystem* UTSSaveLoadSystem::Get(const UObject* InWorldContextObject)
{
	if (!IsValid(InWorldContextObject)) return nullptr;
	
	UWorld* World = InWorldContextObject->GetWorld();
	if (!IsValid(World)) return nullptr;
			
	UGameInstance* GameInstance = World->GetGameInstance();
	if (!IsValid(GameInstance)) return nullptr;

	UTSSaveLoadSystem* SaveLoadSystem = GameInstance->GetSubsystem<UTSSaveLoadSystem>();
	if (!IsValid(SaveLoadSystem)) return nullptr;

	return SaveLoadSystem;
}

#pragma endregion
//======================================================================================================================	
#pragma region 라이프_사이클
	
	
	//━━━━━━━━━━━━━━━━━━━━
	// 라이프 사이클
	//━━━━━━━━━━━━━━━━━━━━	

void UTSSaveLoadSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UTSSaveLoadSystem::Deinitialize()
{
	Super::Deinitialize();
}

#pragma endregion
//======================================================================================================================	
#pragma region 세이브로드_API
	
	//━━━━━━━━━━━━━━━━━━━━
	// 세이브로드 API
	//━━━━━━━━━━━━━━━━━━━━

bool UTSSaveLoadSystem::RequestLoadSaveData(int32 InSaveID)
{
	FString SaveGameName = FString::FromInt(InSaveID);
	if (!UGameplayStatics::DoesSaveGameExist(SaveGameName, 0)) return false;
	
	UTSSaveGame* SaveGameInstance = CastChecked<UTSSaveGame>(UGameplayStatics::CreateSaveGameObject(UTSSaveGame::StaticClass()));
	if (!IsValid(SaveGameInstance)) return false;

	CurrentLoadedData = SaveGameInstance->MasterData;
	LastRequestedLoadSaveID = InSaveID;
	return true;
}

bool UTSSaveLoadSystem::RequestSaveData(int32 InSaveID)
{
	UTSSaveGame* SaveGameInstance = CastChecked<UTSSaveGame>(UGameplayStatics::CreateSaveGameObject(UTSSaveGame::StaticClass()));
	if (!IsValid(SaveGameInstance)) return false;

	SaveGameInstance->MasterData = UesThisForSaveData;
	SaveGameInstance->IsThisMasterDataIsDontHavAnySavedData = false;
	
	FString SaveGameName = FString::FromInt(InSaveID);
	return UGameplayStatics::SaveGameToSlot(SaveGameInstance, SaveGameName, 0);
}

#pragma endregion
//======================================================================================================================	
	