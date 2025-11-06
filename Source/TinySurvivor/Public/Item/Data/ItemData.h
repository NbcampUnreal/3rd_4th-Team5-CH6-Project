// ItemData.h
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "Abilities/GameplayAbility.h"
#include "ItemData.generated.h"

#pragma region Enums
/*
	아이템의 종류를 나타내는 Enum
*/
UENUM(BlueprintType)
enum class EItemCategory : uint8
{
	MATERIAL    UMETA(DisplayName = "Material", ToolTip="재료"),
	WEAPON      UMETA(DisplayName = "Weapon", ToolTip="무기"),
	TOOL        UMETA(DisplayName = "Tool", ToolTip="도구"),
	CONSUMABLE  UMETA(DisplayName = "Consumable", ToolTip="소모품"),
	ARMOR       UMETA(DisplayName = "Armor", ToolTip="방어구")
};

/*
	아이템의 희귀도를 나타내는 Enum
*/
UENUM(BlueprintType)
enum class EItemRarity : uint8
{
	NONE    UMETA(DisplayName = "None", ToolTip="등급 없음"),
	COMMON  UMETA(DisplayName = "Common", ToolTip="일반"),
	NORMAL  UMETA(DisplayName = "Normal", ToolTip="보통"),
	RARE    UMETA(DisplayName = "Rare", ToolTip="희귀"),
	UNIQUE  UMETA(DisplayName = "Unique", ToolTip="유일/특별")
};

/*
	장비의 장착 부위를 나타내는 Enum
*/
UENUM(BlueprintType)
enum class EEquipSlot : uint8
{
	HEAD    UMETA(DisplayName = "Head", ToolTip="머리 장착 부위"),
	TORSO   UMETA(DisplayName = "Torso", ToolTip="몸통 장착 부위"),
	LEG     UMETA(DisplayName = "Leg", ToolTip="다리 장착 부위")
};
#pragma endregion

#pragma region WeaponData
/*
	무기 전용 능력치
*/
USTRUCT(BlueprintType)
struct FWeaponData
{
	GENERATED_BODY()

public:
	FWeaponData()
	: bEquipable(false)
	, DamageValue(0.f)
	, AttackSpeed(1.f)
	, AttackRange(100.f)
	, MaxDurability(100)
	{}
	
	// 장착 가능 여부
	UPROPERTY(EditAnywhere, BlueprintReadOnly,
		meta = (DisplayName="Equipable (장착 가능 여부)", ToolTip="무기를 장착할 수 있는지 여부"))
	bool bEquipable;
	
	// 기본 공격력
	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		meta = (DisplayName="DamageValue (기본 공격력)", ToolTip="무기가 가진 기본 공격력 수치"))
	float DamageValue;

	// 공격과 다음 공격 사이의 딜레이
	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		meta = (DisplayName="AttackSpeed (공격 속도)", ToolTip="공격 사이의 딜레이(초 단위)"))
	float AttackSpeed;

	// 공격 사거리
	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		meta = (DisplayName="AttackRange (공격 사거리)", ToolTip="공격이 유효한 사거리(유닛 단위)"))
	float AttackRange;

	// 최대 내구도
	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		meta = (DisplayName="MaxDurability (최대 내구도)", ToolTip="무기의 최대 내구도"))
	int32 MaxDurability;
};
#pragma endregion

#pragma region ToolData
/*
	도구 전용 능력치
*/
USTRUCT(BlueprintType)
struct FToolData
{
	GENERATED_BODY()

public:
	FToolData()
	: bEquipable(false)
	, HarvestLevel(1)
	, SpeedMultiplier(1.0f)
	, MaxDurability(100)
	, DurabilityLossRate(1.0f)
	{}
	
	// 장착 가능 여부
	UPROPERTY(EditAnywhere, BlueprintReadOnly,
		meta = (DisplayName="Equipable (장착 가능 여부)", ToolTip="도구를 장착할 수 있는지 여부"))
	bool bEquipable;

