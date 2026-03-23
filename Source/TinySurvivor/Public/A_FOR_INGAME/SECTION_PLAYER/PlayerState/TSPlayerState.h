// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "TSPlayerState.generated.h"

class UTSAttributeSet;

UCLASS()
class TINYSURVIVOR_API ATSPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

//======================================================================================================================	
#pragma region 라이프_사이클
	
	//━━━━━━━━━━━━━━━━━━━━
	// 라이프 사이클
	//━━━━━━━━━━━━━━━━━━━━	
	
public:
	ATSPlayerState();
	
#pragma endregion
//======================================================================================================================	
#pragma region GAS
	//━━━━━━━━━━━━━━━━━━━━
	// GAS
	//━━━━━━━━━━━━━━━━━━━━

protected:
	// IAbilitySystemInterface ~ 
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;;
	// ~ IAbilitySystemInterface	
	
protected:
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = " TS | Player")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = " TS | Player")
	TObjectPtr<UTSAttributeSet> AttributeSet = nullptr;
	
#pragma endregion
//======================================================================================================================	
	
};

