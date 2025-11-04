// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TSInventorySlot.generated.h"

/**
 * 인벤토리 타입 열거형 데이터
 */
UENUM(Blueprintable, BlueprintType)
enum class EInventoryType : uint8
{
	Any			UMETA(DisplayName = "Any"),
	HotKey		UMETA(DisplayName = "HotKey"),
	BackPack	UMETA(DisplayName = "BackPack"),
	Equipment	UMETA(DisplayName = "Equipment"),
	Storage		UMETA(DisplayName = "Storage")
};

/**
 * 슬롯 타입 열거형 데이터
 */
UENUM(Blueprintable, BlueprintType)
enum class ESlotType : uint8
{
	// 공통
	Any				UMETA(DisplayName = "Any"),

	// 소모품 관련
	Consumable		UMETA(DisplayName = "Consumable"),

	// 가방
	BackPack		UMETA(DisplayName = "BackPack"),

	// 신체
	Helmet			UMETA(DisplayName = "Helmet"),
	Hand 			UMETA(DisplayName = "Hand"),
	Chest			UMETA(DisplayName = "Chest"),
	Belt			UMETA(DisplayName = "Belt"),
	Pant			UMETA(DisplayName = "Pant"),
	Boot			UMETA(DisplayName = "Boot"),
};

/**
 *  인벤토리 및 슬롯 관련 데이터 구조체
 */
USTRUCT(Blueprintable, BlueprintType)
struct TINYSURVIVOR_API FItemInventoryAndSlotInfo
{
	GENERATED_BODY()

	// 어떤 인벤토리 타입에 들어갈 수 있는가? (여러 타입 선택 가능)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "InventoryType")
	TArray<EInventoryType> InventoryType = { EInventoryType::Any };

	// 어떤 인벤토리 슬롯 타입에 들어갈 수 있는가? (여러 타입 선택 가능)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = " SlotType")
	TArray<ESlotType> SlotType = { ESlotType::Any };
	
	// 스택이 가능한가?
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CanStack")
	bool bCanStack = false;

	// 스택이 최대 얼마나 가능한가?
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(EditCondition = "bCanStack"), Category = "StackSize")
	int32 StackSize = 0;
	
};
/**
 * 슬롯 정보 데이터
 */
USTRUCT(Blueprintable, BlueprintType)
struct TINYSURVIVOR_API FSlotStructMaster
{
	GENERATED_BODY()
	
	// 슬롯 타입 정보
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SlotType")
	ESlotType SlotType = ESlotType::Any;

	// 스택이 가능한가?
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "bCanStack")
	bool bCanStack = false;
    
	// 스택 현재 사이즈
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CurrentStackSize")
	int32 CurrentStackSize = 0;
    
	// 스택 최대 사이즈
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MaxStackSize")
	int32 MaxStackSize = 1;

	// 아이템 정적데이터 ID
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ItemData")
	FName StaticDataID = "";

	// 아이템 동적데이터 ID
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ItemData")
	FName DynamicDataID = "";
};

/**
 * 인벤토리 정보 데이터
 */
USTRUCT(Blueprintable, BlueprintType)
struct TINYSURVIVOR_API FInventoryStructMaster
{
	GENERATED_BODY()

	// 인벤토리 타입
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "InventoryType")
	EInventoryType InventoryType = EInventoryType::Any;
	
	// 슬롯 컨테이너
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "InventorySlotContainer")
	TArray<FSlotStructMaster> InventorySlotContainer = {};
};