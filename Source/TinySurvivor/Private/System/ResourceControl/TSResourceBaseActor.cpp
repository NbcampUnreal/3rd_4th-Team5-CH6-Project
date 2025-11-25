#include "System/ResourceControl/TSResourceBaseActor.h"
#include "Item/WorldItem.h"
#include "Item/System/ItemDataSubsystem.h"
#include "System/ResourceControl/TSResourceControlSubSystem.h"
#include "Item/LootComponent.h"
#include "System/ResourceControl/TSResourcePoint.h"

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
	
	UItemDataSubsystem* ItemDataSubsystem = UItemDataSubsystem::GetItemDataSubsystem(this);
	if (!IsValid(ItemDataSubsystem))
	{
		UE_LOG(ResourceControlSystem, Error, TEXT("ItemDataSubsystem 찾지 못함."));
		return;
	}
	
	bool bFoundData = ItemDataSubsystem->GetResourceDataSafe(ThisResourceID, ResourceRuntimeData);
	if (!bFoundData)
	{
		UE_LOG(ResourceControlSystem, Error, TEXT("//============================================//"));
		UE_LOG(ResourceControlSystem, Error, TEXT("%s 가 자원 원천 데이터를 찾는데 실패함."), *GetName());
		UE_LOG(ResourceControlSystem, Error, TEXT("ThisResourceID %d 찾지 못함."), ThisResourceID);
		UE_LOG(ResourceControlSystem, Error, TEXT("//============================================//"));
		return;
	}
	else
	{
		UE_LOG(ResourceControlSystem, Log, TEXT("//============================================//"));
		UE_LOG(ResourceControlSystem, Log, TEXT("%s 가 자원 원천 데이터를 찾는데 성공함."), *GetName());
		UE_LOG(ResourceControlSystem, Log, TEXT("ThisResourceID %d 찾음."), ThisResourceID);
	}
	
	CurrentItemCount = ResourceRuntimeData.TotalYield;
	UE_LOG(ResourceControlSystem, Log, TEXT("아이템 수량 설정 완료 %d"), CurrentItemCount);
	
	UE_LOG(ResourceControlSystem, Log, TEXT("아이템 뱉어내기 준비 완료"));
	UE_LOG(ResourceControlSystem, Error, TEXT("테스트 용이므로 추후 반드시 언바인딩할 것."));
	UE_LOG(ResourceControlSystem, Log, TEXT("//============================================//"));
	
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
	UE_LOG(ResourceControlSystem, Log, TEXT("OwningPoint 이식 성공 "));
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	

	//========================
	// ATSResourceBaseActor 아이템 스폰  
	//========================


void ATSResourceBaseActor::InitFromResourceData(const FResourceData& Data)
{
	// 1. ID나 타입 저장
	ResourceRuntimeData.ResourceID = Data.ResourceID;
	ResourceRuntimeData.NodeType = Data.NodeType;
	ResourceRuntimeData.TotalYield = Data.TotalYield;
	ResourceRuntimeData.RespawnTime = Data.RespawnTime;

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

	// 3. 드롭 테이블 설정
	ResourceRuntimeData.DropTableID = Data.DropTableID;

	// 4. 필요 도구 설정
	ResourceRuntimeData.RequiredToolID = Data.RequiredToolID;

	// 5. 노드 티어 저장
	ResourceRuntimeData.NodeTier = Data.NodeTier;

	// 6. 스폰 루트 저장 
	FLootRule NewLoot = {};
	NewLoot.DropChance = 1.0f;
	NewLoot.ItemID = ResourceRuntimeData.DropTableID;
	NewLoot.MaxCount = 1;
	NewLoot.MinCount = 1;
	LootComponent->LootTable.Add(NewLoot);
	
	UE_LOG(LogTemp, Warning, TEXT("Init Resource %d at Actor %s"), Data.ResourceID, *GetName());
}


