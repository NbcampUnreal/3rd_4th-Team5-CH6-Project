// ItemMetaData.h
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "ItemMetaData.generated.h"

// 아이템 카테고리 ENUM
UENUM(BlueprintType)
enum class EItemCategory : uint8
{
	Material    UMETA(DisplayName = "Material", ToolTip = "재료 아이템"),
	Tool        UMETA(DisplayName = "Tool", ToolTip = "도구 아이템"),
	Weapon      UMETA(DisplayName = "Weapon", ToolTip = "무기 아이템"),
	Consumable  UMETA(DisplayName = "Consumable", ToolTip = "소모품 아이템"),
	Building    UMETA(DisplayName = "Building", ToolTip = "건축물 아이템"),
	Story       UMETA(DisplayName = "Story", ToolTip = "단서/로어 아이템")
};

// 획득 방식 ENUM
UENUM(BlueprintType)
enum class EItemSourceType : uint8
{
	None        UMETA(DisplayName = "None", ToolTip = "아이템 획득 없음"),
	Harvest     UMETA(DisplayName = "Harvest", ToolTip = "자원 채취로 획득"),
	MonsterDrop UMETA(DisplayName = "MonsterDrop", ToolTip = "몬스터 드롭 아이템"),
	Chest       UMETA(DisplayName = "Chest", ToolTip = "숨겨진 상자에서 획득"),
	Craft       UMETA(DisplayName = "Craft", ToolTip = "제작으로 획득"),
	Interaction UMETA(DisplayName = "Interaction", ToolTip = "필드 오브젝트와 상호작용하여 획득"),
	Install     UMETA(DisplayName = "Install", ToolTip = "건축물 설치로 획득")
};

// 아이템 등급 ENUM
UENUM(BlueprintType)
enum class EItemRarity : uint8
{
	None   UMETA(DisplayName = "None", ToolTip = "없음"),
	Common UMETA(DisplayName = "Common", ToolTip = "일반 등급"),
	Normal UMETA(DisplayName = "Normal", ToolTip = "보통 등급"),
	Rare   UMETA(DisplayName = "Rare", ToolTip = "희귀 등급")
};

// 티어 ENUM
UENUM(BlueprintType)
enum class EMaterialTier : uint8
{
	T1 UMETA(DisplayName = "T1", ToolTip = "Tier 1"),
	T2 UMETA(DisplayName = "T2", ToolTip = "Tier 2"),
	T3 UMETA(DisplayName = "T3", ToolTip = "Tier 3")
};

// 제작대 ENUM
UENUM(BlueprintType)
enum class ECraftingStationType : uint8
{
	None        UMETA(DisplayName = "None", ToolTip = "No installation"), 
	CraftingBox   UMETA(DisplayName = "CraftingBox", ToolTip = "제작대"),  
	Brazier     UMETA(DisplayName = "Brazier", ToolTip = "화로"),  
	Cooker      UMETA(DisplayName = "Cooker", ToolTip = "요리솥")  
};

/*
	크래프팅 재료 정보 구조체
	기획: RequiredItems (재료 ID 및 수량 배열)
*/
USTRUCT(BlueprintType)
struct FItemCraftingMaterial
{
	GENERATED_BODY()
	
	// 필요한 재료 아이템 ID
	UPROPERTY(EditAnywhere, BlueprintReadOnly
		, meta = (DisplayName = "MaterialItemID (재료 ID)", ToolTip = "재료 ID"))
	int32 MaterialItemID = 0;
	
	// 필요 수량
	UPROPERTY(EditAnywhere, BlueprintReadOnly
		, meta=(ClampMin="1"
			, DisplayName = "RequiredQuantity (재료 수량)", ToolTip = "재료 수량"))
	int32 RequiredQuantity = 1;
};

/*
	크래프팅 제작 정보
*/
USTRUCT(BlueprintType)
struct FItemCraftingData
{
	GENERATED_BODY()
	
	// 레시피 ID
	UPROPERTY(EditAnywhere, BlueprintReadOnly
		, meta = (DisplayName = "RecipeID (레시피 ID)", ToolTip = "레시피 ID"))
	int32 RecipeID = 0;

	// 제작 완료 시 생성되는 아이템 ID
	UPROPERTY(EditAnywhere, BlueprintReadOnly
		, meta = (DisplayName = "OutputItemID (제작 완료 아이템의 ID)", ToolTip = "제작 완료 아이템의 ID"))
	int32 OutputItemID = 0;
	
