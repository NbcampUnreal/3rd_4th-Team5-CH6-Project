// All CopyRight From YulRyongGameStudio //


#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "InputActionValue.h"
#include "A_FOR_INGAME/SECTION_ITEM/Item/Interface/IInteraction.h"
#include "A_FOR_INGAME/SECTION_ITEM/Item/Data/Common/ItemCommonEnums.h"
#include "TSCharacter.generated.h"

#pragma region 전방선언
class UTSInventoryGASControlComponent;
class UTSEqInvControlComponent;
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
#pragma endregion

#pragma region 델리게이트선언
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnReticleInteractionBegin);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnReticleInteractionEnd);
#pragma endregion

UCLASS()
class TINYSURVIVOR_API ATSCharacter : public ACharacter , public IAbilitySystemInterface, public IIInteraction
{
	GENERATED_BODY()

//----------------------------------------------------------------------------------------------------------------------
	
public:
	
	UPROPERTY(BlueprintAssignable, Category = "Delegate")
	FOnReticleInteractionBegin OnReticleInteractionBegin;
	UPROPERTY(BlueprintAssignable, Category = "Delegate")
	FOnReticleInteractionEnd OnReticleInteractionEnd;
	
//----------------------------------------------------------------------------------------------------------------------
	
public:
	
	virtual void OnRep_PlayerState() override; 
	UFUNCTION() void OnRep_IsDownedState();
	UFUNCTION() void OnRep_IsDeadState();

//----------------------------------------------------------------------------------------------------------------------


public:
	ATSCharacter();
	virtual void PostInitializeComponents() override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override; 
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Tick(float DeltaTime) override;
	
//----------------------------------------------------------------------------------------------------------------------

	
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
//----------------------------------------------------------------------------------------------------------------------

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UTSPlayerInputDataAsset> InputDataAsset;

	

//----------------------------------------------------------------------------------------------------------------------
	
	
private:

	void InitAbilitySystem();
	void InitializeAbilities();

public:

		
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UTSAttributeSet* GetAttributeSet() const;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAbilitySystemComponent> ASC;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UTSAttributeSet> Attributes;

	

//----------------------------------------------------------------------------------------------------------------------
	
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Animation")
	EItemAnimType GetAnimType() const	{return AnimType;}
	
	void SetAnimType(EItemAnimType ItemAnimType) { this->AnimType = ItemAnimType;}

	UPROPERTY(Replicated, EditAnywhere, Category = "Animation")
	EItemAnimType AnimType = EItemAnimType::AXE;

	
	virtual void BecomeDowned();
	
	UFUNCTION(BlueprintCallable, Category = "State")
	bool IsDowned() const;
	
	virtual void Die();
	
	UFUNCTION(BlueprintCallable, Category = "State")
	bool IsDead() const; 
	
	UPROPERTY(ReplicatedUsing = OnRep_IsDownedState, BlueprintReadOnly, Category = "State")
	bool bIsDownedState = false; 
	
	UPROPERTY(ReplicatedUsing = OnRep_IsDeadState, BlueprintReadOnly, Category = "State")
	bool bIsDeadState ;
	
	
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
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Downed")
	TSubclassOf<UGameplayEffect> ProneMoveSpeedEffectClass; // 기절 시 MoveSpeed 200 설정
	
	FTimerHandle LightCheckTimerHandle;

	UFUNCTION()
	void CheckInLightSource();
	

protected:

public:
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Crouch")
	bool bIsCrouching = false;
	
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

public:
	
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Climb")
	FVector CurrentWallNormal = FVector::ZeroVector;
	
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Climb")
	bool bIsClimbState = false;
	
	UFUNCTION(BlueprintCallable, Category = "Climb")
	bool IsClimbing();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly ,Category = "Ping")
	TSubclassOf<AActor> PingActorClass;
	
	UFUNCTION(Server, Reliable) 
	void ServerSpawnPing(ETSPingType PingType, FVector Location);
	
	UFUNCTION(Server, Reliable) 
	void ServerPlayEmote(ETSEmoteType EmoteType);
	
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
	bool bIsInteracting = false;
	
private:
	UFUNCTION(Server, Reliable, WithValidation) void ServerSendHotKeyEvent(int HotKeyIndex);
	UFUNCTION(Server, Reliable,WithValidation)  void ServerSendUseItemEvent();
	UFUNCTION(Server, Reliable,WithValidation)  void ServerInteract(AActor* TargetActor);
	UFUNCTION(Server, Reliable) void ServerSendStopInteractEvent();
	
public:	
	
	// 소모품 사용 몽타주 재생 (Multicast)
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayConsumeMontage(UAnimMontage* Montage, float PlayRate, float ServerStartTime);
	
	// 소모품 사용 몽타주 정지 (Multicast)
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_StopConsumeMontage(UAnimMontage* Montage);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	TObjectPtr<UAnimMontage> PickUpMontage;
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayPickUpMontage();
	
	UFUNCTION(BlueprintImplementableEvent) void ShowPlayerName(const FString& Name);
	
	
	
	
	
	
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
	
	
	
	// 관리용 컴포넌트 (_리팩토링 용_)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UTSEqInvControlComponent> EqInvControlComponent = nullptr;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UTSInventoryGASControlComponent> InventoryGasControlComponent = nullptr;
};
