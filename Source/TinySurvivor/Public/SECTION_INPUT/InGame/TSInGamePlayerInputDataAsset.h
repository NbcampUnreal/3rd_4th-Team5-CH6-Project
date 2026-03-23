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
	
	// 기본 IMC 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TS | Input")
	UInputMappingContext* DefaultMovementIMC;

	// WASD 움직임
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TS | Input")
	UInputAction* IA_Move;

	// 시야 회전 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TS | Input")
	UInputAction* IA_Look;

	// 점프
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TS | Input")
	UInputAction* IA_Jump;

	// 달리기 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TS | Input")
	UInputAction* IA_Sprint;

	// 걷기
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TS | Input")
	UInputAction* IA_Walk;
	
};