	// 필요한 재료 목록
	UPROPERTY(EditAnywhere, BlueprintReadOnly
		, meta = (DisplayName = "RequiredItems (필요한 재료 목록)", ToolTip = "필요한 재료 목록"))
	TArray<FItemCraftingMaterial> RequiredItems;
	
	// 제작 소요 시간 (초)
	UPROPERTY(EditAnywhere, BlueprintReadOnly
		, meta=(ClampMin="0.0"
			, DisplayName = "CraftingTime (제작 소요 시간, 초)", ToolTip = "제작 소요 시간, 초"))
	float CraftingTime = 1.0f;
	
	// 필요한 제작대 ID
	UPROPERTY(EditAnywhere, BlueprintReadOnly
		, meta = (DisplayName = "RequiredTool_ID (필요한 제작대 ID)", ToolTip = "필요한 제작대 ID"))
	ECraftingStationType RequiredTool_ID = ECraftingStationType::None;
	//int32 RequiredTool_ID = 0;

	// 연소 가능 여부
	UPROPERTY(EditAnywhere, BlueprintReadOnly
		, meta = (DisplayName = "Combustible (재료의 특수 태그)", ToolTip = "재료의 특수 태그"))
	bool bCombustible = false;
};

/*
	자원 채취 정보
	채취 후 5분간 리스폰 대기 상태로 전환
*/
USTRUCT(BlueprintType)
struct FItemHarvestData
{
	GENERATED_BODY()
	
	// 채취 소요 시간 (초)
	UPROPERTY(EditAnywhere, BlueprintReadOnly
		, meta=(ClampMin="0.1"
			, DisplayName = "HarvestTime (채취 소요 시간, 초)", ToolTip = "채취 소요 시간, 초"))
	float HarvestTime = 1.0f;
	
	// 드롭 수량
	UPROPERTY(EditAnywhere, BlueprintReadOnly
		, meta=(ClampMin="1"
			, DisplayName = "DropQuantity (드롭 수량)", ToolTip = "드롭 수량"))
	int32 DropQuantity = 1;
	
	// 드롭 수량 랜덤 범위
	UPROPERTY(EditAnywhere, BlueprintReadOnly
		, meta = (DisplayName = "DropRandomRange (드롭 수량 범위)", ToolTip = "드롭 수량 범위"))
	FInt32Range DropRandomRange = FInt32Range(1, 3);
	
	// 필요한 도구 ID
	UPROPERTY(EditAnywhere, BlueprintReadOnly
		, meta = (DisplayName = "RequiredTool (필요한 도구 ID)", ToolTip = "필요한 도구 ID"))
	int32 RequiredTool = 0;
	
	// // 채취 후 리스폰 대기 시간 (초) - 기획: 5분 = 300초
	// UPROPERTY(EditAnywhere, BlueprintReadOnly
	// 	, meta=(ClampMin="0.0"
	// 		, DisplayName = "RespawnWaitTime (리스폰 대기 시간)", ToolTip = "리스폰 대기 시간"))
	// float RespawnWaitTime = 300.0f;
};

/*
	몬스터 드롭 정보
*/
USTRUCT(BlueprintType)
struct FItemMonsterDropData
{
	GENERATED_BODY()
	
	// 몬스터 드롭 테이블 ID
	UPROPERTY(EditAnywhere, BlueprintReadOnly
		, meta = (DisplayName = "DropTable_ID (몬스터 드롭 테이블 ID)", ToolTip = "몬스터 드롭 테이블 ID"))
	int32 DropTable_ID = 0;
	
	// 드롭 확률 (0.0 ~ 1.0)
	UPROPERTY(EditAnywhere, BlueprintReadOnly
		, meta=(ClampMin="0.0", ClampMax="1.0"
			, DisplayName = "DropChance (드롭 확률)", ToolTip = "드롭 확률"))
	float DropChance = 0.5f;
};

/*
	상자 획득 정보
*/
USTRUCT(BlueprintType)
struct FItemChestLootData
{
	GENERATED_BODY()
	
	// 루트 테이블 ID
	UPROPERTY(EditAnywhere, BlueprintReadOnly
		, meta = (DisplayName = "LootTable_ID (루트 테이블 ID)", ToolTip = "루트 테이블 ID"))
	int32 LootTable_ID = 0;
	
	// 필요한 열쇠 아이템 ID
	UPROPERTY(EditAnywhere, BlueprintReadOnly
		, meta = (DisplayName = "RequiredKeyID (열쇠 아이템 ID)", ToolTip = "열쇠 아이템 ID"))
	int32 RequiredKeyID = 0;
};

