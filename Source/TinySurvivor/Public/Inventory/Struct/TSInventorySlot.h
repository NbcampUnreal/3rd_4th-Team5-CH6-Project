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
	Any UMETA(DisplayName = "Any"),
	HotKey UMETA(DisplayName = "HotKey"),
	BackPack UMETA(DisplayName = "BackPack"),
	Equipment UMETA(DisplayName = "Equipment"),
};

/**
 * 슬롯 타입 열거형 데이터
 */
UENUM(Blueprintable, BlueprintType)
enum class ESlotType : uint8
{
	// 공통
	Any UMETA(DisplayName = "Any"),
	
	// 방어구
	Head UMETA(DisplayName = "Head"),
	Torso UMETA(DisplayName = "Torso"),
	Leg UMETA(DisplayName = "Leg")
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
	int32 StaticDataID = INDEX_NONE;

	// 아이템 동적데이터 ID
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ItemData")
	int32 DynamicDataID = INDEX_NONE;
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
