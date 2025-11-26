#include "GAS/AbilityManager/TSAbilityManagerSubSystem.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"
#include "Abilities/GameplayAbility.h"
#include "DataAsset/TSAbilityDataAsset.h"
#include "DataAsset/TSAbilityDataRow.h"
#include "GAS/AbilityManager/AbilitySystemSetting.h"

DEFINE_LOG_CATEGORY(AbilityManager)

void UTSAbilityManagerSubSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	// // 어빌리티 매니저 초기화
	// // 1. GI에서 AbilityAsset 참조
	// if (UTSGameInstance* GI = Cast<UTSGameInstance>(GetGameInstance()))
	// {
	// 	if (GI->AbilityAsset)
	// 	{
	// 		// 2. DataAsset의 TMap을 Subsystem TMap으로 복사
	// 		AbilityTables = GI->AbilityAsset->AbilityTables;
	// 		for (auto Table : AbilityTables)
	// 		{
	// 			UE_LOG(LogTemp, Log, TEXT("Subsystem loaded Ability DataTable: %s"), *Table.Key.ToString());
	// 		}
	// 	}
	// }
	//

	const UAbilitySystemSetting* AbilitySystemSetting = UAbilitySystemSetting::GeAbilitySystemSetting();
	if (!IsValid(AbilitySystemSetting))
	{
		UE_LOG(AbilityManager, Error, TEXT("UAbilitySystemSetting 찾지 못함."));
		return;
	}
	
	if (AbilitySystemSetting->AbilityDataAsset == nullptr)
	{
		UE_LOG(AbilityManager, Error, TEXT("UAbilitySystemSetting 의 데이터 에셋 유효하지 않음."));
		return;;
	}
	
	UTSAbilityDataAsset* AbilityDataAsset = AbilitySystemSetting->AbilityDataAsset.LoadSynchronous();
	if (!IsValid(AbilityDataAsset))
	{
		UE_LOG(AbilityManager, Error, TEXT("AbilityDataAsset 로드 실패."));
		return;
	}
	
	if (!AbilityDataAsset->AbilityTables.Num())
	{
		UE_LOG(AbilityManager, Error, TEXT("AbilityDataAsset의 AbilityTables가 비어있음."));
		return;
	}
	
	AbilityTables = AbilitySystemSetting->AbilityDataAsset->AbilityTables;
	for (auto Table : AbilityTables)
	{
		UE_LOG(AbilityManager, Log, TEXT("Subsystem loaded Ability DataTable: %s"), *Table.Key.ToString());
	}
	
}

