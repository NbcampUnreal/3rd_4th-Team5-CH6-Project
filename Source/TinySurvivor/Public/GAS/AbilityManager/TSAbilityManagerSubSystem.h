#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "TSAbilityManagerSubSystem.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(AbilityManager, Log, All);

struct FGameplayTag;
class UAbilitySystemComponent;
class UDataTable;

UCLASS()
class TINYSURVIVOR_API UTSAbilityManagerSubSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	void GiveAbilityByTag(UAbilitySystemComponent* ASC, FGameplayTag DesiredTag, FName TableKey = "Default", int32 AbilityLevel = 1, int32 InputID = -1) const;
	void ActivateAbilityByTag(UAbilitySystemComponent* ASC, FGameplayTag DesiredTag, FName TableKey = "Default", int32 AbilityLevel = 1, int32 InputID = -1) const;
	void ClearAbilityByTag(UAbilitySystemComponent* ASC, FGameplayTag DesiredTag, FName TableKey = "Default", int32 AbilityLevel = 1, int32 InputID = -1) const;


	static UTSAbilityManagerSubSystem* GetAbilityManager(const UObject* WorldContextObject)
	{
		if (!WorldContextObject) return nullptr;

		const UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(WorldContextObject);
		if (!GameInstance) return nullptr;

		return GameInstance->GetSubsystem<UTSAbilityManagerSubSystem>();
	}

protected:
	UPROPERTY()
	TMap<FName, UDataTable*> AbilityTables;
};
