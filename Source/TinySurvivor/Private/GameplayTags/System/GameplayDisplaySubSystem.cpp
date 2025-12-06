// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayTags/System/GameplayDisplaySubSystem.h"
#include "Engine/DataTable.h"
#include "GameplayTags/System/GameplayTagDisplaySettings.h"

void UGameplayTagDisplaySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// 데이터 테이블 로드
	const UGameplayTagDisplaySettings* Settings = GetDefault<UGameplayTagDisplaySettings>();
	if (!Settings || Settings->GameplayTagDisplayDataTable.IsNull())
	{
		UE_LOG(LogTemp, Warning, TEXT("GameplayTagDisplaySubsystem: No GameplayTagDisplayTable Path"));
	}
	GameplayTagDisplayTable = Settings->GameplayTagDisplayDataTable.LoadSynchronous();

	if (GameplayTagDisplayTable)
	{
		LoadDataTable(GameplayTagDisplayTable);
		UE_LOG(LogTemp, Log, TEXT("GameplayTagDisplaySubsystem: DataTable loaded successfully"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("GameplayTagDisplaySubsystem: Failed to load GameplayTagDisplayTable"));
	}
}

void UGameplayTagDisplaySubsystem::LoadDataTable(UDataTable* DataTable)
{
	if (!DataTable)
	{
		UE_LOG(LogTemp, Error, TEXT("LoadDataTable: Invalid DataTable"));
		return;
	}

	GameplayTagDisplayTable = DataTable;
	CachedDisplayData.Empty();

	// 모든 행을 캐시
	TArray<FGameplayTagDisplayData*> Rows;
	DataTable->GetAllRows<FGameplayTagDisplayData>(TEXT("LoadDataTable"), Rows);

	for (FGameplayTagDisplayData* Row : Rows)
	{
		if (Row && Row->Tag.IsValid())
		{
			CachedDisplayData.Add(Row->Tag, *Row);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("GameplayTagDisplaySubsystem: Cached %d tag display data"), CachedDisplayData.Num());
}

FText UGameplayTagDisplaySubsystem::GetDisplayName_KR(const FGameplayTag& Tag) const
{
	if (const FGameplayTagDisplayData* Data = CachedDisplayData.Find(Tag))
	{
		return Data->DisplayName_KR;
	}

	// 못 찾으면 태그 이름 그대로 반환
	return FText::FromString(Tag.ToString());
}

FText UGameplayTagDisplaySubsystem::GetDisplayName_EN(const FGameplayTag& Tag) const
{
	if (const FGameplayTagDisplayData* Data = CachedDisplayData.Find(Tag))
	{
		return Data->DisplayName_EN;
	}

	return FText::FromString(Tag.ToString());
}

FText UGameplayTagDisplaySubsystem::GetUnit(const FGameplayTag& Tag) const
{
	if (const FGameplayTagDisplayData* Data = CachedDisplayData.Find(Tag))
	{
		switch (Data->Unit)
		{
		case EDisplayUnit::NONE: return FText::GetEmpty();
		case EDisplayUnit::PERCENT: return FText::FromString(TEXT("%"));
		case EDisplayUnit::SEC: return FText::FromString(TEXT("s"));
		}
	}

	return FText::GetEmpty();
}

bool UGameplayTagDisplaySubsystem::GetDisplayData(const FGameplayTag& Tag, FGameplayTagDisplayData& OutData) const
{
	if (const FGameplayTagDisplayData* Data = CachedDisplayData.Find(Tag))
	{
		OutData = *Data;
		return true;
	}

	return false;
}

FText UGameplayTagDisplaySubsystem::GetDisplayNamesFromContainer_KR(const FGameplayTagContainer& Tags,
                                                                    const FString& Separator) const
{
	if (Tags.IsEmpty())
	{
		return FText::FromString(TEXT("없음"));
	}

	TArray<FString> DisplayNames;

	for (const FGameplayTag& Tag : Tags)
	{
		DisplayNames.Add(GetDisplayName_KR(Tag).ToString());
	}

	return FText::FromString(FString::Join(DisplayNames, *Separator));
}

UGameplayTagDisplaySubsystem* UGameplayTagDisplaySubsystem::Get(const UObject* WorldContextObject)
{
	if (!WorldContextObject)
	{
		return nullptr;
	}

	if (UWorld* World = WorldContextObject->GetWorld())
	{
		if (UGameInstance* GameInstance = World->GetGameInstance())
		{
			return GameInstance->GetSubsystem<UGameplayTagDisplaySubsystem>();
		}
	}

	return nullptr;
}
