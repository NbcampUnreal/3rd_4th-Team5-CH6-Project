// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "Components/ActorComponent.h"
#include "TSPlayerInputActionComponent.generated.h"

class UTSInGamePlayerInputDataAsset;
class UEnhancedInputComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TINYSURVIVOR_API UTSPlayerInputActionComponent : public UActorComponent
{
	GENERATED_BODY()

//======================================================================================================================	
#pragma region 라이프_사이클
	
	
	//━━━━━━━━━━━━━━━━━━━━
	// 라이프 사이클
	//━━━━━━━━━━━━━━━━━━━━	
	
public:
	UTSPlayerInputActionComponent();

#pragma endregion
//======================================================================================================================	
#pragma region 외부_API
	
	
	//━━━━━━━━━━━━━━━━━━━━
	// 외부_API
	//━━━━━━━━━━━━━━━━━━━━	
	
public:
	// 캐릭터로부터 호출받아 입력을 바인딩할 함수
	void SetupPlayerInput(UEnhancedInputComponent* InEnhancedInputComponent);

#pragma endregion
//======================================================================================================================	
#pragma region 움직임_관련
	
	
	//━━━━━━━━━━━━━━━━━━━━
	// 움직임_관련
	//━━━━━━━━━━━━━━━━━━━━	
	
protected:
	// 실제 이동 로직 (Callback)
	void Move_internal(const FInputActionValue& Value);
	void Look_internal(const FInputActionValue& Value);
	void Jump_internal(const FInputActionValue& Value);
	void Sprint_internal(const FInputActionValue& Value);
	void Walk_internal(const FInputActionValue& Value);
	
#pragma endregion
//======================================================================================================================	
#pragma region 데이터
	
	
	//━━━━━━━━━━━━━━━━━━━━
	// 데이터
	//━━━━━━━━━━━━━━━━━━━━	
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TS | Input")
	TObjectPtr<UTSInGamePlayerInputDataAsset> InputData = nullptr;
	
	// 오너 캐싱 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TS | Input")
	TObjectPtr<ACharacter> OwnerCharacter = nullptr;
	
#pragma endregion
};