void ATSResourceBaseActor::GetItemFromResource(int32 RequiredToolID, FVector HitPoint, FVector HitNormal, FVector PlayerLocation, FVector ForwardVector)
{
	if (!HasAuthority()) return;
	
	if (0 != ResourceRuntimeData.RequiredToolID)
	{
		if (RequiredToolID != ResourceRuntimeData.RequiredToolID)
		{
			UE_LOG(ResourceControlSystem, Warning,TEXT("[GetItemFromResource] RequiredToolID is NOT matching"));
			return;
		}
	}

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
		// 너무 가까우면 플레이어보다 높은 위치로 올림
		float PlayerZ = PlayerLocation.Z;
		float SpawnZ = FinalSpawnLocation.Z;

		// 플레이어보다 위에 있도록 상승
		float DesiredZ = PlayerZ + ExtraUp;
		float AddZ = (DesiredZ - SpawnZ);

		FinalSpawnLocation.Z += AddZ;
	}
	
	// 최종 위치 트랜스폼 생성
	FTransform SpawnTransform = FTransform();
	SpawnTransform.SetLocation(FinalSpawnLocation);
	SpawnTransform.SetRotation(FRotator::ZeroRotator.Quaternion());
	
	// 아이템 생성용 구조체 생성
	FItemData NewDataForNewItem = {};
	
	// 풀 시스템과 아이템에게 넘겨줄 래퍼 구조체 생성
	FSlotStructMaster NewSlotStructMasterForNewItem = {};
	
	// *** 생성 시작 *** //
	
	UItemDataSubsystem* ItemDataSubsystem = UItemDataSubsystem::GetItemDataSubsystem(this);
	if (!IsValid(ItemDataSubsystem))
	{
		UE_LOG(ResourceControlSystem, Error, TEXT("ItemDataSubsystem 찾지 못함."));
		return;
	}
	
	bool bFoundData = ItemDataSubsystem->GetItemDataSafe(ResourceRuntimeData.DropTableID,NewDataForNewItem);
	if (!bFoundData)
	{
		UE_LOG(ResourceControlSystem, Error, TEXT("//============================================//"));
		UE_LOG(ResourceControlSystem, Error, TEXT("%s 가 자원 데이터를 찾는데 실패함."), *GetName());
		UE_LOG(ResourceControlSystem, Error, TEXT("ThisResourceID %d 찾지 못함."), ResourceRuntimeData.DropTableID);
		UE_LOG(ResourceControlSystem, Error, TEXT("//============================================//"));
		return;
	}
	else
	{
		UE_LOG(ResourceControlSystem, Log, TEXT("//============================================//"));
		UE_LOG(ResourceControlSystem, Log, TEXT("%s 가 자원 원천 데이터를 찾는데 성공함."), *GetName());
		UE_LOG(ResourceControlSystem, Log, TEXT("ThisResourceID %d 찾음."), ResourceRuntimeData.DropTableID);
		NewSlotStructMasterForNewItem.ItemData.StaticDataID = NewDataForNewItem.ItemID;
		UE_LOG(ResourceControlSystem, Log, TEXT("NewSlotStructMasterForNewItem.ItemData.StaticDataID %d"), NewSlotStructMasterForNewItem.ItemData.StaticDataID);
	}
	
	// 스폰 로직 >> 이 부분을 루트 컴포넌트에서 해야함!!!!
	/*
	 * 1. 스폰 확률에 따라서 어떤 아이템을 생성할지 결정함.
	 * 2. 총 몇 개 타입의 아이템을, 각각 얼마나 생성할지 결정함.
	 * 3. for 문 돌려서 결정된 아이템들을 수량에 맞게 생성. 
	 * 4. 결정된 데이터를 캐싱 후 스폰이 완료되면 현재 수량에서 깎아야 함.
	 */
	
	// 스폰 요청
	
	bool bSuccess = LootComponent->SpawnLoot(SpawnTransform, PlayerLocation);
	if (!bSuccess) return;
	
	// 성공 시 Count를 깎고 제거 (일단 그냥 깎는 걸로)
	--CurrentItemCount;
	UE_LOG(ResourceControlSystem, Log, TEXT("%s 의 남은 수량 %d"), *GetName(), CurrentItemCount);

	// 만약 현재 수량이 남은 게 없다? 그러면 일단 죽어.
	if (CurrentItemCount <= 0)
	{
		UE_LOG(ResourceControlSystem, Error, TEXT("남은 수량이 없으므로 삭제"));
		
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
				
				UE_LOG(ResourceControlSystem, Log, TEXT("스폰 포인트와 컨트롤 시스템에게 요청 완료"));
			}
			else
			{
				UE_LOG(ResourceControlSystem, Error, TEXT("스폰 포인트와 컨트롤 시스템에게 요청 실패 : 스폰 포인트 찾지 못함."));
			}
		}
		else
		{
			UE_LOG(ResourceControlSystem, Error, TEXT("스폰 포인트와 컨트롤 시스템에게 요청 실패 : 컨트롤 시스템 찾지 못함."));
		}
		Destroy();
	}
	
