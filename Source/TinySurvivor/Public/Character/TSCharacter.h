// TSCharacter.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "InputActionValue.h"
#include "Item/Data/Common/ItemCommonEnums.h"
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
class UTSPlayerInputDataAsset;
class UInputAction;
class AErosionLightSourceSubActor;

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
	// ------------------------------------------------
	// ------------------ Downed 상태 ------------------
	// ------------------------------------------------
	virtual void BecomeDowned(); // Health<=0 일 때 캐릭터 Downed 상태로 만드는 함수
	
	UFUNCTION(BlueprintCallable, Category = "State")
	bool IsDowned() const; // 현재 캐릭터가 Downed 상태인지 확인 (태그로 확인)
	
	UPROPERTY(ReplicatedUsing = OnRep_IsDownedState, BlueprintReadOnly, Category = "State")
	bool bIsDownedState = false; // 기절 상태 클라 동기화 함수 
	
	UFUNCTION()
	void OnRep_IsDownedState();
	// ----------------------------------------------
	// ------------------ Dead 상태 ------------------
	// ----------------------------------------------
	virtual void Die(); // Down 상태에서 DownedHealth 0 되면 호출
	
	UFUNCTION(BlueprintCallable, Category = "State")
	bool IsDead() const; // 현재 캐릭터가 Dead 상태인지 확인 (살릴 수 있나 없나) 
	
	UPROPERTY(ReplicatedUsing = OnRep_IsDeadState, BlueprintReadOnly, Category = "State")
	bool bIsDeadState ; // 클라로 Dead 여부 동기화
	
	UFUNCTION()
	void OnRep_IsDeadState(); // 클라에서 Dead true면 호출 (죽는 모션 동기화)
	
	// ------------------------------------------------
	// ------------------ Revive 상태 ------------------
	// ------------------------------------------------
	ATSCharacter* DetectReviveTarget(); // Target 감지
	
	virtual void Revive(); // Downed 캐릭터를 살려주는 함수
	
	UFUNCTION(BlueprintCallable, Category = "State")
	bool IsRescueCharacter() const; // 내가 지금 친구를 살려주고 있는가
	
	UFUNCTION(Server, Reliable)
	void ServerStartRevive(ATSCharacter* Target); // 소생 시작 요청
	
	UFUNCTION(Server, Reliable)
	void ServerStopRevive(); // 소생 중단 요청
	
	UFUNCTION(Client, Reliable)
	void ClientForceStopRevive(); // 서버에서 강제로 소생 중당 -> 클라 알림
	
	void OnReviveFinished(); // 서버 타이머가 ReviveDuration 만큼 지난 후 호출되는 콜백함수
	
	void TickReviveValidation(); // 죽었는지, Downed이 아닌지, 거리 멀진 않은지 확인
	
	UPROPERTY(Replicated)
	ATSCharacter* ReviveTargetCharacter; // 소생 중인 대상 (기절된 친구)
	
	UPROPERTY(ReplicatedUsing = OnRep_IsRescuing,BlueprintReadOnly, Category = "State")
	bool bIsRescuing; // 기절한 친구를 살려주고 있는지
	
	UFUNCTION()
	void OnRep_IsRescuing(); // bIsRescuing 이 변경될 때마다 클라에서 이동 모드 동기화하는 함수
	
	FTimerHandle ReviveTimerHandle; // 5초 Revive 타이머 핸들
	
	UPROPERTY()
	float CurrentReviveTime = 0.f; // 소생 진행 시간 (현재 진행 시간 누적 값)
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Revive")
	float ReviveDuration = 5.0f; // 소생 소요시간 5초
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Revive")
	float MaxReviveDistance = 250.0f; // 소생 유지 최대 거리 (친구가 기어가서 더 멀어지면 소생 끊김)
	
	
	// ------------------ GE ------------------
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Downed")
	TSubclassOf<UGameplayEffect> ProneMoveSpeedEffectClass; // 기절 시 MoveSpeed 200 설정
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Downed")
	TSubclassOf<UGameplayEffect> DownedEffectClass; // 기절 시 DownedHealth 1초당 -5씩 깎는 GE

