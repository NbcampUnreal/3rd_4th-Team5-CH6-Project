// WorldItem.cpp
/*
	===============================================================================
	[ FILE MODIFICATION NOTICE - DECAY SYSTEM INTEGRATION ]
	작성자: 양한아
	날짜: 2025/11/21
	
	본 파일에는 '부패(Decay) 시스템'을 통합하기 위한 변경이 포함되어 있습니다.
	해당 변경들은 모두 아래 표기된 주석 블록 내부에 위치합니다:
	
		// ■ Decay
		//[S]=====================================================================================
			(Decay 관련 통합 코드)
		//[E]=====================================================================================
		
	위 영역 외의 기존 풀링/스폰/인스턴싱 로직은 변경하지 않았습니다.
	Decay 시스템만 연동한 최소 변경입니다.
	후속 작업 시 해당 블록을 참고해주세요.
	===============================================================================
*/

#include "Item/WorldItem.h"
#include "Net/UnrealNetwork.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Item/System/ItemDataSubsystem.h"
#include "Item/Data/ItemData.h"
#include "Item/System/ItemDataSubsystem.h"
// ■ Decay
//[S]=====================================================================================
#include "Item/Runtime/DecayManager.h"
//[E]=====================================================================================
#include "Kismet/GameplayStatics.h"

AWorldItem::AWorldItem()
{
	bReplicates = true;
	// 클라이언트가 이 액터를 스폰할 수 있도록 설정
	SetReplicatingMovement(true);
	
	if (!RootComponent)
		RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);
	// 시작시 물리 시뮬레이션 비활성화 (풀에서 꺼낼 때 켤 수 있음)
	MeshComponent->SetSimulatePhysics(false);

	// 상호작용 콜리전 생성 (줍기 감지용)
	InteractionCollision = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionCollision"));
	InteractionCollision->SetupAttachment(RootComponent);
	InteractionCollision->SetSphereRadius(150.0f);	// 줍기 반경 (임시)

	// 생성자에서 콜리전을 비활성화
	InteractionCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	InteractionCollision->SetGenerateOverlapEvents(false);
	
	// 오버램 이벤트 바인딩 (서버에서만)
	if (HasAuthority())
	{
		InteractionCollision->OnComponentBeginOverlap.AddDynamic(this, &AWorldItem::OnInteractionOverlap);
		InteractionCollision->OnComponentBeginOverlap.AddDynamic(this, &AWorldItem::OnInteractionEndOverlap);
	}
	
	// [디버그] 텍스트 렌더 컴포넌트 추가
	DebugTextComp = CreateDefaultSubobject<UTextRenderComponent>(TEXT("DebugTextComp"));
	DebugTextComp->SetupAttachment(RootComponent);
	DebugTextComp->SetRelativeLocation(FVector(0, 0, 100.0f)); // 아이템 머리 위 1m
	DebugTextComp->SetHorizontalAlignment(EHTA_Center);
	DebugTextComp->SetTextRenderColor(FColor::Red);
	DebugTextComp->SetWorldSize(20.0f); // 글자 크기
	DebugTextComp->SetVisibility(true); // 항상 보이게
}

// 어떤 변수를 클라이언트로 복제할 지 설정
void AWorldItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// ItemData 변수가 변경될 때마다 클라이언트로 복제하고 OnRep_ItemData 함수 호출
	DOREPLIFETIME(AWorldItem, ItemData);
}

// 풀에서 액터를 꺼낼 때 자동으로 호출
void AWorldItem::OnAcquire_Implementation(const int32& IntParam, const FString& StringParam, const UObject* ObjectParam)
{
	Super::OnAcquire_Implementation(IntParam, StringParam, ObjectParam);
	
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	
	if (MeshComponent)
	{
		MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		MeshComponent->SetSimulatePhysics(false); 
	}
	
	if (InteractionCollision)
	{
		InteractionCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		InteractionCollision->SetGenerateOverlapEvents(true);
	}
	
	// ■ Decay
	//[S]=====================================================================================
	if (HasAuthority())
	{
		InitializeDecaySystem();
		
		if (!DecayManager)
		{
			DecayManager = GetWorld()->GetSubsystem<UDecayManager>();
		}
		
		if (DecayManager)
		{
			if (DecayManager->OnDecayTick.IsAlreadyBound(this, &AWorldItem::OnDecayTick))
			{
				DecayManager->OnDecayTick.RemoveDynamic(this, &AWorldItem::OnDecayTick);	
			}
			
			DecayManager->OnDecayTick.AddDynamic(this, &AWorldItem::OnDecayTick);
		}
	}
	//[E]=====================================================================================
	
	// 아이템 효과 로직
}