#pragma region 구 로직 (이제 안 씀)
	//// 월드 아이템 풀에게 우선 요청
	//UWorldItemPoolSubsystem* Pool = GetWorld()->GetSubsystem<UWorldItemPoolSubsystem>();
	//if (!IsValid(Pool))
	//{
	//	UE_LOG(ResourceControlSystem, Error, TEXT("월드 아이템 풀이 유효하지 않음"));
	//	return;
	//}
	//
	// bool bSpawnSuccess = Pool->DropItem(NewSlotStructMasterForNewItem,SpawnTransform, PlayerLocation);
	// if (bSpawnSuccess)
	// {
	// 	UE_LOG(ResourceControlSystem, Error, TEXT("월드 아이템 풀에서 생성이 성공함."));
	// 	
	// 	// 성공 시 Count를 깎고 제거 (일단 그냥 깎는 걸로)
	// 	CurrentItemCount--;
	// 	
	// 	// 만약 현재 수량이 남은 게 없다? 그러면 일단 죽어.
	// 	if (CurrentItemCount <= 0)
	// 	{
	// 		UE_LOG(ResourceControlSystem, Error, TEXT("남은 수량이 없으므로 삭제"));
	// 		Destroy();
	// 	}
	// 	return;
	// }
	// else
	// {
	// 	UE_LOG(ResourceControlSystem, Error, TEXT("월드 아이템 풀이 생성에 실패함."));
	// 	// 실패 시 자체적으로 생성
	// 	FTimerHandle TimerHandle;
	// 	GetWorld()->GetTimerManager().SetTimer(
	// 		TimerHandle,
	// 		FTimerDelegate::CreateLambda([this,NewDataForNewItem, NewSlotStructMasterForNewItem,SpawnTransform]()
	// 		{
	// 			if (!IsValid(this)) return;
	// 			SpawnItem_Internal(NewDataForNewItem, NewSlotStructMasterForNewItem, SpawnTransform);
	// 		}),0.005f,false);
	// }
#pragma endregion
}

void ATSResourceBaseActor::SpawnItem_Internal(const FItemData& ItemDataForMesh, const FSlotStructMaster& ItemData, const FTransform& SpawnTransform)
{
	if (!IsValid(this)) return;
	
	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AWorldItem* SpawnedItem = GetWorld()->SpawnActor<AWorldItem>(AWorldItem::StaticClass(),SpawnTransform,Params);

	if (!IsValid(SpawnedItem))
	{
		UE_LOG(ResourceControlSystem, Error, TEXT("스폰 실패: AWorldItem 생성 불가"));
		return;
	}
	
	// 데이터 주입
	SpawnedItem->SetItemData(ItemData);
	UE_LOG(ResourceControlSystem, Log, TEXT("ItemData.StaticDataID %d"), ItemData.ItemData.StaticDataID);
	
	// 피직스 설정
	UStaticMeshComponent* MeshComp = SpawnedItem->FindComponentByClass<UStaticMeshComponent>();
	if (MeshComp != nullptr)
	{
		UStaticMesh* Mesh = ItemDataForMesh.WorldMesh.LoadSynchronous();
		MeshComp->SetStaticMesh(Mesh);
		MeshComp->SetVisibility(true);
		MeshComp->SetSimulatePhysics(true);
		MeshComp->SetEnableGravity(true);
		MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		MeshComp->SetCollisionResponseToAllChannels(ECR_Block);
		UE_LOG(ResourceControlSystem, Error, TEXT("스폰된 아이템에서 StaticMeshComponent 찾는데 성공하고 스태틱 메쉬 설정 해줌."));
	}
	else
	{
		UE_LOG(ResourceControlSystem, Error, TEXT("스폰된 아이템에서 StaticMeshComponent 찾지 못함."));
	}
	
	// 성공 시 Count를 깎고 제거 (일단 그냥 깎는 걸로)
	CurrentItemCount--;
		
	// 만약 현재 수량이 남은 게 없다? 그러면 일단 죽어.
	if (CurrentItemCount <= 0)
	{
		UE_LOG(ResourceControlSystem, Error, TEXT("남은 수량이 없으므로 삭제"));
		Destroy();
	}
}

void ATSResourceBaseActor::SetMeshComp(UStaticMesh* InResourceStaticMeshComp)
{
	ResourceStaticMeshComp->SetStaticMesh(InResourceStaticMeshComp);
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
