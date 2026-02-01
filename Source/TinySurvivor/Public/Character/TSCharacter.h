// TSCharacter.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "InputActionValue.h"
#include "Item/Data/Common/ItemCommonEnums.h"
#include "Item/Interface/IInteraction.h"
#include "TSCharacter.generated.h"

class UHitComponent;
class UFootstepComponent;
class UTSBuildingComponent;
class UTSInventoryMasterComponent;
enum class EItemAnimType : uint8;
class UAbilitySystemComponent;
class UTSAttributeSet;
class UGameplayAbility;
class UCameraComponent;
class USpringArmComponent;
class UWidgetComponent;
class UTSPlayerInputDataAsset;
class UInputAction;
class AErosionLightSourceSubActor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnReticleInteractionBegin);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnReticleInteractionEnd);

UCLASS()
class TINYSURVIVOR_API ATSCharacter : public ACharacter , public IAbilitySystemInterface, public IIInteraction
{
	GENERATED_BODY()
	
public:
	ATSCharacter();
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UTSAttributeSet* GetAttributeSet() const;
	
	virtual void PossessedBy(AController* NewController) override; 
	virtual void OnRep_PlayerState() override; 

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
	
	bool bIsRightShoulder = true; 
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

#pragma region Animation
public:
	UPROPERTY(Replicated, EditAnywhere, Category = "Animation")
	EItemAnimType AnimType = EItemAnimType::AXE;
	
	UFUNCTION(BlueprintPure, Category = "Animation")
	EItemAnimType GetAnimType() const	{
		return AnimType;
	}
	
	void SetAnimType(EItemAnimType ItemAnimType)
	{
		this->AnimType = ItemAnimType;
	}
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
#pragma endregion
	
#pragma region Downed & Dead & Revive
	// Downed 
	virtual void BecomeDowned();
	UFUNCTION(BlueprintCallable, Category = "State")
	bool IsDowned() const;
	UPROPERTY(ReplicatedUsing = OnRep_IsDownedState, BlueprintReadOnly, Category = "State")
	bool bIsDownedState = false; 
	UFUNCTION()
	void OnRep_IsDownedState();
	// Dead 
	virtual void Die();
	
	UFUNCTION(BlueprintCallable, Category = "State")
	bool IsDead() const; 
	
	UPROPERTY(ReplicatedUsing = OnRep_IsDeadState, BlueprintReadOnly, Category = "State")
	bool bIsDeadState ;
	
	UFUNCTION()
	void OnRep_IsDeadState();
	// Revive
	ATSCharacter* DetectReviveTarget();
	
	virtual void Revive();
	
	UFUNCTION(BlueprintCallable, Category = "State")
	bool IsRescueCharacter() const;
	
	UFUNCTION(Server, Reliable)
	void ServerStartRevive(ATSCharacter* Target);
	
	UFUNCTION(Server, Reliable)
	void ServerStopRevive();
	
	UFUNCTION(Client, Reliable)
	void ClientForceStopRevive();
	
	void OnReviveFinished();
	
	void TickReviveValidation();
	
	UPROPERTY(Replicated)
	ATSCharacter* ReviveTargetCharacter;
	
	UPROPERTY(ReplicatedUsing = OnRep_IsRescuing,BlueprintReadOnly, Category = "State")
	bool bIsRescuing;
	
	UFUNCTION()
	void OnRep_IsRescuing();
	
	FTimerHandle ReviveTimerHandle;
	
	UPROPERTY()
	float CurrentReviveTime = 0.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Revive")
	float ReviveDuration = 5.0f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Revive")
	float MaxReviveDistance = 250.0f;
	
	// ------------------ GE ------------------
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Downed")
	TSubclassOf<UGameplayEffect> ProneMoveSpeedEffectClass; // 기절 시 MoveSpeed 200 설정
#pragma endregion
#pragma region Sanity
	