/*
	자원 상호작용 정보
	기획(예상): 필드 내 오브젝트 상호작용, 드롭 확률
*/
USTRUCT(BlueprintType)
struct FItemInteractionData
{
	GENERATED_BODY()
	
	// 상호작용 가능 여부 (디버깅/조건 스킵용)
	UPROPERTY(EditAnywhere, BlueprintReadOnly
		, meta = (DisplayName = "CanInteract (상호작용 가능 여부)", ToolTip = "상호작용 가능 여부"))
	bool bCanInteract = true;
	
	// 필요한 도구 카테고리 (예: Tool.Pickaxe, Tool.Axe 등)
	UPROPERTY(EditAnywhere, BlueprintReadOnly
		, meta=(Categories="Item.Category.Tool"
			, DisplayName = "RequiredToolCategory (필요한 도구 카테고리)", ToolTip = "필요한 도구 카테고리"))
	FGameplayTag RequiredToolCategory;
	
	// 필요한 최소 도구 등급
	UPROPERTY(EditAnywhere, BlueprintReadOnly
		, meta = (DisplayName = "MinRequiredToolRarity (필요한 최소 도구 등급)", ToolTip = "필요한 최소 도구 등급"))
	EItemRarity MinRequiredToolRarity = EItemRarity::None;
	
	// 채취 소요 시간 (초)
	UPROPERTY(EditAnywhere, BlueprintReadOnly
		, meta=(ClampMin="0.1"
			, DisplayName = "InteractionTime (채취 소요 시간, 초)", ToolTip = "채취 소요 시간, 초"))
	float InteractionTime = 1.0f;
	
	// 드롭 수량
	UPROPERTY(EditAnywhere, BlueprintReadOnly
		, meta=(ClampMin="1"
			, DisplayName = "DropQuantity (드롭 수량)", ToolTip = "드롭 수량"))
	int32 DropQuantity = 1;
	
	// 드롭 수량 랜덤 범위
	UPROPERTY(EditAnywhere, BlueprintReadOnly
		, meta = (DisplayName = "DropRandomRange (드롭 수량 랜덤 범위)", ToolTip = "드롭 수량 랜덤 범위"))
	FInt32Range DropRandomRange = FInt32Range(1, 1);
	
	// 드롭 확률 (None, Common, Normal, Rare)
	UPROPERTY(EditAnywhere, BlueprintReadOnly
		, meta = (DisplayName = "DropRarity (드롭 확률)", ToolTip = "드롭 확률"))
	EItemRarity DropRarity = EItemRarity::None;
};

/*
	도구 관련 정보
	기획(예상): Attack, AttackSpeed, 허용되는 등급, 내구도
*/
USTRUCT(BlueprintType)
struct FItemToolData
{
	GENERATED_BODY()
	
	// 공격력
	UPROPERTY(EditAnywhere, BlueprintReadOnly
		, meta=(ClampMin="0"
			, DisplayName = "Attack (공격력)", ToolTip = "공격력"))
	int32 Attack = 1;
	
	// 공격 속도
	UPROPERTY(EditAnywhere, BlueprintReadOnly
		, meta=(ClampMin="0.1"
			, AttackSpeed = "AttackSpeed (공격 속도)", ToolTip = "공격 속도"))
	float AttackSpeed = 1.0f;
	
	// 허용되는 등급 (도구가 채취할 수 있는 재료 등급)
	UPROPERTY(EditAnywhere, BlueprintReadOnly
		, meta = (DisplayName = "AllowedRarity (도구가 채취할 수 있는 재료 등급)", ToolTip = "도구가 채취할 수 있는 재료 등급"))
	EItemRarity AllowedRarity = EItemRarity::None;
	
	// 총알 필요 여부 (새총 등)
	UPROPERTY(EditAnywhere, BlueprintReadOnly
		, meta = (DisplayName = "RequiresBullet (총알 필요 여부)", ToolTip = "총알 필요 여부"))
	bool bRequiresBullet = false;
};

/*
	소모품 관련 정보
	기획(예상): 배고픔, 갈증, HP, Sanity 회복
*/
USTRUCT(BlueprintType)
struct FItemConsumableData
{
	GENERATED_BODY()
	
	// 배고픔 회복량
	UPROPERTY(EditAnywhere, BlueprintReadOnly
		, meta = (DisplayName = "HungerRestore (배고픔 회복량)", ToolTip = "배고픔 회복량"))
	float HungerRestore = 0.0f;
	
