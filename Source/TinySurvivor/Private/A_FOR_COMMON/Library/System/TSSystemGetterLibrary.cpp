// All CopyRight From YulRyongGameStudio //


#include "A_FOR_COMMON/Library/System/TSSystemGetterLibrary.h"


UItemDataSubsystem* UTSSystemGetterLibrary::GetItemDataSubsystem(const UObject* InWorldContextObject)
{
	if (!IsValid(InWorldContextObject)) return nullptr;

	UWorld* World = InWorldContextObject->GetWorld();
	if (!IsValid(World)) return nullptr;
	
	UGameInstance* GameInstance = World->GetGameInstance();
	if (!IsValid(GameInstance)) return nullptr;
	
	return GameInstance->GetSubsystem<UItemDataSubsystem>();
}

UWorldItemPoolSubsystem* UTSSystemGetterLibrary::GetWorldItemPoolSubsystem(const UObject* InWorldContextObject)
{
	if (!IsValid(InWorldContextObject)) return nullptr;

	UWorld* World = InWorldContextObject->GetWorld();
	if (!IsValid(World)) return nullptr;

	return World->GetSubsystem<UWorldItemPoolSubsystem>();
}

UItemDecaySubSystem* UTSSystemGetterLibrary::GetDecayManager(const UObject* InWorldContextObject)
{
	if (!IsValid(InWorldContextObject)) return nullptr;
	
	UWorld* World = InWorldContextObject->GetWorld();
	if (!IsValid(World)) return nullptr;
	
	return World->GetSubsystem<UItemDecaySubSystem>();
}

UGameplayTagDisplaySubsystem* UTSSystemGetterLibrary::GetGameplayTagDisplaySubsystem(const UObject* InWorldContextObject)
{
	if (!IsValid(InWorldContextObject)) return nullptr;

	UWorld* World = InWorldContextObject->GetWorld();
	if (!IsValid(World)) return nullptr;
	
	UGameInstance* GameInstance = World->GetGameInstance();
	if (!IsValid(GameInstance)) return nullptr;
	
	return GameInstance->GetSubsystem<UGameplayTagDisplaySubsystem>();
}