// 풀로 액터를 반납할 때 자동으로 호출
void AWorldItem::OnRelease_Implementation()
{
	Super::OnRelease_Implementation();
	
	// ■ Decay
	//[S]=====================================================================================
	if (HasAuthority() && DecayManager)
	{
		DecayManager->OnDecayTick.RemoveDynamic(this, &AWorldItem::OnDecayTick);
	}
	else
	{
		if (UDecayManager* Sys = GetWorld()->GetSubsystem<UDecayManager>())
		{
			Sys->OnDecayTick.RemoveDynamic(this, &AWorldItem::OnDecayTick);
		}
	}
	//[E]=====================================================================================

	SetActorHiddenInGame(true);
	SetActorEnableCollision(false); // 액터 자체 충돌 끄기
    
	// 컴포넌트들도 확실하게 끄기
	if (InteractionCollision)
	{
		InteractionCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
    
	if (MeshComponent)
	{
		MeshComponent->SetStaticMesh(nullptr);
		MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	ItemData = FSlotStructMaster();
	SourceInstanceIndex = -1;
	
	// 시스템 해제. 부패 매니저 등 구독 해제
	if (HasAuthority() && DecayManager)
	{
		DecayManager->OnDecayTick.RemoveDynamic(this, &AWorldItem::OnDecayTick);
	}	
}

// 풀에서 꺼낸 액터에게 어떤 아이템인지 데이터 주입
void AWorldItem::SetItemData(const FSlotStructMaster& NewItemData)
{
	ItemData = NewItemData;

	// 이 함수는 서버에서만 호출
	if (HasAuthority())
	{
		// ■ Decay
		//[S]=====================================================================================
		InitializeDecaySystem();
		//[E]=====================================================================================
		
		// 외형 업데이트
		UpdateAppearance();
		
		// 디버그
		UpdateDebugText();
	}
}

// 클라이언트에서 ItemData 변수가 복제되었을 때 호출
void AWorldItem::OnRep_ItemData()
{
	UpdateAppearance();
	
	// 디버그
	UpdateDebugText();
}

// ItemData.StaticDataID를 이용해 메시를 찾아 비동기 로드하고 적용
void AWorldItem::UpdateAppearance()
{
	if (!MeshComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("[Appearance] MeshComponent is NULL!"));
		return;	
	}

	// 진행 중이던 이전 로드 요청이 있으면 취소
	if (MeshLoadHandle.IsValid())
	{
		MeshLoadHandle->CancelHandle();
		MeshLoadHandle.Reset();
	}
	
	// ID가 유효한지 확인 (음수이면 빈 슬롯으로 간주)
	if (ItemData.ItemData.StaticDataID <= 0)
	{
		UE_LOG(LogTemp, Log, TEXT("[Appearance] ItemID is 0 or Invalid. Clearing Mesh."));
		MeshComponent->SetStaticMesh(nullptr);
		return;
	}
	
	// 데이터 테이블 서브시스템을 가져옵니다
	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(this);
	if (!GameInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("UpdateAppearance : GameInstance not found"));
		MeshComponent->SetStaticMesh(nullptr);
		return;
	}
	
	UItemDataSubsystem* ItemDataSubsystem = GameInstance->GetSubsystem<UItemDataSubsystem>();
	if (!ItemDataSubsystem || !ItemDataSubsystem->IsInitialized())
	{
		UE_LOG(LogTemp, Error, TEXT("UpdateAppearance : ItemDataSubsystem not found or not initialized"));
		MeshComponent->SetStaticMesh(nullptr);
		return;
	}
	
	// ID로 FItemData를 찾습니다
	FItemData StaticItemData;
	if (ItemDataSubsystem->GetItemDataSafe(ItemData.ItemData.StaticDataID, StaticItemData))
	{
		// WorldMesh 변수가 유효한 지 확인
		if (StaticItemData.IsWorldMeshValid())
		{
			TSoftObjectPtr<UStaticMesh> MeshToLoad = StaticItemData.WorldMesh;
			TWeakObjectPtr<AWorldItem> WeakThis(this);
	
			FStreamableManager& StreamableManager = UAssetManager::Get().GetStreamableManager();
			MeshLoadHandle = StreamableManager.RequestAsyncLoad(MeshToLoad.ToSoftObjectPath(), [WeakThis, MeshToLoad]()
			{
				if (!WeakThis.IsValid())
					return;
				if (MeshToLoad.IsValid())
					WeakThis->MeshComponent->SetStaticMesh(MeshToLoad.Get());
			});
		}
		else
		{
			// FItemData를 못 찾은 경우
			UE_LOG(LogTemp, Warning, TEXT("AWorldItem : Could not find StaticDataID '%d' in Data Table"), ItemData.ItemData.StaticDataID);
			MeshComponent->SetStaticMesh(nullptr);
		}
	}
}

