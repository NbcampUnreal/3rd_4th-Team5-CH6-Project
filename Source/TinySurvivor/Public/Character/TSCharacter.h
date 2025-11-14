#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "InputActionValue.h"
#include "TSCharacter.generated.h"

class UAbilitySystemComponent;
class UTSAttributeSet;
class UGameplayAbility;
class UCameraComponent;
class USpringArmComponent;
class UTSPlayerInputDataAsset;
class UInputAction;

UCLASS()
class TINYSURVIVOR_API ATSCharacter : public ACharacter , public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	ATSCharacter();
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UTSAttributeSet* GetAttributeSet() const;
	
	virtual void PossessedBy(AController* NewController) override; //서버빙의시호출
	virtual void OnRep_PlayerState() override; //클라 복제시 호출

#pragma region Camera
	FORCEINLINE USpringArmComponent* GetSpringArmComponent() const { return SpringArmComponent; }
	FORCEINLINE UCameraComponent* GetCameraComponent() const { return CameraComponent; }
	// 카메라 
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Camera" , meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USpringArmComponent> SpringArmComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UCameraComponent> CameraComponent;
	
	//shoulder Switch 설정	
	UPROPERTY(EditAnywhere, Category="ShoulderSwitchCamera")
	float RightShoulderOffset = 40.0f;

	UPROPERTY(EditAnywhere, Category = "ShoulderSwitchCamera")
	float LeftShoulderOffset = -80.0f;
	
	UPROPERTY(EditAnywhere, Category="ShoulderSwitchCamera")
	float ShoulderSwitchDuration = 0.5f; //0.5초 동안 이동

	bool bIsRightShoulder = true; //현재 오른쪽 어깨 카메라인가?
	bool bIsSwitchingShoulder = false; // 전환 중인지
	float ShoulderSwitchElapsed = 0.f; // 경과 시간
	
	FVector ShoulderStartOffset; // 시작 위치
	FVector ShoulderTargetOffset; // 목표 위치
	FVector SpringArmBaseLocation;
	FVector SpringArmRightLocation;
	FVector SpringArmLeftLocation;
#pragma endregion

#pragma region Input
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UTSPlayerInputDataAsset> InputDataAsset;
#pragma endregion
	
#pragma region GAS
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAbilitySystemComponent> ASC;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UTSAttributeSet> Attributes;

private:
	void InitAbilitySystem();
	void InitializeAbilities();
#pragma endregion

protected:
	virtual void BeginPlay() override;

#pragma region Function
	// GAS 안쓰는 함수
	void Move(const struct FInputActionValue& Value);
	void Look(const struct FInputActionValue& Value);
	void ShoulderSwitch(const struct FInputActionValue& Value);

	// GAS 어빌리티 매니저 호출 함수
	void OnJumpOrClimb(const struct FInputActionValue& Value);
	void OnRoll(const struct FInputActionValue& Value);
	void OnCrouch(const struct FInputActionValue& Value);
	void OnSprintStarted(const struct FInputActionValue& Value);
	void OnSprintCompleted(const struct FInputActionValue& Value);
	void OnLyingDown(const struct FInputActionValue& Value);
	
	// Interaction 함수들
	void OnOpenBag(const struct FInputActionValue& Value);
	void OnBuild(const struct FInputActionValue& Value);
	void OnInteract(const struct FInputActionValue& Value);
	void OnLeftClick(const struct FInputActionValue& Value); //얘넨 모르겠다 (한 키에 여러가지 함수?)
	void OnRightClick(const struct FInputActionValue& Value); // 얘넨 모르겠다.
	void OnPing(const struct FInputActionValue& Value);
	void OnWheelScroll(const struct FInputActionValue& Vaule);
	
	//hot key 함수들
	void OnHotKey1(const struct FInputActionValue& Value);
	void OnHotKey2(const struct FInputActionValue& Value);
	void OnHotKey3(const struct FInputActionValue& Value);
	void OnHotKey4(const struct FInputActionValue& Value);
	void OnHotKey5(const struct FInputActionValue& Value);
	void OnHotKey6(const struct FInputActionValue& Value);
	void OnHotKey7(const struct FInputActionValue& Value);
	void OnHotKey8(const struct FInputActionValue& Value);
	void OnHotKey9(const struct FInputActionValue& Value);
	void OnHotKey0(const struct FInputActionValue& Value);
	
#pragma endregion
		
#pragma region LineTrace
private:
	void LineTrace();
	
	UPROPERTY(EditAnywhere,Category = "LineTrace")
	float TraceLength = 500.f; // 상호작용 가능한 최대 거리
	UPROPERTY(EditAnywhere,Category = "LineTrace")
	TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Visibility; //탐지에 사용할 트레이스 채널
	UPROPERTY()
	TWeakObjectPtr<AActor> CurrentHitActor; //현재 라인트레이스에 맞고 있는 액터
	UPROPERTY()
	TWeakObjectPtr<AActor> LastHitActor; //직전 프레임에서 맞고 있던 액터
#pragma endregion
private:
	void SendHotKeyEvent(int HotKeyIndex);
	
public:	
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