	// 갈증 회복량
	UPROPERTY(EditAnywhere, BlueprintReadOnly
		, meta = (DisplayName = "ThirstRestore (갈증 회복량)", ToolTip = "갈증 회복량"))
	float ThirstRestore = 0.0f;
	
	// HP 회복량
	UPROPERTY(EditAnywhere, BlueprintReadOnly
		, meta = (DisplayName = "HealthRestore (HP 회복량)", ToolTip = "HP 회복량"))
	float HealthRestore = 0.0f;
	
	// Sanity 회복량
	UPROPERTY(EditAnywhere, BlueprintReadOnly
		, meta = (DisplayName = "SanityRestore (Sanity 회복량)", ToolTip = "Sanity 회복량"))
	float SanityRestore = 0.0f;
};

/*
	건축물 관련 정보
*/
USTRUCT(BlueprintType)
struct FItemBuildingData
{
	GENERATED_BODY()
	
	// 건물 체력
	UPROPERTY(EditAnywhere, BlueprintReadOnly
		, meta=(ClampMin="1"
			,DisplayName = "StructureHP (건물 체력)", ToolTip = "건물 체력"))
	int32 StructureHP = 100;
	
	// 저주 유지비로 소모되는지 여부
	UPROPERTY(EditAnywhere, BlueprintReadOnly
		, meta = (DisplayName = "ConsumedByCurse (건물 유지비 소모 여부)", ToolTip = "건물 유지비 소모 여부"))
	bool bConsumedByCurse = false;
	
	// 저주 유지비로 소모되는 재료 아이템 ID
	UPROPERTY(EditAnywhere, BlueprintReadOnly
		, meta=(EditCondition="bConsumedByCurse", EditConditionHides
			, DisplayName = "ConsumedByCurseID (유지비 소모 재료 아이템의 ID)", ToolTip = "유지비 소모 재료 아이템의 ID"))
	int32 ConsumedByCurseID = 0;
	
	// 저주 유지비 소모 재료의 수량
	UPROPERTY(EditAnywhere, BlueprintReadOnly
		, meta=(EditCondition="bConsumedByCurse", EditConditionHides, ClampMin="1"
			, DisplayName = "CurseCostQuantity (유지비 소모 재료의 수량)", ToolTip = "유지비 소모 재료의 수량"))
	int32 CurseCostQuantity = 1;
	
	// 광원 여부
	UPROPERTY(EditAnywhere, BlueprintReadOnly
		, meta = (DisplayName = "LightSource (광원 여부)", ToolTip = "광원 여부"))
	bool bLightSource = false;
	
	// 빛의 반경
	UPROPERTY(EditAnywhere, BlueprintReadOnly
		, meta = (EditCondition="bLightSource", EditConditionHides, ClampMin="0.0"
			, DisplayName = "LightRadius (빛 반경)", ToolTip = "빛 구역 생성 반경"))
	float LightRadius = 0.0f;

	// 빛의 밝기 또는 강도
	UPROPERTY(EditAnywhere, BlueprintReadOnly
		, meta = (EditCondition="bLightSource", EditConditionHides, ClampMin="0.0"
			, DisplayName = "LightIntensity (빛의 강도/밝기)", ToolTip = "빛의 밝기/색상/품질 등 아트 스타일 정의"))
	float LightIntensity = 0.0f;
};

// 획득 데이터 통합
USTRUCT(BlueprintType)
struct FItemAcquisitionData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EItemSourceType SourceType = EItemSourceType::None;
	
	// 자원 채취 데이터
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="SourceType==EItemSourceType::Harvest", EditConditionHides))
	FItemHarvestData HarvestData;
	
	// 몬스터 드롭 데이터
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="SourceType==EItemSourceType::MonsterDrop", EditConditionHides))
	FItemMonsterDropData MonsterDropData;
	
	// 상자 획득 데이터
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="SourceType==EItemSourceType::Chest", EditConditionHides))
	FItemChestLootData ChestLootData;
	
	// 상호작용 획득 데이터
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="SourceType==EItemSourceType::Interaction", EditConditionHides))
	FItemInteractionData InteractionData;
	
	// 건축물 설치 데이터
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="SourceType==EItemSourceType::Install", EditConditionHides))
	FItemBuildingData BuildingData;
	
	// 크래프팅 제작 데이터 (이 아이템을 만드는 방법)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="SourceType==EItemSourceType::Craft", EditConditionHides))
	FItemCraftingData CraftingData;
};