	// 최소 채취 가능 레벨
	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		meta = (DisplayName="HarvestLevel (최소 채취 가능 레벨)", ToolTip="이 도구로 채취할 수 있는 최소 레벨"))
	int32 HarvestLevel; 
	
	// 채취 대상 태그
	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		meta = (Categories="Item.HarvestTarget"
			, DisplayName="HarvestTargetTag (채취 대상 태그)", ToolTip="이 도구로 채취할 수 있는 자원 태그"))
	FGameplayTagContainer HarvestTargetTag;
	
	// 채취 속도 계수
	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		meta = (DisplayName="SpeedMultiplier (채취 속도 계수)", ToolTip="채취 속도에 곱해지는 계수"))
	float SpeedMultiplier;
	
	// 최대 내구도
	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		meta = (DisplayName="MaxDurability (최대 내구도)", ToolTip="도구의 최대 내구도"))
	int32 MaxDurability;
	
	// 내구도 소모량
	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		meta = (DisplayName="DurabilityLossRate (내구도 소모량)", ToolTip="도구 사용 시 감소하는 내구도 비율"))
	float DurabilityLossRate;
};
#pragma endregion

#pragma region ConsumableData
/*
	소모품 전용 데이터
*/
USTRUCT(BlueprintType)
struct FConsumableData
{
	GENERATED_BODY()

public:
	FConsumableData()
	: ConsumptionTime(1.0f)
	, EffectDuration(0.0f)
	, bDecayEnabled(false)
	, DecayRate(0.0f)
	{}
	
	// 사용 시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		meta = (DisplayName="ConsumptionTime (사용 시간)", ToolTip="소모품을 사용하는 데 걸리는 시간"))
	float ConsumptionTime;
	
	// 효과 지속 시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		meta = (DisplayName="EffectDuration (효과 지속 시간)", ToolTip="효과가 지속되는 시간(초)"))
	float EffectDuration;
	
	// 부패 시스템 적용 여부
	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		meta = (DisplayName="Decay (부패 시스템 적용 여부)", ToolTip="부패 시스템이 적용되는지 여부"))
	bool bDecayEnabled;
	
	// 부패 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		meta = (DisplayName="DecayRate (부패 속도)", ToolTip="부패가 진행되는 속도"))
	float DecayRate;
};
#pragma endregion

#pragma region ArmorData
/*
	방어구 전용 데이터
*/
USTRUCT(BlueprintType)
struct FArmorData
{
	GENERATED_BODY()

public:
	FArmorData()
	: EquipSlot(EEquipSlot::TORSO) // 기본값 예시
	, HealthBonus(0.0f)
	, DamageReductionRate(0.0f)
	, MoveSpeedBonus(0.0f)
	, SpecialEffectID(0)
	, RequiredRarity(1)
	, MaxDurability(100)
	{}
	
	// 장착 가능 부위
	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		meta = (DisplayName="EquipSlot (장착 가능 부위)", ToolTip="장착 가능 부위 정의 (HEAD, TORSO, LEG 등)"))
	EEquipSlot EquipSlot;
	
	// 최대 체력 추가 수치
	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		meta = (DisplayName="HealthBonus (최대 체력 추가 수치)", ToolTip="플레이어 최대 체력에 추가되는 수치"), 
		AdvancedDisplay)
	float HealthBonus;
	
	// 피해 감소율 (0.0 ~ 1.0)
	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		meta = (DisplayName="DamageReductionRate (피해 감소율)", ToolTip="해당 방어구가 제공하는 피해 감소율 (0.0 ~ 1.0)"), 
		AdvancedDisplay)
	float DamageReductionRate;
	
	// 이동 속도 증가 보너스 (다리 부위 전용)
	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		meta = (DisplayName="MoveSpeedBonus (이동 속도 증가 보너스)", ToolTip="해당 방어구가 제공하는 이동 속도 증가 보너스 (다리 부위 전용)"), 
		AdvancedDisplay)
	float MoveSpeedBonus;
	
	// 고유 효과 ID (Effect Tag 테이블 참조)
	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		meta = (DisplayName="SpecialEffectID (고유 효과 ID)", ToolTip="피해 반사 등 고유 효과가 있는 경우 참조하는 ID"), 
		AdvancedDisplay)
	int32 SpecialEffectID;
	
	// 제작에 필요한 기술 레벨
	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		meta = (DisplayName="RequiredRarity (제작에 필요한 기술 레벨)", ToolTip="해당 방어구를 제작하는데 필요한 기술 레벨"))
	int32 RequiredRarity;

	// 최대 내구도
	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		meta = (DisplayName="MaxDurability (최대 내구도)", ToolTip="방어구의 최대 내구도"))
	int32 MaxDurability;
};
#pragma endregion

#pragma region ItemEffectData
// 복합 효과
// USTRUCT(BlueprintType)
// struct FItemEffectData
// {
// 	GENERATED_BODY()
//
// public:
// 	FItemEffectData()
// 		: EffectValue(0.f)
// 	{}
// 	
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Categories="Item.Effect"))
// 	FGameplayTag EffectTag;
//
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite)
// 	float EffectValue;
// };
#pragma endregion