#pragma endregion
#pragma region Sanity
	
	FTimerHandle LightCheckTimerHandle;
	UFUNCTION()
	void CheckInLightSource();
	
#pragma endregion

protected:
	virtual void BeginPlay() override;

#pragma region Crouch
	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override; 
	
	void UpdateCrouchCamera(); // 매 프레임 카메라 위치 갱신
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Crouch")
	float StandCameraZ = 0.f; // 서있을때 카메라 높이 보정
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Crouch")
	float CrouchCameraZ = 0.f; // 앉았을때 카메라 높이 보정
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Crouch")
	FName CrouchCurveName = "CrouchCameraAlpha"; //애니메이션 커브 이름
	
	float OriginalCrouchHeightAdjust = 0.f; //캡슐 높이 변화량
	FVector BaseSpringArmSocketOffset = FVector::ZeroVector; //초기 스프링암 오프셋
	
	FTimerHandle CrouchTimerHandle;
	void UnlockCrouchToggle(); //잠금 해제 함수
	bool bCrouchToggleLocked = false;
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
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Survival")
	TSubclassOf<UGameplayEffect> FullRecoverHealthEffectClass; // Full 시 health recover 하는 GE
protected:
	// test -> 체온 상태이상에 따라 GE 적용 되는지 보기 위한 GE 적용 테스트 코드
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Survival")
	TSubclassOf<UGameplayEffect> TempTESTClass;
	
#pragma endregion
#pragma region Function
	// GAS 안쓰는 함수
	void Move(const struct FInputActionValue& Value);
	void Look(const struct FInputActionValue& Value);
	void ShoulderSwitch(const struct FInputActionValue& Value);

	// GAS 어빌리티 매니저 호출 함수
	void OnJumpOrClimbStarted(const struct FInputActionValue& Value);
	void OnJumpOrClimbCompleted(const struct FInputActionValue& Value);
	void OnRoll(const struct FInputActionValue& Value);
	void OnCrouch(const struct FInputActionValue& Value);
	void OnSprintStarted(const struct FInputActionValue& Value);
	void OnSprintCompleted(const struct FInputActionValue& Value);
	
	// Interaction 함수들
	void OnOpenBag(const struct FInputActionValue& Value);
	void OnBuild(const struct FInputActionValue& Value);
	void OnInteract(const struct FInputActionValue& Value);
	void OnStopInteract(const struct FInputActionValue& Value);
	void OnLeftClick(const struct FInputActionValue& Value); //얘넨 모르겠다 (한 키에 여러가지 함수?)
	void OnRightClick(const struct FInputActionValue& Value); // 얘넨 모르겠다.
	void OnPingStarted(const struct FInputActionValue& Value);
	void OnPingCompleted(const struct FInputActionValue& Value);
	void OnEmoteStarted(const struct FInputActionValue& Value);
	void OnEmoteCompleted(const struct FInputActionValue& Value);
	void OnWheelScroll(const struct FInputActionValue& Vaule);
	void OnEsc(const struct FInputActionValue& Vaule);
	
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
	
	virtual void OnMoveSpeedChanged(const FOnAttributeChangeData& Data);
	virtual void Landed(const FHitResult& Hit) override; //낙하 감지 함수
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
	
	
	//test-----------------------------------------------------------------------
	// 디버그 라인 On/Off
	UPROPERTY(EditAnywhere, Category = "LineTrace|Debug")
	bool bLineTraceDebugDraw = false;
	
	void OnTogglelinetrace(const struct FInputActionValue& Value);
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
	
	// 줍는 몽타주 하나 추가
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	TObjectPtr<UAnimMontage> PickUpMontage;
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayPickUpMontage();
};