	FTimerHandle LightCheckTimerHandle;
	UFUNCTION()
	void CheckInLightSource();
	
#pragma endregion

protected:
	virtual void BeginPlay() override;

#pragma region Crouch
public:
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Crouch")
	bool bIsCrouching = false;
#pragma endregion
	
#pragma region SurvivalAttribute
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Survival")
	TSubclassOf<UGameplayEffect> StaminaIncreaseEffectClass; // 스태미나 자연 회복용 GE
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Survival")
	TSubclassOf<UGameplayEffect> ThirstDecayEffectClass; // 갈증 자연 감소용 -> 20초마다 -1
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Survival")
	TSubclassOf<UGameplayEffect> HungerDecayEffectClass; // 배고픔 자연 감소용 -> 30초마다 -1 
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Survival")
	TSubclassOf<UGameplayEffect> HungerSpeedEffectClass; // hunger 0 -> 속도 * 0.9
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Survival")
	TSubclassOf<UGameplayEffect> HungerHealthEffectClass; // hunger 0 -> 체력 20초당 -1
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Survival")
	TSubclassOf<UGameplayEffect> TempHotEffectClass;   // 더위 상태이상 -> 최대 스태미나 * 0.5

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Survival")
	TSubclassOf<UGameplayEffect> TempColdEffectClass; // 추위 상태이상 -> 속도 * 0.5 
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Survival")
	TSubclassOf<UGameplayEffect> LightEffectClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Survival")
	TSubclassOf<UGameplayEffect> DarkEffectClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Survival")
	TSubclassOf<UGameplayEffect> FallDamageEffectClass; // 낙하데미지 GE
	
	
	
public:
	// 태그 적용 이펙트
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GETag")
	TSubclassOf<UGameplayEffect> DownedTagEffectClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GETag")
	TSubclassOf<UGameplayEffect> DeadTagEffectClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GETag")
	TSubclassOf<UGameplayEffect> RescuingTagEffectClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GETag")
	TSubclassOf<UGameplayEffect> InLightTagEffectClass;
	
	// AS 태그 적용 이벤트
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GETag")
	TSubclassOf<UGameplayEffect> ThirstTagEffectClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GETag")
	TSubclassOf<UGameplayEffect> HungerTagEffectClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GETag")
	TSubclassOf<UGameplayEffect> FullTagEffectClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GETag")
	TSubclassOf<UGameplayEffect> TempHotTagEffectClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GETag")
	TSubclassOf<UGameplayEffect> TempColdTagEffectClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GETag")
	TSubclassOf<UGameplayEffect> AnxietyTagEffectClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GETag")
	TSubclassOf<UGameplayEffect> PanicTagEffectClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GETag")
	TSubclassOf<UGameplayEffect> SanityBlockTagEffectClass;
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Survival")
	TSubclassOf<UGameplayEffect> FullRecoverHealthEffectClass; // Full 시 health recover 하는 GE
protected:
#pragma endregion
#pragma region Function
	// Move
	void Move(const struct FInputActionValue& Value);
	void Look(const struct FInputActionValue& Value);
	void ShoulderSwitch(const struct FInputActionValue& Value);
	void OnJumpOrClimbStarted(const struct FInputActionValue& Value);
	void OnJumpOrClimbCompleted(const struct FInputActionValue& Value);
	void OnRoll(const struct FInputActionValue& Value);
	void OnCrouch(const struct FInputActionValue& Value);
	void OnSprintStarted(const struct FInputActionValue& Value);
	void OnSprintCompleted(const struct FInputActionValue& Value);
	
	// Interaction
	void OnOpenBag(const struct FInputActionValue& Value);
	void OnBuild(const struct FInputActionValue& Value);
	void OnInteract(const struct FInputActionValue& Value);
	void OnStopInteract(const struct FInputActionValue& Value);
	void OnLeftClick(const struct FInputActionValue& Value); 
	void OnRightClick(const struct FInputActionValue& Value);
	void OnPingStarted(const struct FInputActionValue& Value);
	void OnPingCompleted(const struct FInputActionValue& Value);
	void OnEmoteStarted(const struct FInputActionValue& Value);
	void OnEmoteCompleted(const struct FInputActionValue& Value);
	void OnWheelScroll(const struct FInputActionValue& Vaule);
	void OnEsc(const struct FInputActionValue& Vaule);
	
