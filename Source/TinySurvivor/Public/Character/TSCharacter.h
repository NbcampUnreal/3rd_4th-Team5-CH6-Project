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
	
private:
	void SendHotKeyEvent(int HotKeyIndex);
	
public:	
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
