#include "System/ResourceControl/TSResourceBaseActor.h"
#include "AbilitySystemComponent.h"
#include "Controller/TSPlayerController.h"
#include "Item/WorldItem.h"
#include "Item/System/ItemDataSubsystem.h"
#include "System/ResourceControl/TSResourceControlSubSystem.h"
#include "Item/LootComponent.h"
#include "System/ResourceControl/TSResourcePoint.h"
#include "GameplayTags/ItemGameplayTags.h"
#include "GameplayTags/NofiticationTags.h"
#include "GameplayTags/System/GameplayDisplaySubSystem.h"
#include "Kismet/GameplayStatics.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	

	//========================
	// ATSResourceBaseActor 라이프사이클
	//========================

ATSResourceBaseActor::ATSResourceBaseActor()
{
	// 네트워크
	bReplicates = true;
	SetReplicatingMovement(true);
	SetMinNetUpdateFrequency(1.f);
	SetNetUpdateFrequency(10.f);

	// 틱
	PrimaryActorTick.bCanEverTick = false;
	
	// 메쉬 설정
	ResourceStaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	SetRootComponent(ResourceStaticMeshComp);
	ResourceStaticMeshComp->SetNotifyRigidBodyCollision(true); // Hit 이벤트 켜기
	
	// 루트 컴포넌트
	LootComponent = CreateDefaultSubobject<ULootComponent>(TEXT("LootComponent"));
}

void ATSResourceBaseActor::BeginPlay()
{
	Super::BeginPlay();
	
	if (!HasAuthority()) return;
	
	// 자원 원천 테스트 용도로 추가. 
	UItemDataSubsystem* ItemDataSubsystem = UItemDataSubsystem::GetItemDataSubsystem(this);
	if (!IsValid(ItemDataSubsystem))
	{
		if (bShowDebug) UE_LOG(ResourceControlSystem, Error, TEXT("ItemDataSubsystem 찾지 못함."));
		return;
	}
	
	bool bFoundData = ItemDataSubsystem->GetResourceDataSafe(ThisResourceID, ResourceRuntimeData);
	if (!bFoundData)
	{
		if (bShowDebug) UE_LOG(ResourceControlSystem, Error, TEXT("//============================================//"));
		if (bShowDebug) UE_LOG(ResourceControlSystem, Error, TEXT("%s 가 자원 원천 데이터를 찾는데 실패함."), *GetName());
		if (bShowDebug) UE_LOG(ResourceControlSystem, Error, TEXT("ThisResourceID %d 찾지 못함."), ThisResourceID);
		if (bShowDebug) UE_LOG(ResourceControlSystem, Error, TEXT("//============================================//"));
		return;
	}
	else
	{
		if (bShowDebug) UE_LOG(ResourceControlSystem, Log, TEXT("//============================================//"));
		if (bShowDebug) UE_LOG(ResourceControlSystem, Log, TEXT("%s 가 자원 원천 데이터를 찾는데 성공함."), *GetName());
		if (bShowDebug) UE_LOG(ResourceControlSystem, Log, TEXT("ThisResourceID %d 찾음."), ThisResourceID);
	}
	
	CurrentItemCount = ResourceRuntimeData.TotalYield;
	if (bShowDebug) UE_LOG(ResourceControlSystem, Log, TEXT("아이템 수량 설정 완료 %d"), CurrentItemCount);
	
	if (bShowDebug) UE_LOG(ResourceControlSystem, Log, TEXT("아이템 뱉어내기 준비 완료"));
	if (bShowDebug) UE_LOG(ResourceControlSystem, Error, TEXT("테스트 용이므로 추후 반드시 언바인딩할 것."));
	if (bShowDebug) UE_LOG(ResourceControlSystem, Log, TEXT("//============================================//"));
	
	if (IsLevelPlaced)
	{
		// 3. 메인 스폰 저장 
		FLootRule MainLoot;
		MainLoot.ItemID = ResourceRuntimeData.MainDropTableID;
		MainLoot.DropChance = ResourceRuntimeData.MainDropTablePrecent;
		MainLoot.MaxCount = ResourceRuntimeData.MainDropMaxNum;
		MainLoot.MinCount = ResourceRuntimeData.MainDropMinNum;
		LootComponent->LootTable.Add(MainLoot);
	
		// 4. 서브 스폰 저장
		FLootRule SubLoot;
		SubLoot.ItemID = ResourceRuntimeData.SubDropTableID;
		SubLoot.DropChance = ResourceRuntimeData.SubDropTablePrecent;
		SubLoot.MaxCount = ResourceRuntimeData.SubDropMaxNum;
		SubLoot.MinCount = ResourceRuntimeData.SubDropMinNum;
		LootComponent->LootTable.Add(SubLoot);
	
		if (!IsValid(LootComponent))
		{
			if (bShowDebug) UE_LOG(LogTemp, Error, TEXT("[TSLoot] LootComponent INVALID!"));
		}
		else
		{
			if (bShowDebug) UE_LOG(LogTemp, Warning, TEXT("[TSLoot] LootTable Count = %d"), LootComponent->LootTable.Num());

			for (int32 i = 0; i < LootComponent->LootTable.Num(); i++)
			{
				const FLootRule& Rule = LootComponent->LootTable[i];

				if (bShowDebug) UE_LOG(LogTemp, Warning,
					TEXT("[TSLoot] [%d] ItemID=%d | Chance=%f | Min=%d | Max=%d"),
					i,
					Rule.ItemID,
					Rule.DropChance,
					Rule.MinCount,
					Rule.MaxCount
				);
			}
		}
	}
}

void ATSResourceBaseActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
	}
	Super::EndPlay(EndPlayReason);
}

void ATSResourceBaseActor::SetSpawnPoint(ATSResourcePoint* Point)
{
	if (!IsValid(Point)) return;
	OwningPoint = Point;	
	if (bShowDebug) UE_LOG(ResourceControlSystem, Log, TEXT("OwningPoint 이식 성공 "));
}

void ATSResourceBaseActor::SetMeshComp(UStaticMesh* InResourceStaticMeshComp)
{
	ResourceStaticMeshComp->SetStaticMesh(InResourceStaticMeshComp);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	

	//========================
	// ATSResourceBaseActor 아이템 스폰  
	//========================

void ATSResourceBaseActor::InitFromResourceData(FResourceData& Data)
{
	// 1. 리소스 데이터 복사
	ResourceRuntimeData = Data;
	CurrentItemCount = ResourceRuntimeData.TotalYield;
	CurrentResourceHealth = ResourceRuntimeData.ResourceHealth;
	CurrentDestroySound = ResourceRuntimeData.DestroySound;
	if (bShowDebug) UE_LOG(ResourceControlSystem, Log, TEXT("아이템 수량 설정 완료 %d"), CurrentItemCount);
	if (bShowDebug) UE_LOG(ResourceControlSystem, Log, TEXT("자원 체력 설정 완료 %f"), CurrentResourceHealth);
	
	// 2. 메시 설정
	if (IsValid(ResourceStaticMeshComp))
	{
		if (Data.WorldMesh.IsValid())
		{
			UStaticMesh* LoadedMesh = Data.WorldMesh.LoadSynchronous();
			if (LoadedMesh)
			{
				ResourceStaticMeshComp->SetStaticMesh(LoadedMesh);
			}
		}
	}

	// 3. 메인 스폰 저장 
	FLootRule MainLoot;
	MainLoot.ItemID = ResourceRuntimeData.MainDropTableID;
	MainLoot.DropChance = ResourceRuntimeData.MainDropTablePrecent;
	MainLoot.MaxCount = ResourceRuntimeData.MainDropMaxNum;
	MainLoot.MinCount = ResourceRuntimeData.MainDropMinNum;
	LootComponent->LootTable.Add(MoveTemp(MainLoot));
	
	// 4. 서브 스폰 저장
	FLootRule SubLoot;
	SubLoot.ItemID = ResourceRuntimeData.SubDropTableID;
	SubLoot.DropChance = ResourceRuntimeData.SubDropTablePrecent;
	SubLoot.MaxCount = ResourceRuntimeData.SubDropMaxNum;
	SubLoot.MinCount = ResourceRuntimeData.SubDropMinNum;
	LootComponent->LootTable.Add(MoveTemp(SubLoot));
	
	if (!IsValid(LootComponent))
	{
		if (bShowDebug) UE_LOG(LogTemp, Error, TEXT("[TSLoot] LootComponent INVALID!"));
	}
	else
	{
		if (bShowDebug) UE_LOG(LogTemp, Warning, TEXT("[TSLoot] LootTable Count = %d"), LootComponent->LootTable.Num());

		for (int32 i = 0; i < LootComponent->LootTable.Num(); i++)
		{
			const FLootRule& Rule = LootComponent->LootTable[i];

			if (bShowDebug) UE_LOG(LogTemp, Warning,
				TEXT("[TSLoot] [%d] ItemID=%d | Chance=%f | Min=%d | Max=%d"),
				i,
				Rule.ItemID,
				Rule.DropChance,
				Rule.MinCount,
				Rule.MaxCount
			);
		}
	}
	
	if (bShowDebug) UE_LOG(LogTemp, Warning, TEXT("Init Resource %d at Actor %s"), Data.ResourceID, *GetName());
}

void ATSResourceBaseActor::GetItemFromResource(UAbilitySystemComponent* ASC, EItemAnimType& InRequiredToolType, int32& ATK, FVector& HitPoint, FVector& HitNormal, FVector PlayerLocation, FVector ForwardVector, bool IsLeftMouseClicked)
{
#pragma region 검사
	
	if (!HasAuthority())
	{
		UE_LOG(ResourceControlSystem, Warning,TEXT("[GetItemFromResource] HasAuthority is fail"));
		return;
	}
	
	if (!IsValid(ASC))
	{
		UE_LOG(ResourceControlSystem, Warning,TEXT("[GetItemFromResource] ASC is Null"));
		return;
	}
	
	// 이미 파괴 예약 상태면 추가 처리 막기 (중복 스폰/중복 요청 방지)
	if (bPendingDestroy)
	{
		return;
	}
	
	bool IsFoundAnyMatchType = false;
	bool IsFoundMatchTypeNone = false;
	
	for (auto RequiredToolType : ResourceRuntimeData.RequiredToolTypes)
	{
		if (InRequiredToolType == RequiredToolType)
		{
			IsFoundAnyMatchType = true;
		}
		
		if (InRequiredToolType == EItemAnimType::INTERACT)
		{
			IsFoundMatchTypeNone = true;
		}
	}
	if (ResourceRuntimeData.RequiredToolTypes.Contains(EItemAnimType::INTERACT) && IsLeftMouseClicked)
	{
		UE_LOG(ResourceControlSystem, Warning,TEXT("[GetItemFromResource] EItemAnimType::INTERACT 입니다. 왼쪽 마우스 클릭으로는 호출할 수 없습니다."));
		ShowNotification(ASC, NotificationTags::TAG_Notification_Resource_Interact);
		return;
	}
	
	if (IsFoundAnyMatchType == false)
	{
		UE_LOG(ResourceControlSystem, Warning,TEXT("[GetItemFromResource] RequiredToolID is NOT matching"));
		ShowNotification(ASC, NotificationTags::TAG_Notification_Resource_NotToolMatch);
		return;
	}


#pragma endregion
	
#pragma region 위치_세팅
	constexpr float BackOffset = 80.f;  // 표면 반대 방향
	constexpr float UpOffset = 50.f;    // 기본 위로 띄우기
	constexpr float MinPlayerDistance = 120.f; // 플레이어와 최소 거리
	constexpr float ExtraUp = 120.f;    // 플레이어와 겹칠 때 추가 상승값

	// HitPoint → 반대 방향 기본 위치
	FVector FinalSpawnLocation = HitPoint + HitNormal * BackOffset + FVector(0, 0, UpOffset);

	// 2) 플레이어와 거리 체크
	float DistToPlayer = FVector::Dist(PlayerLocation, FinalSpawnLocation);

	if (DistToPlayer < MinPlayerDistance)
	{
		if (FinalSpawnLocation.Z < PlayerLocation.Z + ExtraUp)
		{
			FinalSpawnLocation.Z = PlayerLocation.Z + ExtraUp;
		}
	}
#pragma endregion 

	if (bShowDebug) UE_LOG(ResourceControlSystem, Error, TEXT("GetItemFromResource 시작"));
	
	// (추가) "이번 함수 호출(=한 번의 공격)"에서 사운드는 딱 1번만 재생
	bool bPlayedSoundThisCall = false;
	auto PlaySoundOnce = [&]()
	{
		if (bPlayedSoundThisCall) return;
		bPlayedSoundThisCall = true;

		USoundBase* Sound = CurrentDestroySound.LoadSynchronous();
		Multicast_PlaySound(Sound);
	};
	
	// none 타입의 경우 시간 차로 얻어야 함.
	if (IsFoundMatchTypeNone)
	{
		bool NoHealth = false;
		DoHarvestLogic(ASC, ATK,PlayerLocation, FinalSpawnLocation, NoHealth);
		
		PlaySoundOnce();
		
		bPendingDestroy = true;
		RequestSpawnResource();
		return;
	}
	
	// none 타입이 아닌 경우 (펀치 등등 다른 모든 타입)
	if (IsFoundAnyMatchType == true)
	{
		bool YesHealth = true;
		
		// ResourceHealth가 0(또는 음수)인 데이터는 "체력 미사용 자원"으로 처리
		// -> 1회 스폰 후 즉시 리스폰/파괴 (기존 IsHasHealth==false 루트의 대체)
		if (ResourceRuntimeData.ResourceHealth <= 0.0f)
		{
			bool NoHealth = false;
			DoHarvestLogic(ASC, ATK, PlayerLocation, FinalSpawnLocation, NoHealth);

			PlaySoundOnce();

			bPendingDestroy = true;
			RequestSpawnResource();
			return;
		}

		// 피격 전/후 를 계산하여 스폰 실행 => 한 번에 체력이 0이 될 수 있기 때문에
		const float MaxHP = FMath::Max(1.0f, ResourceRuntimeData.ResourceHealth);
		const float OldHP = CurrentResourceHealth;
		const float NewHP = CurrentResourceHealth - ATK;

		const float OldPercent = OldHP / MaxHP;
		const float NewPercent = NewHP / MaxHP;

		CurrentResourceHealth = NewHP;
		if (bShowDebug) UE_LOG(ResourceControlSystem, Error, TEXT("현재 자원 체력 %f"), CurrentResourceHealth);
	
		// ASC 이벤트 태그 전송  (도구 내구도 감소)
		FGameplayEventData EventData;
		EventData.EventTag = ItemTags::TAG_Event_Item_Tool_Harvest;
		EventData.EventMagnitude = 0.0f;
		EventData.Instigator = ASC->GetAvatarActor();
		EventData.Target = ASC->GetAvatarActor();
		ASC->HandleGameplayEvent(ItemTags::TAG_Event_Item_Tool_Harvest, &EventData);
		if (bShowDebug) UE_LOG(ResourceControlSystem, Error, TEXT("이벤트 전송함"));
	
		// 이번 공격에서 스폰이 실제로 몇 번 발생했는지 (사운드 중복 재생 방지)
		int32 SpawnCountThisCall = 0;

		// 70% 
		if (!bDoOnceSpawnedIn70 && OldPercent > 0.7f && NewPercent <= 0.7f)
		{
			bDoOnceSpawnedIn70 = true;
			DoHarvestLogic(ASC, ATK,PlayerLocation, FinalSpawnLocation, YesHealth);
			SpawnCountThisCall++;
		}

		// 30% 
		if (!bDoOnceSpawnedIn30 && OldPercent > 0.3f && NewPercent <= 0.3f)
		{
			bDoOnceSpawnedIn30 = true;
			DoHarvestLogic(ASC, ATK,PlayerLocation, FinalSpawnLocation, YesHealth);
			SpawnCountThisCall++;
		}

		// 0%
		if (!bDoOnceSpawnedIn00 && OldPercent > 0.0f && NewPercent <= 0.0f)
		{
			bDoOnceSpawnedIn00 = true;
			DoHarvestLogic(ASC, ATK,PlayerLocation, FinalSpawnLocation, YesHealth);
			SpawnCountThisCall++;
		}

		// 스폰이 1번이라도 발생했으면 사운드 재생
		if (SpawnCountThisCall > 0)
		{
			PlaySoundOnce();
		}

		// 파괴/리스폰은 모든 스폰 처리 후 마지막에 실행
		if (CurrentResourceHealth <= 0.0f)
		{
			bPendingDestroy = true;
			if (bShowDebug) UE_LOG(ResourceControlSystem, Error, TEXT("체력이 없으므로 삭제"));
			RequestSpawnResource();
			return;
		}
	}
}

void ATSResourceBaseActor::ShowNotification(UAbilitySystemComponent* ASC, const FGameplayTag& NotificationTag)
{
	if (!ASC) return;

	// 캐싱된 서브시스템 확인 및 초기화
	if (!CachedGTDS)
	{
		if (UGameInstance* GI = GetWorld()->GetGameInstance())
		{
			CachedGTDS = GI->GetSubsystem<UGameplayTagDisplaySubsystem>();
		}
	}

	if (!CachedGTDS) return;

	// ASC를 통해 플레이어 컨트롤러 찾기
	APlayerController* PC = nullptr;
	if (AActor* Avatar = ASC->GetAvatarActor())
	{
		if (APawn* Pawn = Cast<APawn>(Avatar))
		{
			PC = Cast<APlayerController>(Pawn->GetController());
		}
	}

	// 클라이언트 전용 알림 호출 (RPC)
	ATSPlayerController* TSPC = Cast<ATSPlayerController>(PC);
	if (TSPC)
	{
		TSPC->ClientShowNotificationOnHUD(CachedGTDS->GetDisplayName_KR(NotificationTag));
	}
}

void ATSResourceBaseActor::DoHarvestLogic(UAbilitySystemComponent* ASC, int32& ATK, FVector& TargetLocation, FVector& SpawnOriginLocation, bool& IsHasHealth)
{
	if (IsValid(LootComponent))
	{
		bool bSuccess = LootComponent->SpawnHarvestLoot(TargetLocation, SpawnOriginLocation);
		if (false == bSuccess)
		{
			if (bShowDebug) UE_LOG(ResourceControlSystem, Error, TEXT("현재 자원 아이템 스폰 실패함"));
			return;
		}
	}

	if (bShowDebug) UE_LOG(ResourceControlSystem, Error, TEXT("현재 자원 아이템 스폰 성공"));

}

void ATSResourceBaseActor::RequestSpawnResource()
{
	UTSResourceControlSubSystem* ResourceControlSubSystem = GetWorld()->GetSubsystem<UTSResourceControlSubSystem>();
	if (IsValid(ResourceControlSubSystem))
	{
		if (OwningPoint.IsValid())
		{
			ResourceControlSubSystem->RequestRespawn(
			OwningPoint->GetSectionTag(),
			ResourceRuntimeData.ResourceID,
			ResourceRuntimeData.RespawnTime,
			OwningPoint->GetResourceItemType(),
			OwningPoint->GetSectionResourceUniqueTag(), 
			OwningPoint->IsCommonPoint()
			);
			OwningPoint->ClearAllocatedResource(); 
			if (bShowDebug) UE_LOG(ResourceControlSystem, Log, TEXT("스폰 포인트와 컨트롤 시스템에게 요청 완료"));
		}
		else
		{
			if (bShowDebug) UE_LOG(ResourceControlSystem, Error, TEXT("스폰 포인트와 컨트롤 시스템에게 요청 실패 : 스폰 포인트 찾지 못함."));
		}
	}
	else
	{
		if (bShowDebug) UE_LOG(ResourceControlSystem, Error, TEXT("스폰 포인트와 컨트롤 시스템에게 요청 실패 : 컨트롤 시스템 찾지 못함."));
	}
	Destroy();
}

void ATSResourceBaseActor::Multicast_PlaySound_Implementation(USoundBase* SoundToPlay)
{
	//  파괴 사운드 재생
	if (SoundToPlay)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			SoundToPlay,
			GetActorLocation(),
			FRotator::ZeroRotator,
			1.0f, // Volume Multiplier
			1.0f, // Pitch Multiplier
			0.0f, // Start Time
			DistanceAttenuation
		);
	}
}


TArray<EItemAnimType>& ATSResourceBaseActor::GetResourceRequiredToolTypes()
{
	return ResourceRuntimeData.RequiredToolTypes;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	

	//========================
	// ATSResourceBaseActor 인터렉션 
	//========================
	
void ATSResourceBaseActor::ShowInteractionWidget(ATSCharacter* InstigatorCharacter)
{
}

void ATSResourceBaseActor::HideInteractionWidget()
{
}

void ATSResourceBaseActor::SetInteractionText(FText InteractionText)
{
}

bool ATSResourceBaseActor::CanInteract(ATSCharacter* InstigatorCharacter)
{
	return true;
}

void ATSResourceBaseActor::Interact(ATSCharacter* InstigatorCharacter)
{
}

bool ATSResourceBaseActor::RunOnServer()
{
	return true;
}