// 플레이어가 줍기 콜리전에 들어왓을 때 서버에서만 호출
void AWorldItem::OnInteractionOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 서버에서만 실행됨
	// TODO:
	// 1. OtherActor가 줍기 가능한 플레이어(Pawn)인지 확인
	// 2. 플레이어라면 줍기 UI 표시 요청 (플레이어 컨트롤러/HUD로 알림)
	// 3. 플레이어가 줍기 키를 누르면 (이 함수가 아닌 별도 RPC로)
	// 4.   해당 플레이어의 InventoryComponent->TryAddItem(this->ItemData) 호출
	// 5.   TryAddItem이 true를 반환하면 (줍기 성공)
	// 6.      UWorldItemPoolSubsystem* PoolSubsystem = GetWorld()->GetSubsystem<UWorldItemPoolSubsystem>();
	// 7.      PoolSubsystem->ReleaseItemActor(this); // 풀에 반납
	
	UE_LOG(LogTemp, Warning, TEXT("AWorldItem overlapped by %s"), *OtherActor->GetName());
}

void AWorldItem::OnInteractionEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
}

// ■ Decay
//[S]=====================================================================================
void AWorldItem::BeginPlay()
{
	Super::BeginPlay();
	
	// 서버에서만 DecayManager 구독
	if (HasAuthority())
	{
		DecayManager = GetWorld()->GetSubsystem<UDecayManager>();
		if (DecayManager)
		{
			DecayManager->OnDecayTick.AddDynamic(this, &AWorldItem::OnDecayTick);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[WorldItem]: DecayManager not found!"));
		}
	}
}

void AWorldItem::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// 구독 해제
	if (HasAuthority() && DecayManager)
	{
		DecayManager->OnDecayTick.RemoveDynamic(this, &AWorldItem::OnDecayTick);
	}
	
	Super::EndPlay(EndPlayReason);
}

void AWorldItem::OnDecayTick()
{
	// ItemData가 비어있으면 처리 안 함
	if (ItemData.ItemData.StaticDataID <= 0)
	{
		return;
	}
	
	// 부패 가능한 아이템인지 확인
	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(this);
	if (!GameInstance)
	{
		return;
	}
	
	UItemDataSubsystem* ItemDataSubsystem = GameInstance->GetSubsystem<UItemDataSubsystem>();
	if (!ItemDataSubsystem || !ItemDataSubsystem->IsInitialized())
	{
		return;
	}
	
	FItemData StaticItemData;
	if (!ItemDataSubsystem->GetItemDataSafe(ItemData.ItemData.StaticDataID, StaticItemData))
	{
		return;
	}
	
	// 부패 활성화되지 않은 아이템은 처리 안 함
	if (!StaticItemData.IsDecayEnabled())
	{
		return;
	}
	
	// 부패 진행도 계산
	float CurrentTime = GetWorld()->GetTimeSeconds();
	
	// ExpirationTime이 설정되지 않았으면 초기화
	if (ItemData.ExpirationTime <= 0)
	{
		ItemData.ExpirationTime = ItemData.ItemData.CreationServerTime + StaticItemData.ConsumableData.DecayRate;
	}
	
	// 부패 완료 체크
	if (CurrentTime >= ItemData.ExpirationTime)
	{
		ConvertToDecayedItem();
	}
	else
	{// UI 표시용 부패 진행도 갱신 (0~1)
		float DecayRate = StaticItemData.ConsumableData.DecayRate;
		if (DecayRate > 0)
		{
			ItemData.CurrentDecayPercent = (ItemData.ExpirationTime - CurrentTime) / DecayRate;
		}
	}
	
	if (HasAuthority())
	{
		UpdateDebugText();
	}
}