void UTSAbilityManagerSubSystem::GiveAbilityByTag(UAbilitySystemComponent* ASC, FGameplayTag DesiredTag, FName TableKey, int32 AbilityLevel, int32 InputID) const
{
	// 1. ASC, DesiredTag 유효성 검사
	if (!ASC) return;
	if (!DesiredTag.IsValid()) return;

	// 2. Table Key 로 AbilityTables 맵에서 UDataTable* 검색
	const UDataTable* AbilityDataTable = nullptr;
	if (const UDataTable* const* Found = AbilityTables.Find(TableKey))
	{
		AbilityDataTable = *Found;
	}
	if (!AbilityDataTable) return; //없으면 종료
	// 3. DataTable을 순회하며 DesiredTag와 일치하는 AbilityClass 찾기
	TArray<FTSAbilityDataRow*> Rows;
	AbilityDataTable->GetAllRows(TEXT("Ability Lookup"), Rows);
	TSubclassOf<UGameplayAbility> MatchingClass = nullptr;
	
	for (FTSAbilityDataRow* Row : Rows)
	{
		if (!Row || !Row->AbilityClass) continue;

		const UGameplayAbility* CDO = Row->AbilityClass->GetDefaultObject<UGameplayAbility>();
		if (!CDO) continue;

		if (CDO->AbilityTags.HasTagExact(DesiredTag))
		{
			MatchingClass = Row->AbilityClass; //일치하는 클래스 찾음
			break;
		}
	}
	if (!MatchingClass) return; //없으면 종료

	// 4. ASC에 이미 해당 AbilityClass 의 Spec 있는지 확인 
	if (FGameplayAbilitySpec* ExistingSpec = ASC->FindAbilitySpecFromClass(MatchingClass); ExistingSpec) return;

	// 5. ASC의 Owner가 서버 권한을 가졌는지 확인
	AActor* OwnerActor = ASC->GetOwner();
	if (!(OwnerActor && OwnerActor->HasAuthority())) return;

	// 6. FGameplayAbilitySpec을 생성하고 ASC에 어빌리티를 부여 (기브어빌리티)
	FGameplayAbilitySpec Spec(MatchingClass, AbilityLevel, (InputID == -1 ? INDEX_NONE : InputID));
	ASC->GiveAbility(Spec); //어빌리티 부여 실행
}
void UTSAbilityManagerSubSystem::ActivateAbilityByTag(UAbilitySystemComponent* ASC, FGameplayTag DesiredTag, FName TableKey, int32 AbilityLevel, int32 InputID) const
{
	// 1. 유효성 검사
	if (!ASC) return;
	if (!DesiredTag.IsValid()) return;

	//2. DesiredTag를 가진 어빌리티가 이미 부여되어있는지 확인하고 즉시 활성화 시도
	if (ASC->TryActivateAbilitiesByTag(FGameplayTagContainer(DesiredTag))) return;

	//3. 활성화 실패시 TableKey로 UDataTable* 검색
	const UDataTable* AbilityDataTable = nullptr;
    if (const UDataTable* const* Found = AbilityTables.Find(TableKey))
    {
        AbilityDataTable = *Found;
    }
    if (!AbilityDataTable) return;

	//4. DataTable 순회하며 DesiredTag와 일치하는 AbilityClass 찾기
    TArray<FTSAbilityDataRow*> Rows;
    AbilityDataTable->GetAllRows(TEXT("Ability Lookup"), Rows);
    TSubclassOf<UGameplayAbility> MatchingClass = nullptr;
	
    for (FTSAbilityDataRow* Row : Rows)
    {
        if (!Row || !Row->AbilityClass) continue;
        const UGameplayAbility* CDO = Row->AbilityClass->GetDefaultObject<UGameplayAbility>();
        if (!CDO) continue;

        if (CDO->AbilityTags.HasTagExact(DesiredTag))
        {
            MatchingClass = Row->AbilityClass;
            break;
        }
    }
    if (!MatchingClass) return;

	//5. 이미 부여된 Spec 이 있는지 확인
    if (FGameplayAbilitySpec* ExistingSpec = ASC->FindAbilitySpecFromClass(MatchingClass))
    {
        if (ASC->TryActivateAbility(ExistingSpec->Handle)) return;
    }

	//6. ASC의 Owner가 서버 권한을 가졌는지 확인
    AActor* OwnerActor = ASC->GetOwner();
    if (!(OwnerActor && OwnerActor->HasAuthority())) return;

	//7. Spec을 생성, ASC에 부여(기브어빌)하고 반환된 핸들로 즉시 활성화(트라이어빌) 시도
    FGameplayAbilitySpec Spec(MatchingClass, AbilityLevel, (InputID == -1 ? INDEX_NONE : InputID));
	ASC->TryActivateAbility(ASC->GiveAbility(Spec));

}
void UTSAbilityManagerSubSystem::ClearAbilityByTag(UAbilitySystemComponent* ASC, FGameplayTag DesiredTag, FName TableKey, int32 AbilityLevel, int32 InputID) const
{
	//1.유효성검사
	if (!ASC) return;
    if (!DesiredTag.IsValid()) return;

	//2. TableKey로 UDataTable* 검색
	const UDataTable* AbilityDataTable = nullptr;
    if (const UDataTable* const* Found = AbilityTables.Find(TableKey))
    {
        AbilityDataTable = *Found;
    }

    if (!AbilityDataTable) return;

	//3. DataTable 을 순회하며 DesiredTag와 일치하는 AbilityClass 찾기
    TArray<FTSAbilityDataRow*> Rows;
    AbilityDataTable->GetAllRows(TEXT("Ability Lookup"), Rows);
    TSubclassOf<UGameplayAbility> MatchingClass = nullptr;

    for (FTSAbilityDataRow* Row : Rows)
    {
        if (!Row || !Row->AbilityClass) continue;
        const UGameplayAbility* CDO = Row->AbilityClass->GetDefaultObject<UGameplayAbility>();
        if (!CDO) continue;

        if (CDO->AbilityTags.HasTagExact(DesiredTag))
        {
            MatchingClass = Row->AbilityClass;
            break;
        }
    }
    if (!MatchingClass) return;

	//4. ASC에서 해당 AbilityClass 로 Spec 검색
    if (FGameplayAbilitySpec* ExistingSpec = ASC->FindAbilitySpecFromClass(MatchingClass))
    {
    	//5. ASC 의 Owner 가 서버 권한을 가졌는지 확인
        AActor* OwnerActor = ASC->GetOwner();
        if (!(OwnerActor && OwnerActor->HasAuthority())) return;
    	//6. SpecHandle 을 사용하여 어빌리티 제거(클리어어빌)
        ASC->ClearAbility(ExistingSpec->Handle);
    }

}
