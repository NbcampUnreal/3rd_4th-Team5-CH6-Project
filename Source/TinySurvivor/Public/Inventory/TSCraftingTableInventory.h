// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TSInventoryMasterComponent.h"
#include "TSCraftingTableInventory.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TINYSURVIVOR_API UTSCraftingTableInventory : public UTSInventoryMasterComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UTSCraftingTableInventory();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void PostInitProperties() override;

public:
	// 플레이어별 슬롯 매핑
	int32 GetorAssignSlotForPlayer(APlayerController* PC);

	// 플레이어가 UI 닫을 때 호출
	// 아이템 남아있으면 월드에 아이템 드랍 및 플레이어 매핑 해제
	UFUNCTION(BlueprintCallable, Category = "CraftingTable")
	void OnPlayerClosedUI(APlayerController* PC);

	// 제작 결과물 해당 슬롯에 배치
	UFUNCTION(BlueprintCallable, Category = "CraftingTable")
	int32 PlaceCraftResult(APlayerController* PC, int32 ResultItemID, int32 Quantity = 1);

private:
	// 빈 슬롯 찾기 (없으면 -1 반환)
	int32 GetUnusedSlot() const;
	// 플레이어 슬롯 매핑 해제
	void ReleasePlayerSlot(APlayerController* PC);
	
	// 제작대 고정 슬롯 개수 (최대 동시 접속자 수)
	static constexpr int32 MAX_CRAFTING_SLOTS = 4;
	// 플레이어별 슬롯 매핑
	UPROPERTY()
	TMap<APlayerController*, int32> PlayerSlotMap;
	// 슬롯 사용 여부 추적용
	TArray<bool> UsedSlot;
};