/*
	아이템 마스터 구조체 (메타데이터)
	기획 명세의 모든 필드 포함
*/
USTRUCT(BlueprintType)
struct FItemMetaData : public FTableRowBase
{
	GENERATED_BODY()
	
	//========================================
	// 기본 정보 (필수)
	//========================================
	
	/*
	   아이템 ID (숫자, 001-999)
	   기획: INT, Primary Key
	   범위: 재료(001-199), 도구(200-299)
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="CoreData",
		meta=(ClampMin="1", ClampMax="999"
		, DisplayName="ItemID (ID)", ToolTip="아이템 ID"))
	int32 ItemID = 0;
	
	// 한글명 (기획: string)
	// 실제 게임 내 표시용 → 로컬라이징 고려하여 FText 사용
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="CoreData"
		, meta = (DisplayName = "Name_KR (한글명)", ToolTip = "한글명"))
	FText Name_KR;

	// 영문명
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="CoreData"
		, meta = (DisplayName = "Name_EN (영문명)", ToolTip = "영문명"))
	FText Name_EN;
	
	// 티어
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="CoreData"
		, meta = (DisplayName = "Tier (티어)", ToolTip = "티어"))
	EMaterialTier Tier = EMaterialTier::T1;
	
	// 아이템 카테고리 (ENUM)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="CoreData"
		, meta = (DisplayName = "Category (카테고리)", ToolTip = "카테고리"))
	EItemCategory Category = EItemCategory::Material;

	// TODO: 기획 확인 필요
	// 소모품 전용 데이터
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Consumable",
	meta=(EditCondition="Category==EItemCategory::Consumable", EditConditionHides,
		DisplayName="ConsumableData (소모품 전용 데이터)",
		ToolTip="소모품 카테고리 전용 회복량/효과 관련 데이터"))
	FItemConsumableData ConsumableData;

	// TODO: 기획 확인 필요
	// 도구 전용 데이터
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Tool",
		meta=(EditCondition="Category==EItemCategory::Tool", EditConditionHides,
			DisplayName="ToolData (도구 전용 데이터)",
			ToolTip="도구 카테고리 전용 데이터 (공격력, 공격속도, 등급 등)"))
	FItemToolData ToolData;
	
	// 내구도 (도구 및 무기만 해당 / 0이면 파괴)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="CoreData"
		, meta=(ClampMin="0", ClampMax="100"
			, EditCondition="Category==EItemCategory::Tool|| Category==EItemCategory::Weapon || Category==EItemCategory::Building"
			, DisplayName = "Durability (내구도/Only-도구,무기,건축물)", ToolTip = "내구도"))
	int32 Durability = 0;
	
	//========================================
	// 기본 정보 (필수) - 인벤토리 관련
	//========================================
	
	// 인벤토리에서의 중첩 가능 여부
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="CoreData"
		, meta = (DisplayName = "Stackable (인벤토리/중첩 가능 여부)", ToolTip = "인벤토리/중첩 가능 여부"))
	bool bStackable = true;
	
	// 최대 스택 수량
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="CoreData"
		, meta=(EditCondition="bStackable", ClampMin="1"
		, DisplayName = "MaxStack (인벤토리/최대 중첩 개수)", ToolTip = "인벤토리/최대 중첩 개수"))
	int32 MaxStack = 99;
	
	//========================================
	// 기본 정보 (필수) - 획득 방식
	//========================================
	
	// 획득 데이터 통합
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="CoreData"
		, meta = (DisplayName = "SourceType (획득 방식)", ToolTip = "획득 방식"))
	FItemAcquisitionData AcquisitionData;
	
	// 획득 가능한 원천 오브젝트 ID
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="CoreData"
		, meta = (DisplayName = "SourceObject_ID (획득 가능한 원천 오브젝트 ID)", ToolTip = "획득 가능한 원천 오브젝트 ID"))
	int32 SourceObject_ID = 0;
	
	//========================================
	// 기본 정보 (필수) - 사용 관련 정보
	//========================================

	// 장착이 필요한지 여부
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="CoreData"
		, meta = (DisplayName = "Equipable (장착 가능 여부)", ToolTip = "장착 가능 여부"))
	bool bEquipable = false;
	
	// 사용 대상 (Self/Other/Field)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="CoreData"
		, meta=(Categories="Item.UseTarget"
			,DisplayName = "UseTargetTag (자신, 타인, 필드)", ToolTip = "자신, 타인, 필드"))
	FGameplayTag UseTargetTag;
	
	// 아이템에 부여되는 특수 기능 태그
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="CoreData"
		, meta = (Categories="Item.Logic"
			, DisplayName = "LogicTags (아이템 발동시키는 특수 기능 명칭)", ToolTip = "아이템 발동시키는 특수 기능 명칭"))
	FGameplayTagContainer LogicTags;

	// 효과 수치 (LogicTag 효과의 수치: HP 회복, Sanity 회복, 대미지 등)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="CoreData"
		, meta = (DisplayName = "EffectValue (LogicTag 효과의 수치)", ToolTip = "LogicTag 효과의 수치"))
	float EffectValue = 0.0f;
	
	//========================================
	// 기본 정보 (필수)
	// Icon: 인벤토리 및 UI 표시용
	// WorldMesh: 월드 드롭 및 프리뷰용 표시용
	//========================================
	
	// 아이템 아이콘
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Visual"
		, meta = (DisplayName = "Icon (아이콘/인벤토리 및 UI 표시용)", ToolTip = "아이콘/인벤토리 및 UI 표시용"))
	TSoftObjectPtr<UTexture2D> Icon;
	
	// 아이템 3D 메시 (월드 드롭용)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Visual"
		, meta = (DisplayName = "WorldMesh (3D 메시/월드 드롭 및 프리뷰용 표시용)", ToolTip = "3D 메시/월드 드롭 및 프리뷰용 표시용"))
	TSoftObjectPtr<UStaticMesh> WorldMesh;

	//========================================
	// 필드 드롭 관련
	// 기획: Timeout (필드 잔여 시간 3분)
	//========================================

	// // 필드에 드롭된 후 잔여 시간 (초) - 기획: 3분 = 180초
	// UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Field"
	// 	, meta = (DisplayName = "FieldDropTimeout (필드 잔여 시간, 초)", ToolTip = "필드 잔여 시간, 초"))
	// float FieldDropTimeout = 180.0f;

	//========================================
	// 기획 비포함, 개발 편의상 추가
	// 오브젝트 풀링 관련
	//========================================

	// // 오브젝트 풀링 사용 여부 (총알 같은 고빈도 아이템)
	// UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Performance"
	// 	, meta = (DisplayName = "UseObjectPooling (오브젝트 풀링 사용 여부/총알 같은 고빈도 아이템)", ToolTip = "오브젝트 풀링 사용 여부/총알 같은 고빈도 아이템"))
	// bool bUseObjectPooling = false;

	// 풀링 오브젝트가 액터인지 여부
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Performance"
		, meta = (DisplayName = "IsActorType (액터 타입 여부)", ToolTip = "해당 오브젝트가 액터 타입인지 여부"))
	bool bIsActorType = false;

	// 액터 타입일 경우 스폰될 블루프린트 클래스
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Performance"
		, meta = (EditCondition="bIsActorType", EditConditionHides
		, DisplayName = "PooledActorClass (풀링 액터 클래스)", ToolTip = "액터 타입일 경우 사용할 블루프린트 클래스 (BP_Actor 등)"))
	TSubclassOf<AActor> PooledActorClass = nullptr;

	//========================================
	// 기획 비포함, 개발 편의상 추가
	// 헬퍼 함수
	//========================================
	
	FORCEINLINE bool IsMaterial() const { return Category == EItemCategory::Material; }
	FORCEINLINE bool IsTool() const { return Category == EItemCategory::Tool; }
	FORCEINLINE bool IsWeapon() const { return Category == EItemCategory::Weapon; }
	FORCEINLINE bool IsBuilding() const { return Category == EItemCategory::Building; }
	FORCEINLINE bool IsConsumable() const { return Category == EItemCategory::Consumable; }
	FORCEINLINE bool IsStory() const { return Category == EItemCategory::Story; }

	FORCEINLINE bool HasDurability() const { return Durability > 0; }
	FORCEINLINE bool IsLightSource() const { return LogicTags.HasTagExact(FGameplayTag::RequestGameplayTag(TEXT("Item.Logic.LightSource"))); }
	FORCEINLINE bool IsConsumedByCurse() const { return LogicTags.HasTagExact(FGameplayTag::RequestGameplayTag(TEXT("Item.Logic.ConsumedByCurse"))); }
	
	// ItemID를 GameplayTag로 변환 (내부 시스템 연동용)
	FGameplayTag GetItemIDAsTag() const
	{
	    FString TagString = FString::Printf(TEXT("Item.ID.%03d"), ItemID);
	    return FGameplayTag::RequestGameplayTag(FName(*TagString), false);
	}
};