#pragma region FItemData
/*
	모든 아이템 공통 데이터
*/
USTRUCT(BlueprintType)
struct FItemData : public FTableRowBase
{
	GENERATED_BODY()

public:
	// 생성자: 기본값 초기화
	FItemData()
		: ItemID(0)
		, Name_KR(FText::FromString(TEXT("")))
		, Name_EN(FText::FromString(TEXT("")))
		, Category(EItemCategory::MATERIAL)
		, WeaponData()         // FWeaponData 기본 생성자 호출
		, ToolData()           // FToolData 기본 생성자 호출
		, ConsumableData()     // FConsumableData 기본 생성자 호출
		, ArmorData()          // FArmorData 기본 생성자 호출
		, Rarity(EItemRarity::NONE)
		, MaxStack(1)
		, EffectTag()          // 단일 효과 태그 초기화
		, EffectValue(0.f)     // 단일 효과 수치 초기화
		//, ItemEffects()        // TArray 기본 생성자
		, Icon(nullptr)
		, WorldMesh(nullptr)
		, bSpawnAsActor(false)
		, ActorClass(nullptr)
		, AbilityBP(nullptr)
	{}
	
#pragma region Identifier
	//==================================
	// 식별자
	//==================================
	// 아이템 고유 ID
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Base-Identifier",
		meta=(ClampMin="1", ClampMax="999", DisplayName="ItemID (아이템 ID)", ToolTip="아이템 고유 ID"))
	int32 ItemID;
	
	// 한국어 이름
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Base-Identifier",
		meta = (DisplayName="Name_KR (한글명)", ToolTip="아이템의 한국어 이름"))
	FText Name_KR;
	
	// 영어 이름
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Base-Identifier",
		meta = (DisplayName="Name_EN (영문명)", ToolTip="아이템의 영어 이름"))
	FText Name_EN;
#pragma endregion

#pragma region System
	//==================================
	// 시스템
	//==================================
	// 아이템 카테고리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Base-System",
		meta = (DisplayName="Category (카테고리)", ToolTip="아이템이 속한 카테고리"))
	EItemCategory Category;
	
	// 무기 전용 데이터
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Base-System-Weapon",
		meta=(EditCondition="Category==EItemCategory::WEAPON", EditConditionHides,
			DisplayName="WeaponData (무기 전용 데이터)", ToolTip="무기 전용 데이터"))
	FWeaponData WeaponData;
	
	// 도구 전용 데이터
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Base-System-Tool",
		meta=(EditCondition="Category==EItemCategory::TOOL", EditConditionHides,
			DisplayName="ToolData (도구 전용 데이터)", ToolTip="도구 전용 데이터"))
	FToolData ToolData;
	
	// 소모품 전용 데이터
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Base-System-Consumable",
		meta=(EditCondition="Category==EItemCategory::CONSUMABLE", EditConditionHides,
			DisplayName="ConsumableData (소모품 전용 데이터)", ToolTip="소모품 전용 데이터"))
	FConsumableData ConsumableData;
	
	// 방어구 전용 데이터
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Base-System-Armor",
		meta=(EditCondition="Category==EItemCategory::ARMOR", EditConditionHides,
			DisplayName="ArmorData (방어구 전용 데이터)", ToolTip="방어구 전용 데이터"))
	FArmorData ArmorData;
	
	// 등급
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Base-System",
		meta = (DisplayName="Rarity (등급)", ToolTip="아이템의 등급"))
	EItemRarity Rarity;
	
	// 최대 중첩 개수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Base-System",
		meta = (DisplayName="MaxStack (최대 중첩 개수)", ToolTip="아이템의 최대 스택 수"))
	int32 MaxStack;

	// 런타임 내구도
	UPROPERTY(Transient, BlueprintReadWrite, Category="Runtime")
	int32 CurrentDurability;
#pragma endregion

