// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "TSInGamePlayerInputDataAsset.generated.h"

class UInputAction;
class UInputMappingContext;
/**
 * 
 */
UCLASS()
class TINYSURVIVOR_API UTSInGamePlayerInputDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	
//======================================================================================================================
#pragma region 기본_움직임
	
	// 기본 IMC 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TS | Input")
	TObjectPtr<UInputMappingContext> DefaultMovementIMC = nullptr;

	// WASD 움직임
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TS | Input")
	TObjectPtr<UInputAction> IA_Move = nullptr;

	// 시야 회전 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TS | Input")
	TObjectPtr<UInputAction> IA_Look = nullptr;

	// 점프
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TS | Input")
	TObjectPtr<UInputAction> IA_Jump = nullptr;

	// 달리기 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TS | Input")
	TObjectPtr<UInputAction> IA_Sprint = nullptr;

	// 걷기
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TS | Input")
	TObjectPtr<UInputAction> IA_Walk = nullptr;
	
#pragma endregion
//======================================================================================================================
#pragma region 핫키
	
	// 핫키 IMC
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TS | Input")
	TObjectPtr<UInputMappingContext> HotKeyIMC = nullptr;

	// 핫키 1번 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TS | Input")
	TObjectPtr<UInputAction> IA_HotKeyOne = nullptr;

	// 핫키 2번 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TS | Input")
	TObjectPtr<UInputAction> IA_HotKeyTwo = nullptr;

	// 핫키 3번 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TS | Input")
	TObjectPtr<UInputAction> IA_HotKeyThree = nullptr;

	// 핫키 4번 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TS | Input")
	TObjectPtr<UInputAction> IA_HotKeyFour = nullptr;

	// 핫키 5번 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TS | Input")
	TObjectPtr<UInputAction> IA_HotKeyFive = nullptr;

	// 핫키 6번 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TS | Input")
	TObjectPtr<UInputAction> IA_HotKeySix = nullptr;

	// 핫키 7번 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TS | Input")
	TObjectPtr<UInputAction> IA_HotKeySeven = nullptr;

	// 핫키 8번 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TS | Input")
	TObjectPtr<UInputAction> IA_HotKeyEight = nullptr;

	// 핫키 9번 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TS | Input")
	TObjectPtr<UInputAction> IA_HotKeyNine = nullptr;
	
#pragma endregion
//======================================================================================================================
#pragma region 상호작용_관련
	
	// 상호작용 IMC
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TS | Input")
	TObjectPtr<UInputMappingContext> InteractIMC = nullptr;

	// F 키 상호작용 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TS | Input")
	TObjectPtr<UInputAction> IA_InteractByNormal = nullptr;

	// 왼쪽 마우스 상호작용 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TS | Input")
	TObjectPtr<UInputAction> IA_InteractByLeftClick = nullptr;
	
#pragma endregion
//======================================================================================================================	
};