void AWorldItem::ConvertToDecayedItem()
{
	if (!HasAuthority() || !DecayManager)
	{
		return;
	}
	
	// 부패물 ID 가져오기
	int32 DecayedItemID = DecayManager->GetDecayItemID();
	
	UE_LOG(LogTemp, Log, TEXT("AWorldItem: Item %d decayed to %d"), ItemData.ItemData.StaticDataID, DecayedItemID);
	
	// 아이템 데이터를 부패물로 교체
	ItemData.ItemData.StaticDataID = DecayedItemID;
	ItemData.ExpirationTime = 0; // 부패물은 더 이상 부패 안 함
	ItemData.CurrentDecayPercent = 0.f;
	
	// 외형 업데이트 (리플리케이션으로 클라이언트도 자동 갱신)
	UpdateAppearance();
}

void AWorldItem::InitializeDecaySystem()
{
	// ■ Decay
	//[S]=====================================================================================
	// 부패 만료 시간 초기화 (서버에서만)
	if (!HasAuthority())
	{
		return;
	}
	
	const int32 StaticID = ItemData.ItemData.StaticDataID;
	if (StaticID <= 0)
	{
		return;
	}
	
	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(this);
	if (!GameInstance)
	{
		return;
	}
	
	UItemDataSubsystem* ItemDataSubsystem = GameInstance->GetSubsystem<UItemDataSubsystem>();
	if (!ItemDataSubsystem || !ItemDataSubsystem->IsInitialized())
	{
		return;
	}
	
	FItemData StaticItemData;
	if (!ItemDataSubsystem->GetItemDataSafe(StaticID, StaticItemData))
	{
		return;
	}
	
	if (!StaticItemData.IsDecayEnabled())
	{
		return;
	}
	
	// CreationTime 보정 (서브시스템에서 못 했을 경우 대비)
	if (ItemData.ItemData.CreationServerTime <= 0)
	{
		ItemData.ItemData.CreationServerTime = GetWorld()->GetTimeSeconds();
	}
	
	// ExpirationTime이 0이면 (새로 스폰된 경우) 초기화
	if (ItemData.ExpirationTime <= 0)
	{
		ItemData.ExpirationTime = ItemData.ItemData.CreationServerTime + StaticItemData.ConsumableData.DecayRate;
	}
	// 이미 설정된 경우는 유지 (인벤토리에서 드랍된 경우)
	//[E]=====================================================================================	
}

//[E]=====================================================================================

// 헬퍼 함수: 디버그 텍스트 갱신 (헤더에 선언 안 해도 내부에서만 쓸 거면 이렇게 구현 가능, 혹은 따로 만드세요)
void AWorldItem::UpdateDebugText()
{
	if (!DebugTextComp) return;

	FString DebugString = FString::Printf(TEXT("ID: %d"), ItemData.ItemData.StaticDataID);
    
	// 부패 정보 추가
	if (ItemData.ExpirationTime > 0)
	{
		float RemainTime = ItemData.ExpirationTime - GetWorld()->GetTimeSeconds();
		DebugString.Append(FString::Printf(TEXT("\nDecay: %.1f sec (%.0f%%)"), RemainTime, ItemData.CurrentDecayPercent * 100.0f));
	}
	else
	{
		DebugString.Append(TEXT("\nNo Decay"));
	}

	// 소스 인덱스 표시 (풀링 디버깅용)
	DebugString.Append(FString::Printf(TEXT("\nSrcIdx: %d"), SourceInstanceIndex));

	DebugTextComp->SetText(FText::FromString(DebugString));
}