#pragma region Effect
	//==================================
	// 효과
	// 한 아이템에 여러 효과 태그가 붙는다면, 태그별 값 매핑 구조가 필요
	//==================================
	
	// 아이템 효과 태그
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Base-Effect",
		meta = (Categories="Item.Effect", DisplayName="EffectTag", ToolTip="아이템이 가진 단일 효과 태그"))
	FGameplayTag EffectTag;
	
	// 태그별 효과 수치
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Base-Effect",
		meta = (DisplayName="EffectValue", ToolTip="효과 수치"))
	float EffectValue;

	// // 복합 효과 - 여러 효과를 가질 수 있도록
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Base-Effect",
	// 	meta = (DisplayName="ItemEffects (효과 목록)", ToolTip="아이템이 가진 효과 태그와 값 목록"))
	// TArray<FItemEffectData> ItemEffects;
#pragma endregion

#pragma region Visual
	//==================================
	// 비주얼
	//==================================
	// 아이콘
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Base-Visual",
		meta = (DisplayName="Icon (아이콘)", ToolTip="인벤토리 및 UI 표시용 아이콘"))
	TSoftObjectPtr<UTexture2D> Icon;
	
	// 3D 메시
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Base-Visual",
		meta = (DisplayName="WorldMesh (3D 메시)", ToolTip="월드 드롭 및 프리뷰용 메시"))
	TSoftObjectPtr<UStaticMesh> WorldMesh;
#pragma endregion

#pragma region Spawn
	//========================================
	// 기획 비포함, 개발 편의상 추가
	// 오브젝트 풀링 관련
	//========================================
	
	// 스폰 액터 여부
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Spawn",
		meta = (DisplayName="bSpawnAsActor (액터 타입 여부)", ToolTip="풀링될 오브젝트가 액터 타입인지 여부"))
	bool bSpawnAsActor;
	
	// 액터 클래스
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Spawn",
		meta = (EditCondition="bSpawnAsActor", EditConditionHides,
			DisplayName="ActorClass (풀링 액터 클래스)", ToolTip="액터 타입일 경우 사용할 블루프린트 클래스"))
	TSubclassOf<AActor> ActorClass;
#pragma endregion

#pragma region Ability
	//========================================
	// 기획 비포함, 개발 편의상 추가
	// GameplayAbility
	//========================================
	
	// Ability BP - 단일 효과
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ability",
		meta = (DisplayName="AbilityBP (Gameplay Ability BP)", ToolTip="Gameplay Ability로 사용할 블루프린트 클래스"))
	TSubclassOf<UGameplayAbility> AbilityBP;

	// // 복합 효과 - TArray로 변경 고려
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ability"
	// 	, meta = (DisplayName="AbilityBP (Gameplay Ability BP)", ToolTip="Gameplay Ability로 사용할 블루프린트 클래스"))
	// TArray<TSubclassOf<UGameplayAbility>> AbilityClasses;
#pragma endregion

#pragma region HelperFunctions
	//========================================
	// 기획 비포함, 개발 편의상 추가
	// 헬퍼 함수
	//========================================

