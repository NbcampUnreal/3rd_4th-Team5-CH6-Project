#include "AI/Monster/Base/TSMonsterBaseCharacter.h"
#include "AI/Monster/MonsterGAS/TSMonsterAS.h"
#include "AI/Monster/MonsterGAS/TSMonsterASC.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Item/LootComponent.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
	
	//---------------------------------------
	// UTSMonsterSpawnSystem 라이프 사이클
	//--------------------------------------
	
ATSMonsterBaseCharacter::ATSMonsterBaseCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	
	MonsterAS = CreateDefaultSubobject<UTSMonsterAS>("MonsterAS");
	MonsterASC = CreateDefaultSubobject<UTSMonsterASC>("MonsterASC");
	MonsterASC->SetIsReplicated(true);
	MonsterASC->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 360.0f, 0.0f);
	GetCharacterMovement()->bOrientRotationToMovement = true;
	
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Block);
	GetMesh()->SetCollisionResponseToAllChannels(ECR_Ignore);
	
	bReplicates = true;
	SetReplicatingMovement(true);
	
	SetNetUpdateFrequency(60.f);
	SetMinNetUpdateFrequency(10.f);
	SetNetCullDistanceSquared(1000000000.0f);
	
	// 루트 컴포넌트
	SpawnedLootComp = CreateDefaultSubobject<ULootComponent>(TEXT("LootComponent"));
	
	GetCharacterMovement()->bUseRVOAvoidance = true;
	GetCharacterMovement()->AvoidanceConsiderationRadius = 300.f;
	GetCharacterMovement()->AvoidanceWeight = 0.5f;
}

void ATSMonsterBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (!IsValid(MonsterASC)) return;
	MonsterASC->InitAbilityActorInfo(this,this);
	
	if (HasAuthority())
	{
		for (auto& GiveGA : GiveAbilities)
		{
			MonsterASC->K2_GiveAbility(GiveGA);
		}
		
		for (auto& GiveGE : GiveGameplayEffects)
		{
			FGameplayEffectContextHandle GameplayEffectContext;
			GameplayEffectContext.AddSourceObject(this);
			MonsterASC->BP_ApplyGameplayEffectToSelf(GiveGE, 1.0f, GameplayEffectContext);
		}
		
		GetCharacterMovement()->bUseRVOAvoidance = true;
	}
}

void ATSMonsterBaseCharacter::Destroyed()
{
	if (IsValid(GetWorld()))
	{
		GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
	}
		
	Super::Destroyed();
}

void ATSMonsterBaseCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (IsValid(GetWorld()))
	{
		GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
	}
	
	Super::EndPlay(EndPlayReason);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
	
	//--------------------------------------
	// ATSMonsterBaseCharacter 기타 
	//--------------------------------------

UAbilitySystemComponent* ATSMonsterBaseCharacter::GetAbilitySystemComponent() const
{
	return MonsterASC;
}

void ATSMonsterBaseCharacter::SetSpeedIncrease()
{
	++CurrentSpeedIncreaseLevel;
	
	if (CurrentSpeedIncreaseLevel > 2)
	{
		CurrentSpeedIncreaseLevel = 2;
	}
	
	if (CurrentSpeedIncreaseLevel == 0)
	{
		GetCharacterMovement()->MaxWalkSpeed = 800.f;
	}
	if (CurrentSpeedIncreaseLevel == 1)
	{
		GetCharacterMovement()->MaxWalkSpeed = 1000.f;
	}
	if (CurrentSpeedIncreaseLevel == 2)
	{
		GetCharacterMovement()->MaxWalkSpeed = 1200.f;
	}
}

void ATSMonsterBaseCharacter::ResetSpeed()
{
	CurrentSpeedIncreaseLevel = 0;
	GetCharacterMovement()->MaxWalkSpeed = 600.f;
}

void ATSMonsterBaseCharacter::StopWalk()
{
	GetCharacterMovement()->MaxWalkSpeed = 0.f;
}

void ATSMonsterBaseCharacter::RegainSpeed()
{
	SetSpeedIncrease();
}

void ATSMonsterBaseCharacter::SetDropRootItems(FTSMonsterTable& MonsterTable)
{
	// 3. 메인 스폰 저장 
	FLootRule MainLoot;
	MainLoot.ItemID = MonsterTable.MainDropTableID;
	MainLoot.DropChance = MonsterTable.MainDropTablePrecent;
	MainLoot.MaxCount = MonsterTable.MainDropMaxNum;
	MainLoot.MinCount = MonsterTable.MainDropMinNum;
	SpawnedLootComp->LootTable.Add(MainLoot);
	
	UE_LOG(LogTemp, Warning, TEXT("[MAIN LOOT] ID=%d, Chance=%.2f, Min=%d, Max=%d"),
		MainLoot.ItemID,
		MainLoot.DropChance,
		MainLoot.MinCount,
		MainLoot.MaxCount
	);
	UE_LOG(LogTemp, Warning, TEXT("LootTable Count After Main: %d"), SpawnedLootComp->LootTable.Num());

	// 4. 서브 스폰 저장
	FLootRule SubLoot;
	SubLoot.ItemID = MonsterTable.SubDropTableID;
	SubLoot.DropChance = MonsterTable.SubDropTablePrecent;
	SubLoot.MaxCount = MonsterTable.SubDropMaxNum;
	SubLoot.MinCount = MonsterTable.SubDropMinNum;
	
	UE_LOG(LogTemp, Warning, TEXT("[SUB LOOT] ID=%d, Chance=%.2f, Min=%d, Max=%d"),
		SubLoot.ItemID,
		SubLoot.DropChance,
		SubLoot.MinCount,
		SubLoot.MaxCount
	);
	
	SpawnedLootComp->LootTable.Add(SubLoot);
	UE_LOG(LogTemp, Warning, TEXT("LootTable Count After Sub: %d"), SpawnedLootComp->LootTable.Num());
	UE_LOG(LogTemp, Error, TEXT("몬스터 드랍 아이템 설정 완료"));
}

void ATSMonsterBaseCharacter::RequestSpawnDropRooItems()
{
	FVector SpawnLocation = GetActorLocation();
	SpawnLocation.Z += 100.f;
	bool bSuccess = SpawnedLootComp->SpawnLoot(SpawnLocation);
	if (false == bSuccess)
	{
		UE_LOG(LogTemp, Error, TEXT("몬스터 죽고 드랍 아이템 스폰 실패함"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("몬스터 죽고 드랍 아이템 스폰 성공함"));
	}
}

void ATSMonsterBaseCharacter::MakeTimeToDead()
{
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &ThisClass::DeadTime, DelayTimeToDead, false);
}

void ATSMonsterBaseCharacter::DeadTime()
{
	this->Destroy();
}
