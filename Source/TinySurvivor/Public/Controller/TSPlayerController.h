#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TSPlayerController.generated.h"

class ATSCharacter;

UCLASS()
class TINYSURVIVOR_API ATSPlayerController : public APlayerController
{
	GENERATED_BODY()
protected:

	virtual void AcknowledgePossession(class APawn* P) override;
	virtual void OnUnPossess() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> HUDWidgetClass;
	
	UPROPERTY(BlueprintReadOnly, Category = "UI")
	UUserWidget* HUDWidget;
	
	bool IsUpdated = false;

public:
	UFUNCTION(BlueprintCallable)
	void InitializePlayerHUD();

	UFUNCTION(BlueprintCallable)
	void UpdateHUDWithCharacter(ATSCharacter* TSCharacter);
};
