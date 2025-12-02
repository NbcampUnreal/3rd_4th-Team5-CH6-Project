#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "TSPlayerInputDataAsset.generated.h"

class UInputMappingContext;
class UInputAction;

UCLASS()
class TINYSURVIVOR_API UTSPlayerInputDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	#pragma region Input
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputMappingContext> DefaultInputMappingContext;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputMappingContext> HotKeyInputMappingContext;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputMappingContext> InteractionInputMappingContext;

	///////////// DefaultInputMappingContext /////////////
	//wasd
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> MoveAction;
	//look
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> LookAction;
	// ShoulderSwitch
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> ShoulderSwitchAction;
	// -- 여기까지 GAS 안씀 --

	
	//-- 여기서부턴 GAS 씀 --
	//jump and climb  
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> JumpOrClimbAction;
	//roll
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> RollAction;
	//crouch
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> CrouchAction;
	//sprint
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> SprintAction;
	//lyingdown 삭제


	///////////// InteractionInputMappingContext /////////////
	//Open Bag -> 얘는 가스 안씀 (그냥 가방 열기 닫기 토글)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> OpenBagAction;
	//bulid
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> BuildAction;
	//interact
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> InteractAction;
	//ping
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> PingAction;
	//wheel Scroll
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> WheelScrollAction;
	//LeftClickAction (Attack, Harvest,)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> LeftClickAction;
	//RightClickAction (ADS, TorchLight, Item,)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> RightClickAction;
	//Esc 열려있는 위젯 닫기, 설정창 열기
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> EscAction;
	
	///////////// hotkeyInputMappingContext /////////////
	//HotKey 1 9
	//1
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> HotKey1Action;
	//2
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> HotKey2Action;
	//3
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> HotKey3Action;
	//4
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> HotKey4Action;
	//5
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> HotKey5Action;
	//6
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> HotKey6Action;
	//7
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> HotKey7Action;
	//8
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> HotKey8Action;
	//9
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> HotKey9Action;
	//0
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> HotKey0Action;
	
	// 테스트 코드
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> DebugDropAction;   // 키보드 'J'
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> DebugRemoveAction; // 키보드 'K'
	//
#pragma endregion
};
