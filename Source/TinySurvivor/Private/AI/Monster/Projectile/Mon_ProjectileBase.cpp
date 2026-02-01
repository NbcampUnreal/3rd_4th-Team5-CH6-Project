// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Monster/Projectile/Mon_ProjectileBase.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "NiagaraComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

AMon_ProjectileBase::AMon_ProjectileBase()
{
	PrimaryActorTick.bCanEverTick = false;
	
	bReplicates = true;
	SetReplicatingMovement(true);
	SetNetUpdateFrequency(100.f);
	SetMinNetUpdateFrequency(10.f);
	
	ProjectileCollision = CreateDefaultSubobject<USphereComponent>("ProjectileCollision");
	SetRootComponent(ProjectileCollision);
	
	ProjectileArrow = CreateDefaultSubobject<UArrowComponent>("ProjectileArrow");
	ProjectileArrow->SetupAttachment(ProjectileCollision);
	
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>("ProjectileMesh");
	ProjectileMesh->SetupAttachment(ProjectileCollision);
	
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");
	ProjectileMovement->UpdatedComponent = ProjectileCollision;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->ProjectileGravityScale = 0.f;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->bAutoActivate = false;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 5000.f;
	
	ProjectileCollision->OnComponentBeginOverlap.AddDynamic(this, &AMon_ProjectileBase::OnComponentBeginOverlap);
	ProjectileCollision->OnComponentHit.AddDynamic(this, &AMon_ProjectileBase::OnComponentHit);
	
	ProjectileVFX = CreateDefaultSubobject<UNiagaraComponent>("ProjectileVFX");
	ProjectileVFX->SetupAttachment(RootComponent);
}

void AMon_ProjectileBase::BeginPlay()
{
	Super::BeginPlay();
	
	ProjectileVFX->SetActive(true);
}

void AMon_ProjectileBase::TryActivateProjectile()
{
	if (!HasAuthority()) return;
	if (!IsValid(ProjectileMovement)) return;
	
	FVector From = GetActorLocation();
	FVector To = TargetLocation;
	FVector Direction = UKismetMathLibrary::GetDirectionUnitVector(From, To);
	FVector FinalVelocity =  Direction * Speed;
	
	// 속도 = 방향 x 크기 
	ProjectileMovement->Velocity = FinalVelocity;
	ProjectileMovement->Activate(true);
	
	// ===== 디버그: 시작점 -> 목표점 라인 표시 =====
	DrawDebugLine(
		GetWorld(),     // 월드 컨텍스트
		From,           // 시작점
		To,             // 끝점
		FColor::Green,  // 색상
		false,          // 지속시간 (false = Tick마다 그려야 함)
		5.0f,           // 화면에 표시되는 시간
		0,              // 깊이 우선순위
		5.0f            // 선 굵기
	);

	// ===== 디버그: 시작점과 끝점 위치 표시 =====
	DrawDebugSphere(GetWorld(), From, 20.f, 12, FColor::Blue, false, 5.f);
	DrawDebugSphere(GetWorld(), To, 20.f, 12, FColor::Red, false, 5.f);
	
	// ===== 속도 로그 =====
	FVector CurrentVel = ProjectileMovement->Velocity;
	FVector CurrentPos = GetActorLocation();
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Tick Debug - Pos: %s | Vel: %s"), *CurrentPos.ToString(), *CurrentVel.ToString()));
	
	FString VelocityString = FinalVelocity.ToString();
	UE_LOG(LogTemp, Warning, TEXT("Projectile velocity: %s"), *VelocityString);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Velocity: %s"), *VelocityString));

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Projectile launched!"));
}

void AMon_ProjectileBase::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ACharacter* PlayerChar = Cast<ACharacter>(OtherActor);
	if (!PlayerChar) return;;

	APlayerController* ThePlayerController = Cast<APlayerController>(PlayerChar->GetController()); 
	if (!IsValid(ThePlayerController)) return;
        
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(PlayerChar);
	if (!TargetASC || !HitAbilityClass) return;

	// ====== 핵심: GameplayEventData 생성 ======
	FGameplayEventData EventData;
	EventData.EventTag = AttackTag;                 // 이 태그 Ability를 Event 방식으로 실행
	EventData.Instigator = Owner;                   // 공격자
	EventData.Target = PlayerChar;                  // 피격자
	EventData.EventMagnitude = AttackDamageValue;   // 공격력 전달

	// AbilitySpec 생성
	FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(
		HitAbilityClass,     // Ability Class
		1,                   // Ability Level
		INDEX_NONE,          // InputID
		Owner                // SourceObject
	);
        
	// ====== Ability 실행 (Payload 포함) ======
	TargetASC->GiveAbilityAndActivateOnce(AbilitySpec, &EventData);
	UE_LOG(LogTemp, Warning, TEXT("Monster damage event sent: %s | Damage=%.1f"), *PlayerChar->GetName(), AttackDamageValue);
	
	Destroy();
}

void AMon_ProjectileBase::OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	Destroy();
}

