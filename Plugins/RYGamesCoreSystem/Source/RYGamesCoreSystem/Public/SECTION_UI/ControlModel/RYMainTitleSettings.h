// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Engine/DeveloperSettings.h"
#include "RYMainTitleSettings.generated.h"

/**
 * 
 */
UCLASS(Config = Game, DefaultConfig, meta=(DisplayName="RY Main Title Settings"))
class RYGAMESCORESYSTEM_API URYMainTitleSettings : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:
	// 새 게임 생성용 서브 위젯 
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category="RY | Core | Main | SubWidget", DisplayName="새 게임 생성용 서브 위젯")
	TSubclassOf<UUserWidget> NewGameWidgetClass = nullptr;
	
	// 세이브 데이터 로드 요청 서브 위젯
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category="RY | Core | Main | SubWidget", DisplayName="세이브 데이터 로드 요청 서브 위젯")
	TSubclassOf<UUserWidget> LoadGameWidgetClass = nullptr;
	
	// 세이브 데이터 삭제 요청 서브 위젯
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category="RY | Core | Main | SubWidget", DisplayName="세이브 데이터 삭제 요청 서브 위젯")
	TSubclassOf<UUserWidget> DeleteSaveWidgetClass = nullptr;
	
	// 옵션 설정 요청 서브 위젯
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category="RY | Core | Main | SubWidget", DisplayName="옵션 설정 요청 서브 위젯")
	TSubclassOf<UUserWidget> OptionsWidgetClass = nullptr;
	
	// 옵션 전체 리셋 요청 서브 위젯
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category="RY | Core | Main | SubWidget", DisplayName="옵션 전체 리셋 요청 서브 위젯")
	TSubclassOf<UUserWidget> ResetALLOptionsWidgetClass = nullptr;
	
	// 그래픽 옵션 리셋 요청 서브 위젯
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category="RY | Core | Main | SubWidget", DisplayName="그래픽 옵션 리셋 요청 서브 위젯")
	TSubclassOf<UUserWidget> ResetGraphicWidgetClass = nullptr;
	
	// 사운드 옵션 리셋 요청 서브 위젯
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category="RY | Core | Main | SubWidget", DisplayName="사운드 옵션 리셋 요청 서브 위젯")
	TSubclassOf<UUserWidget> ResetSoundWidgetClass = nullptr;
	
	// 컨트롤 옵션 리셋 요청 서브 위젯
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category="RY | Core | Main | SubWidget", DisplayName="컨트롤 옵션 리셋 요청 서브 위젯")
	TSubclassOf<UUserWidget> ResetControlWidgetClass = nullptr;
	
	// 게임플레이 옵션 리셋 요청 서브 위젯
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category="RY | Core | Main | SubWidget", DisplayName="게임플레이 옵션 리셋 요청 서브 위젯")
	TSubclassOf<UUserWidget> ResetGameplayWidgetClass = nullptr;
	
	// 게임 종료 요청 서브 위젯 
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category="RY | Core | Main | SubWidget", DisplayName="게임 종료 요청 서브 위젯")
	TSubclassOf<UUserWidget> QuitGameWidgetClass = nullptr;
	
	// 스트리머 모드 확인 요청 서브 위젯 
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category="RY | Core | Main | SubWidget", DisplayName="스트리머 모드 확인 요청 서브 위젯")
	TSubclassOf<UUserWidget> StreamerModeWidgetClass = nullptr;
	
	// 세션 생성 서브 위젯 
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category="RY | Core | Main | SubWidget", DisplayName="세션 생성 서브 위젯")
	TSubclassOf<UUserWidget> CreateSessionWidgetClass = nullptr;
	
	// 세션 생성 실패 알림 서브 위젯 
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category="RY | Core | Main | SubWidget", DisplayName="세션 생성 실패 알림 서브 위젯")
	TSubclassOf<UUserWidget> CreateSessionFailWidgetClass = nullptr;
	
	// 세션 조인 서브 위젯 
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category="RY | Core | Main | SubWidget", DisplayName="세션 조인 서브 위젯")
	TSubclassOf<UUserWidget> JoinSessionWidgetClass = nullptr;
	
	// 세션 조인 실패 알림 서브 위젯 
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category="RY | Core | Main | SubWidget", DisplayName="세션 조인 실패 알림 서브 위젯")
	TSubclassOf<UUserWidget> JoinSessionFailWidgetClass = nullptr;
};
