#include "System/ResourceControl/TSResourceBaseActor.h"
#include "AbilitySystemComponent.h"
#include "Item/WorldItem.h"
#include "Item/System/ItemDataSubsystem.h"
#include "System/ResourceControl/TSResourceControlSubSystem.h"
#include "Item/LootComponent.h"
#include "System/ResourceControl/TSResourcePoint.h"
#include "GameplayTags/ItemGameplayTags.h"

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
	
	if (IsFoundAnyMatchType == false)
	{
		UE_LOG(ResourceControlSystem, Warning,TEXT("[GetItemFromResource] RequiredToolID is NOT matching"));
		return;
	}
	
	if (ResourceRuntimeData.RequiredToolTypes.Contains(EItemAnimType::INTERACT) && IsLeftMouseClicked)
	{
		UE_LOG(ResourceControlSystem, Warning,TEXT("[GetItemFromResource] EItemAnimType::NONE 입니다. 왼쪽 마우스 클릭으로는 호출할 수 없습니다."));
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
	
	// none 타입의 경우 시간 차로 얻어야 함.
	if (IsFoundMatchTypeNone)
	{
		bool NoHealth = false;
		DoHarvestLogic(ASC, ATK,PlayerLocation, FinalSpawnLocation, NoHealth);
	}
	
	// none 타입이 아닌 경우 (펀치 등등 다른 모든 타입)
	if (IsFoundAnyMatchType == true)
	{
		bool YesHealth = true;
		float CurrentHPPercent = CurrentResourceHealth / ResourceRuntimeData.ResourceHealth;
		
		CurrentResourceHealth = CurrentResourceHealth - ATK;
		if (bShowDebug) UE_LOG(ResourceControlSystem, Error, TEXT("현재 자원 체력 %f"), CurrentResourceHealth);
		
		// ASC 이벤트 태그 전송  (도구 내구도 감소)
		FGameplayEventData EventData;
		EventData.EventTag = ItemTags::TAG_Event_Item_Tool_Harvest;
		EventData.EventMagnitude = 0.0f;
		EventData.Instigator = ASC->GetAvatarActor();
		EventData.Target = ASC->GetAvatarActor();
		ASC->HandleGameplayEvent(ItemTags::TAG_Event_Item_Tool_Harvest, &EventData);
		if (bShowDebug) UE_LOG(ResourceControlSystem, Error, TEXT("이벤트 전송함"));
	
		
		// 70퍼센트
		if (CurrentHPPercent > 0.3f && CurrentHPPercent <= 0.7f) 
		{
			if (bDoOnceSpawnedIn70) return;
			bDoOnceSpawnedIn70 = true;
			DoHarvestLogic(ASC, ATK,PlayerLocation, FinalSpawnLocation, YesHealth);
		} 
		// 30퍼센트
		else if (CurrentHPPercent > 0.f && CurrentHPPercent <= 0.3f) 
		{
			if (bDoOnceSpawnedIn30) return;
			bDoOnceSpawnedIn30 = true;
			DoHarvestLogic(ASC, ATK,PlayerLocation, FinalSpawnLocation, YesHealth);
		} 
		// 0퍼센트
		else if (CurrentHPPercent <= 0.0f)
		{
			if (bDoOnceSpawnedIn00) return;
			bDoOnceSpawnedIn00 = true;
			DoHarvestLogic(ASC, ATK,PlayerLocation, FinalSpawnLocation, YesHealth);
		}
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
	
	
	// 만약 체력이 안 쓴다? 그러면 죽어.
	if (false == IsHasHealth)
	{
		UE_LOG(ResourceControlSystem, Error, TEXT("체력을 안 쓰는 놈이 삭제"));
		RequestSpawnResource();
	}
	
	// 만약 체력 없다? 그러면 죽어.
	else
	{
		if (CurrentResourceHealth <= 0.f)
		{
			if (bShowDebug) UE_LOG(ResourceControlSystem, Error, TEXT("체력을 없으므로 삭제"));
			RequestSpawnResource();
		}
	}
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