public:
	/*
		카테고리에 관계없이 장착 가능 여부 확인
		@return 장착 가능하면 true, 아니면 false
	*/
	bool IsEquipable() const
	{
		switch(Category)
		{
		case EItemCategory::WEAPON: return WeaponData.bEquipable;
		case EItemCategory::TOOL: return ToolData.bEquipable;
		default: return false;
		}
	}

	/*
		카테고리에 관계없이 최대 내구도 조회
		@return 최대 내구도 (내구도 없는 아이템은 0 반환)
	*/
	int32 GetMaxDurability() const
	{
		switch(Category)
		{
		case EItemCategory::WEAPON: return WeaponData.MaxDurability;
		case EItemCategory::TOOL: return ToolData.MaxDurability;
		case EItemCategory::ARMOR: return ArmorData.MaxDurability;
		default: return 0;
		}
	}

	/*
		내구도 시스템 적용 여부 확인
		@return 내구도가 있는 아이템이면 true
	*/
	bool HasDurability() const
	{
		return GetMaxDurability() > 0;
	}

	/*
		효과가 있는 아이템인지 확인
		@return 효과 태그가 유효하면 true
	*/
	bool HasEffect() const
	{
		return EffectTag.IsValid();

		// 복합 효과
		// return EffectTag.IsValid() || ItemEffects.Num() > 0;
	}

	/*
		지속 효과가 있는지 확인 (소모품 전용)
		@return 소모품이고 EffectDuration > 0이면 true
	*/
	bool HasDurationEffect() const
	{
		return Category == EItemCategory::CONSUMABLE 
			&& ConsumableData.EffectDuration > 0.0f;
	}

	/*
		부패 시스템 적용 여부 확인 (소모품 전용)
		@return 부패 시스템이 활성화되어 있으면 true
	*/
	bool IsDecayEnabled() const
	{
		return Category == EItemCategory::CONSUMABLE 
			&& ConsumableData.bDecayEnabled;
	}

	/*
		스택 가능 여부 확인
		@return MaxStack > 1이면 true
	*/
	bool IsStackable() const
	{
		return MaxStack > 1;
	}

	/*
		에셋 유효성 검증 - 아이콘
		@return 아이콘이 설정되어 있으면 true
	*/
	bool IsIconValid() const
	{
		return !Icon.IsNull();
	}

	/*
		에셋 유효성 검증 - 월드 메시
		@return 월드 메시가 설정되어 있으면 true
	*/
	bool IsWorldMeshValid() const
	{
		return !WorldMesh.IsNull();
	}

	/*
		스폰 설정 유효성 검증
		@return 액터 스폰이 가능하면 true
	*/
	bool CanSpawnAsActor() const
	{
		return bSpawnAsActor && ActorClass != nullptr;
	}

	/*
		GAS Ability 설정 유효성 검증
		@return Ability BP가 설정되어 있으면 true
	*/
	bool HasAbility() const
	{
		return AbilityBP != nullptr;
	}

	/*
		카테고리별 데이터 유효성 검증
		@return 해당 카테고리의 전용 데이터가 올바르게 설정되어 있으면 true
	*/
	bool ValidateCategoryData() const
	{
		switch(Category)
		{
		case EItemCategory::WEAPON:
			return WeaponData.DamageValue > 0.0f 
				&& WeaponData.AttackSpeed > 0.0f 
				&& WeaponData.AttackRange > 0.0f;
			
		case EItemCategory::TOOL:
			return ToolData.HarvestLevel > 0 
				&& ToolData.SpeedMultiplier > 0.0f;
			
		case EItemCategory::CONSUMABLE:
			return ConsumableData.ConsumptionTime > 0.0f;
			
		case EItemCategory::MATERIAL:
			return true; // 재료는 공통 데이터만 사용
			
		default:
			return false;
		}
	}

	/*
		디버그용 아이템 정보 문자열 생성
		@return 아이템 정보를 담은 문자열
	*/
	FString ToString() const
	{
		FString CategoryStr;
		switch(Category)
		{
		case EItemCategory::MATERIAL: CategoryStr = TEXT("Material"); break;
		case EItemCategory::WEAPON: CategoryStr = TEXT("Weapon"); break;
		case EItemCategory::TOOL: CategoryStr = TEXT("Tool"); break;
		case EItemCategory::CONSUMABLE: CategoryStr = TEXT("Consumable"); break;
		default: CategoryStr = TEXT("Unknown"); break;
		}

		return FString::Printf(
			TEXT("[ID:%d] %s (%s) | MaxStack:%d | Rarity:%d"),
			ItemID,
			*Name_KR.ToString(),
			*CategoryStr,
			MaxStack,
			static_cast<int32>(Rarity)
		);
	}

	/*
		디버그용 상세 정보 로그 출력
	*/
	void PrintDebugInfo() const
	{
		UE_LOG(LogTemp, Log, TEXT("=== Item Debug Info ==="));
		UE_LOG(LogTemp, Log, TEXT("ID: %d"), ItemID);
		UE_LOG(LogTemp, Log, TEXT("Name (KR): %s"), *Name_KR.ToString());
		UE_LOG(LogTemp, Log, TEXT("Name (EN): %s"), *Name_EN.ToString());
		UE_LOG(LogTemp, Log, TEXT("Category: %d"), static_cast<int32>(Category));
		UE_LOG(LogTemp, Log, TEXT("MaxStack: %d"), MaxStack);
		UE_LOG(LogTemp, Log, TEXT("Equipable: %s"), IsEquipable() ? TEXT("Yes") : TEXT("No"));
		UE_LOG(LogTemp, Log, TEXT("Has Durability: %s"), HasDurability() ? TEXT("Yes") : TEXT("No"));
		
		if (HasDurability())
		{
			UE_LOG(LogTemp, Log, TEXT("Max Durability: %d"), GetMaxDurability());
		}
		
		if (HasEffect())
		{
			UE_LOG(LogTemp, Log, TEXT("Effect Tag: %s"), *EffectTag.ToString());
			UE_LOG(LogTemp, Log, TEXT("Effect Value: %.2f"), EffectValue);
		}
		
		UE_LOG(LogTemp, Log, TEXT("======================"));
	}
	
#pragma endregion
};
#pragma endregion