	//hot key
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
	
	virtual void OnMoveSpeedChanged(const FOnAttributeChangeData& Data);
	virtual void Landed(const FHitResult& Hit) override; //낙하 감지 함수
#pragma endregion
		
#pragma region LineTrace
private:
	void LineTrace();
	
	UPROPERTY(EditAnywhere,Category = "LineTrace")
	float TraceLength = 500.f;
	UPROPERTY(EditAnywhere,Category = "LineTrace")
	TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Visibility;
	UPROPERTY()
	TWeakObjectPtr<AActor> CurrentHitActor;
	UPROPERTY()
	TWeakObjectPtr<AActor> LastHitActor;
#pragma endregion

#pragma region Climb
public:
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Climb")
	FVector CurrentWallNormal = FVector::ZeroVector;
	
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Climb")
	bool bIsClimbState = false;
	
	UFUNCTION(BlueprintCallable, Category = "Climb")
	bool IsClimbing();
#pragma endregion
	
#pragma region Ping
	UPROPERTY(EditAnywhere, BlueprintReadOnly ,Category = "Ping")
	TSubclassOf<AActor> PingActorClass;
	
	UFUNCTION(Server, Reliable) 
	void ServerSpawnPing(ETSPingType PingType, FVector Location);
	
	
#pragma endregion
	
#pragma region Emote
	UFUNCTION(Server, Reliable) 
	void ServerPlayEmote(ETSEmoteType EmoteType);
#pragma endregion
	
#pragma region Component
	// 인벤토리 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UTSInventoryMasterComponent> InventoryMasterComponent;
	// 빌딩 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UTSBuildingComponent> BuildingComponent;
	// Footstep 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UFootstepComponent> FootstepComponent;
	// Hit 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UHitComponent> HitComponent;
	// 닉네임을 위한 위젯 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UWidgetComponent> PlayerNameComponent;
#pragma endregion
	
#pragma region Delegate
	UPROPERTY(BlueprintAssignable, Category = "Delegate")
	FOnReticleInteractionBegin OnReticleInteractionBegin;
	UPROPERTY(BlueprintAssignable, Category = "Delegate")
	FOnReticleInteractionEnd OnReticleInteractionEnd;
#pragma endregion
	
#pragma region Interaction
	virtual void ShowInteractionWidget(ATSCharacter* InstigatorCharacter) override;
	virtual void HideInteractionWidget() override;
	virtual void SetInteractionText(FText WidgetText) override;
	virtual bool CanInteract(ATSCharacter* InstigatorCharacter) override;
	virtual void Interact(ATSCharacter* InstigatorCharacter) override;
	virtual bool RunOnServer() override;
	
protected:
	// 상호작용 위젯
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction|Widget")
	TObjectPtr<UWidgetComponent> InteractionWidget;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction|Widget")
	TSubclassOf<UUserWidget> InteractionWidgetClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction|Widget")
	FText InteractionText = FText::FromString(TEXT("살리기"));

private:
	// 현재 상호작용 중인지
	UPROPERTY()
	bool bIsInteracting = false;
#pragma endregion
	
private:
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSendHotKeyEvent(int HotKeyIndex);
	UFUNCTION(Server, Reliable,WithValidation)
	void ServerSendUseItemEvent();
	UFUNCTION(Server, Reliable,WithValidation)
	void ServerInteract(AActor* TargetActor);
	UFUNCTION(Server, Reliable)
	void ServerSendStopInteractEvent();
	
public:	
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
#pragma region Multicast_ConsumeMontage
public:
	// 소모품 사용 몽타주 재생 (Multicast)
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayConsumeMontage(UAnimMontage* Montage, float PlayRate, float ServerStartTime);
	
	// 소모품 사용 몽타주 정지 (Multicast)
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_StopConsumeMontage(UAnimMontage* Montage);
#pragma endregion
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	TObjectPtr<UAnimMontage> PickUpMontage;
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayPickUpMontage();
	
#pragma region PlayerName
	UFUNCTION(BlueprintImplementableEvent)
	void ShowPlayerName(const FString& Name);
#pragma endregion
};
