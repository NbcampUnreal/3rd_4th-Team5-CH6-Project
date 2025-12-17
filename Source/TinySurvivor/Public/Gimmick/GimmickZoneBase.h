// GimmickZoneBase.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayEffectTypes.h"
#include "GimmickZoneBase.generated.h"

class UBoxComponent;
class USphereComponent;
class UGameplayEffect;
class UAbilitySystemComponent;

/*
	영역 형태 타입
*/
UENUM(BlueprintType)
enum class EGimmickZoneShapeType : uint8
{
	Box UMETA(DisplayName = "Box"),
	Sphere UMETA(DisplayName = "Sphere")
};

/*
	영역 효과 적용 타입
*/
UENUM(BlueprintType)
enum class EGimmickZoneEffectType : uint8
{
	// 영역 내에서만 지속 (진입 시 적용, 퇴장 시 제거)
	WhileInZone UMETA(DisplayName = "While In Zone"),
	
	// 진입 시 1회 적용 (Duration GE 사용)
	OnEnter UMETA(DisplayName = "On Enter"),
	
	// 퇴장 시 1회 적용
	OnExit UMETA(DisplayName = "On Exit")
};

/*
	영역별 GE 설정 구조체
*/
USTRUCT(BlueprintType)
struct FGimmickZoneEffectConfig
{
	GENERATED_BODY()
	
	// 적용할 Gameplay Effect 클래스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zone Effect")
	TSubclassOf<UGameplayEffect> GameplayEffectClass;
	
	// 효과 적용 타입
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zone Effect")
	EGimmickZoneEffectType EffectType = EGimmickZoneEffectType::WhileInZone;
	
	// GE 레벨 (기본 1)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zone Effect")
	float EffectLevel = 1.0f;
	
	// OnEnter 타입일 때, 재진입 방지 쿨다운 (초)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zone Effect",
		meta = (EditCondition = "EffectType == EGimmickZoneEffectType::OnEnter"))
	float ReEnterCooldown = 0.0f;
	
	// 면역 태그 (이 태그를 가진 대상은 효과 무시)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zone Effect")
	FGameplayTagContainer ImmunityTags;
	
	// OnEnter 타입일 때, 영역 내 주기적 체크 활성화 (독 영역용)
	// 면역 해제 또는 Duration 종료 시 재적용
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zone Effect",
		meta = (EditCondition = "EffectType == EGimmickZoneEffectType::OnEnter"))
	bool bPeriodicCheckWhileInZone = false;
	
	// 주기적 체크 간격 (초)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zone Effect",
		meta = (EditCondition = "bPeriodicCheckWhileInZone"))
	float PeriodicCheckInterval = 1.0f;
};

/*
	기믹 영역 베이스 Actor
	- 플레이어가 영역에 진입/퇴장 시 GE 적용/제거
	- Server Authority 기반 동작
	- BP에서 파생하여 다양한 영역 타입 생성
*/
UCLASS(Abstract, Blueprintable)
class TINYSURVIVOR_API AGimmickZoneBase : public AActor
{
	GENERATED_BODY()
	
public:
	AGimmickZoneBase();
	
protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
#pragma region Components
protected:
	//==========================================================================
	// Components
	//==========================================================================
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gimmick Zone")
	USceneComponent* RootComp;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gimmick Zone")
	UBoxComponent* BoxCollision;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gimmick Zone")
	USphereComponent* SphereCollision;
#pragma endregion
	
#pragma region ZoneSettings
protected:
	//==========================================================================
	// Zone Settings
	//==========================================================================
	
	// 영역 형태 (Box or Sphere)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gimmick Zone|Shape")
	EGimmickZoneShapeType ZoneShapeType = EGimmickZoneShapeType::Box;
	
	// Box 크기
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gimmick Zone|Shape",
		meta = (EditCondition = "ZoneShapeType == EGimmickZoneShapeType::Box"))
	FVector BoxExtent = FVector(500.0f, 500.0f, 200.0f);
	
	// Sphere 반경
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gimmick Zone|Shape",
		meta = (EditCondition = "ZoneShapeType == EGimmickZoneShapeType::Sphere"))
	float SphereRadius = 500.0f;
	
	// 영역 활성화 여부
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Gimmick Zone")
	bool bZoneEnabled = true;
	
	// 영역에 적용할 효과 목록 (다중 GE 지원)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gimmick Zone|Effects")
	TArray<FGimmickZoneEffectConfig> ZoneEffects;
