#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "TSCharacter.generated.h"

class UAbilitySystemComponent;
class UTSAttributeSet;
class UCameraComponent;
class USpringArmComponent;
class UInputMappingContext;
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
	
	
	FORCEINLINE USpringArmComponent* GetSpringArmComponent() const { return SpringArmComponent; }
	FORCEINLINE UCameraComponent* GetCameraComponent() const { return CameraComponent; }

#pragma region Camera
	// 카메라 
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Camera" , meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USpringArmComponent> SpringArmComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UCameraComponent> CameraComponent;
#pragma endregion

#pragma region Input
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputMappingContext> DefaultInputMappingContext;

	//wasd
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> MoveAction;
	//look
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> LookAction;
	//jump
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> JumpAction;

#pragma endregion

#pragma region GAS
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAbilitySystemComponent> ASC;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UTSAttributeSet> Attributes;

private:
	void InitAbilitySystem();

	
	//GAS 쓸 목록들
	//shift -> GA_Sprint Input.Sprint
	//c -> GA_Crouch
	//q GA_ShoulderSwitch
	//ctrl GA_CameraSwitch
	//z GA_LyingDown
	//b GA_Build
	//e GA_Interact
	//left click GA_Attack, GA_Harvest
	//right click GA_AimingDownSight, GA_UseItem, GA_Torchlight
	//wheel GA_Ping
#pragma endregion
	
protected:
	virtual void BeginPlay() override;
	void Move(const struct FInputActionValue& Value);
	void Look(const struct FInputActionValue& Value);

public:	
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
