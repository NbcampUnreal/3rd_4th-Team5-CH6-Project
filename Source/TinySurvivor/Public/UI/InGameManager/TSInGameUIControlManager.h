// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "TSInGameUIControlManager.generated.h"

class UTSInGameMasterWidget;
/**
 * UTSInGameUIControlManager 역할
 * UTSInGameUIControlManager 는 로컬 플레이어의 UI 관련 정보를 받아서 관리하는 매니저 클래스입니다.
 * 인 게임에 필요한 모든 UI를 관리하며 쉽게 접근이 가능하도록 합니다.
 * 로컬과 긴밀한 접촉을 위해 HUD 와 소통해야 합니다.
 */
UCLASS()
class TINYSURVIVOR_API UTSInGameUIControlManager : public ULocalPlayerSubsystem
{
	GENERATED_BODY()

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//======================================
	// UTSInGameUIControlManager 외부 API
	//======================================

public:
	// UTSInGameUIControlManager 게터 
	static UTSInGameUIControlManager* Get(const UObject* WorldContextObject);

	// 인 게임 위젯 초기 설정
	void SetInGameMasterWidget(TSubclassOf<UTSInGameMasterWidget> InGameMasterWidgetClass,APlayerController* InPlayerController);
	
	// 최상위 위젯 게터
	UTSInGameMasterWidget* GetInGameMasterWidget();

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// 최상위 위젯 
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UTSInGameMasterWidget> InGameMasterWidgetInstance;
};