#pragma endregion
	
#pragma region Debug
protected:
	//==========================================================================
	// Debug
	//==========================================================================
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gimmick Zone|Debug")
	bool bShowDebugInfo = false;
	
	// 에디터/게임에서 영역 시각화 (Box/Sphere 외곽선 표시)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gimmick Zone|Debug")
	bool bShowZoneVisualization = true;
#pragma endregion
	
#pragma region OverlapEvents
protected:
	//==========================================================================
	// Overlap Events (Server Only)
	//==========================================================================
	
	UFUNCTION()
	void OnZoneBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnZoneEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);
#pragma endregion
	
#pragma region EffectApplicationLogic
protected:
	//==========================================================================
	// Effect Application Logic
	//==========================================================================
	
	// 대상에게 영역 효과 적용
	virtual void ApplyZoneEffects(AActor* TargetActor);
	
	// 대상에게서 영역 효과 제거
	virtual void RemoveZoneEffects(AActor* TargetActor);
	
	// 단일 GE 적용
	void ApplySingleEffect(
		UAbilitySystemComponent* TargetASC,
		const FGimmickZoneEffectConfig& Config);
	
	// WhileInZone 타입 GE 제거
	void RemoveWhileInZoneEffects(UAbilitySystemComponent* TargetASC);
	
	// OnEnter 타입 GE 제거 (영역 퇴장 시 또는 면역 획득 시)
	void RemoveOnEnterEffects(UAbilitySystemComponent* TargetASC, AActor* TargetActor);
	
	// 면역 체크 (ImmunityTags 확인)
	bool IsImmuneToEffect(
		UAbilitySystemComponent* TargetASC,
		const FGimmickZoneEffectConfig& Config) const;
	
	// OnEnter 재진입 쿨다운 체크
	bool CanApplyOnEnterEffect(
		AActor* TargetActor,
		const FGimmickZoneEffectConfig& Config) const;
#pragma endregion
	
#pragma region Tracking
protected:
	//==========================================================================
	// Tracking
	//==========================================================================
	
	// 영역 내 Actor들의 WhileInZone GE Handle 추적
	//UPROPERTY() // TMap 중첩은 UPROPERTY 지원 안됨
	TMap<AActor*, TArray<FActiveGameplayEffectHandle>> ActiveEffectHandles;
	
	// OnEnter 타입 GE Handle 추적 (면역 시 제거용)
	TMap<AActor*, TMap<TSubclassOf<UGameplayEffect>, FActiveGameplayEffectHandle>> OnEnterEffectHandles;
	
	// OnEnter 타입의 마지막 적용 시간 추적 (재진입 쿨다운용)
	//UPROPERTY() // TMap 중첩은 UPROPERTY 지원 안됨
	TMap<AActor*, TMap<TSubclassOf<UGameplayEffect>, float>> LastEnterTimes;
	
	// 영역 내에 있는 Actor 목록 (주기적 체크용)
	TSet<AActor*> ActorsInZone;
	
	// 주기적 체크 타이머 핸들
	FTimerHandle PeriodicCheckTimerHandle;
#pragma endregion
	
#pragma region Helpers
protected:
	//==========================================================================
	// Helpers
	//==========================================================================
	
	// ASC 가져오기
	UAbilitySystemComponent* GetASCFromActor(AActor* Actor) const;
	
	// 현재 활성 Collision Component 반환
	UShapeComponent* GetActiveCollisionComponent() const;
	
	// Shape 업데이트 (Editor에서 실시간 반영)
	void UpdateCollisionShape();
	
	// 주기적 체크 (독 영역용)
	void PeriodicCheck();
	
	// 주기적 체크 타이머 시작/중지
	void StartPeriodicCheck();
	void StopPeriodicCheck();
	
	// 영역 밖 Actor의 면역 체크 (영역 퇴장 후 해독제 복용 대응)
	void CheckImmuneActorsOutsideZone();
#pragma endregion
	
#pragma region PostEditChangeProperty
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
#pragma endregion
	
#pragma region BlueprintInterface
public:
	// BP에서 영역 활성화/비활성화
	UFUNCTION(BlueprintCallable, Category = "Gimmick Zone")
	void SetZoneEnabled(bool bEnabled);
	
	UFUNCTION(BlueprintPure, Category = "Gimmick Zone")
	bool IsZoneEnabled() const { return bZoneEnabled; }
#pragma endregion
};