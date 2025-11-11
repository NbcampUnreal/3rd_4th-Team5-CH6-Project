// WorldItem.cpp


#include "Item/WorldItem.h"
#include "Net/UnrealNetwork.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Item/System/ItemDataSubsystem.h"
#include "Item/Data/ItemData.h"
#include "Item/System/ItemDataSubsystem.h"
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

	// 생성된 후, 부모에서 콜리전을 켜주지만, 명시적으로 한 번 더 켜줍니다
	InteractionCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractionCollision->SetGenerateOverlapEvents(true);
	// 아이템 효과 로직
}

// 풀로 액터를 반납할 때 자동으로 호출
void AWorldItem::OnRelease_Implementation()
{
	Super::OnRelease_Implementation();

	// 반납할 때, 콜리전 비활성화
	InteractionCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	InteractionCollision->SetGenerateOverlapEvents(false);
	// 추가 비활성화/초기화 로직

	// 진행 중이던 비동기 로딩이 있다면 즉시 취소
	if (MeshLoadHandle.IsValid())
	{
		MeshLoadHandle->CancelHandle();
		MeshLoadHandle.Reset();
	}

	// 메시 컴포넌트 클리어
	if (MeshComponent)
	{
		MeshComponent->SetStaticMesh(nullptr);
		// 물리 시뮬을 켰을 경우 여기서 반드시 해제
	}

	ItemData = FSlotStructMaster();
}

// 풀에서 꺼낸 액터에게 어떤 아이템인지 데이터 주입
void AWorldItem::SetItemData(const FSlotStructMaster& NewItemData)
{
	ItemData = NewItemData;

	// 이 함수는 서버에서만 호출
	if (HasAuthority())
	{
		// 외형 업데이트
		UpdateAppearance();
	}
}

// 클라이언트에서 ItemData 변수가 복제되었을 때 호출
void AWorldItem::OnRep_ItemData()
{
	UpdateAppearance();
}

// ItemData.StaticDataID를 이용해 메시를 찾아 비동기 로드하고 적용
void AWorldItem::UpdateAppearance()
{
	if (!MeshComponent)
		return;

	// 진행 중이던 이전 로드 요청이 있으면 취소
	if (MeshLoadHandle.IsValid())
	{
		MeshLoadHandle->CancelHandle();
		MeshLoadHandle.Reset();
	}

	// ID가 유효한지 확인 (음수이면 빈 슬롯으로 간주)
	if (ItemData.StaticDataID <= 0)
	{
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
	if (ItemDataSubsystem->GetItemDataSafe(ItemData.StaticDataID, StaticItemData))
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
			UE_LOG(LogTemp, Warning, TEXT("AWorldItem : Could not find StaticDataID '%d' in Data Table"), ItemData.StaticDataID);
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
