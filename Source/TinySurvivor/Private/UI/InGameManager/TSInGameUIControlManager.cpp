// Fill out your copyright notice in the Description page of Project Settings.


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//======================================
	// UTSInGameUIControlManager 외부 API
	//======================================

#include "UI/InGameManager/TSInGameUIControlManager.h"
#include "UI/InGameMaster/TSInGameMasterWidget.h"

UTSInGameUIControlManager* UTSInGameUIControlManager::Get(const UObject* WorldContextObject)
{
	if (!WorldContextObject) return nullptr;

	UWorld* World = GEngine ? GEngine->GetWorldFromContextObjectChecked(WorldContextObject) : nullptr;
	if (!World) return nullptr;

	APlayerController* PC = World->GetFirstPlayerController();
	if (!PC) return nullptr;

	ULocalPlayer* LocalPlayer = PC->GetLocalPlayer();
	if (!LocalPlayer) return nullptr;

	UTSInGameUIControlManager* Manager = LocalPlayer->GetSubsystem<UTSInGameUIControlManager>();
	if (!Manager) return nullptr;

	return Manager;
}

void UTSInGameUIControlManager::SetInGameMasterWidget(TSubclassOf<UTSInGameMasterWidget> InGameMasterWidgetClass, APlayerController* InPlayerController)
{
	if (!IsValid(InPlayerController) || !IsValid(InGameMasterWidgetClass)) return;

	InGameMasterWidgetInstance = CreateWidget<UTSInGameMasterWidget>(InPlayerController, InGameMasterWidgetClass);
	InGameMasterWidgetInstance->AddToViewport();
}

UTSInGameMasterWidget* UTSInGameUIControlManager::GetInGameMasterWidget()
{
	return InGameMasterWidgetInstance;
}
