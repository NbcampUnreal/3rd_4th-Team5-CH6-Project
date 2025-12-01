// ChaserCharacter.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ChaserCharacter.generated.h"

class ULootComponent;

UENUM(Blueprintable)
enum class EChaserState : uint8
{
	Idle,
	Patrol,
	Chase,
	Attack, 
	Dead
};

UCLASS()
class TINYSURVIVOR_API AChaserCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AChaserCharacter();

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

public:	
	// 아이템 루팅을 위한 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	ULootComponent* LootComponent;
	// 서버에서 상태를 바꾸면 클라도 알 수 있게 RepNotify 사용
	UPROPERTY(ReplicatedUsing = OnRep_ChaserState, BlueprintReadOnly, Category = "AI")
	EChaserState CurrentState;
	
	// 설정값
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	UAnimMontage* AttackMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Stats")
	float MaxHealth = 100.0f;
	
	float CurrentHealth;
	
	UFUNCTION()
	void OnRep_ChaserState();
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayerAttackMontage();
	
	// 데미지 받음
	virtual float TakeDamage(
		float DamageAmount, 
		struct FDamageEvent const& DamageEvent, 
		class AController* EventInstigator, 
		AActor* DamageCauser
	) override; 
	
	// [공격] 서버가 호출 -> 클라에서 몽타주 재생
	void PerformAttack();
	// 사망
	void Die();
};
