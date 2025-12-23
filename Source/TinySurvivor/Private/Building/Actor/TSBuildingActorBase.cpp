// Fill out your copyright notice in the Description page of Project Settings.


#include "Building/Actor/TSBuildingActorBase.h"

#include "NiagaraFunctionLibrary.h"
#include "Character/TSCharacter.h"
#include "Components/TextBlock.h"
#include "Components/WidgetComponent.h"
#include "GameplayTags/ItemGameplayTags.h"
#include "Item/System/ItemDataSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

class UTextBlock;
// Sets default values
ATSBuildingActorBase::ATSBuildingActorBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComponent;
	// 충돌 설정
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	MeshComponent->SetCollisionObjectType(ECC_WorldDynamic);
	MeshComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	// 응답 설정
	MeshComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	MeshComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	MeshComponent->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	MeshComponent->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
	MeshComponent->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECR_Block);

	InteractionWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractionWidget"));
	InteractionWidget->SetupAttachment(RootComponent);
	InteractionWidget->SetWidgetSpace(EWidgetSpace::Screen);
	InteractionWidget->SetDrawSize(FVector2D(300.f, 60.f));
	InteractionWidget->SetVisibility(false);
	InteractionWidget->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ATSBuildingActorBase::BeginPlay()
{
	// 레벨 배치 액터 초기화 (서버에서만)
	if (HasAuthority() && ItemInstance.StaticDataID != 0 && ItemInstance.CurrentDurability == -1)
	{
		FBuildingData BuildingInfo;
		if (CachedIDS && CachedIDS->GetBuildingDataSafe(ItemInstance.StaticDataID, BuildingInfo))
		{
			// 빌딩 컴포넌트에서 지연 스폰할때처럼 초기화
			InitializeFromBuildingData(BuildingInfo, ItemInstance.StaticDataID);
		}
	}
	// 상호작용 위젯 설정
	if (InteractionWidget && InteractionWidgetClass)
	{
		InteractionWidget->SetWidgetClass(InteractionWidgetClass);
	}

	Super::BeginPlay();
}

void ATSBuildingActorBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	// IDS 캐싱
	if (UGameInstance* GI = GetWorld()->GetGameInstance())
	{
		CachedIDS = GI->GetSubsystem<UItemDataSubsystem>();
	}


#if WITH_EDITOR
	// 충돌 설정 디버그 출력
	if (MeshComponent)
	{
		ECollisionEnabled::Type CollisionType = MeshComponent->GetCollisionEnabled();
		ECollisionChannel ObjectType = MeshComponent->GetCollisionObjectType();

		UE_LOG(LogTemp, Warning, TEXT("[%s] Collision Enabled: %d, ObjectType: %d"),
		       *GetName(), (int32)CollisionType, (int32)ObjectType);

		UE_LOG(LogTemp, Warning, TEXT("[%s] Response to Pawn: %d"),
		       *GetName(), (int32)MeshComponent->GetCollisionResponseToChannel(ECC_Pawn));
	}

	// 태그 출력
	for (const FName& Tag : Tags)
	{
		UE_LOG(LogTemp, Log, TEXT("[%s] Has Tag: %s"), *GetName(), *Tag.ToString());
	}
#endif
}

void ATSBuildingActorBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ATSBuildingActorBase, ItemInstance);
}

void ATSBuildingActorBase::InitializeFromBuildingData(const FBuildingData& BuildingInfo, const int32 StaticDataID)
{
	// 서버에서 빌딩 데이터로 멤버 변수 업데이트
	if (HasAuthority())
	{
		// 빌딩 데이터 ID 설정
		ItemInstance.StaticDataID = StaticDataID;
		// 내구도 설정
		ItemInstance.CurrentDurability = BuildingInfo.MaxDurability;
		// 생성 시간 설정
		ItemInstance.CreationServerTime = GetWorld()->GetTimeSeconds();
		// 메쉬 설정
		InitializeMesh(BuildingInfo);
		if (!BuildingInfo.bIsSurface && !Tags.Contains(FName("BlockBuilding")))
		{
			// 빌딩 오버랩 감지용 태그 추가
			Tags.Add(FName("BlockBuilding"));
		}
	}
}

void ATSBuildingActorBase::DamageDurability(UAbilitySystemComponent* ASC, float DamageAmount)
{
	if (!HasAuthority())
	{
		return;
	}
	// 내구도 업데이트
	ItemInstance.CurrentDurability -= (int)DamageAmount;

	// 플레이어면 공격한 아이템 내구도 업데이트
	if (ASC->GetAvatarActor()->IsA(ATSCharacter::StaticClass()))
	{
		SendItemDurabilityEvent(ASC);
	}

	// 내구도 0 이하이면 Destroy
	if (ItemInstance.CurrentDurability <= 0)
	{
		Multicast_PlayDestroyEffect();
		SetLifeSpan(0.5f);
	}
}

void ATSBuildingActorBase::ShowInteractionWidget(ATSCharacter* InstigatorCharacter)
{
	if (!InteractionWidget)
	{
		return;
	}
	InteractionWidget->SetVisibility(true);
	if (CanInteract(InstigatorCharacter))
	{
		SetInteractionText(InteractionText);
	}
	else
	{
		SetInteractionText(DisabledText);
	}
}

void ATSBuildingActorBase::HideInteractionWidget()
{
	if (!InteractionWidget)
	{
		return;
	}
	InteractionWidget->SetVisibility(false);
}

void ATSBuildingActorBase::SetInteractionText(FText WidgetText)
{
	UUserWidget* Widget = InteractionWidget->GetWidget();
	if (Widget)
	{
		UTextBlock* TextBlock = Cast<UTextBlock>(Widget->GetWidgetFromName(TEXT("InteractionText")));
		if (TextBlock)
		{
			TextBlock->SetText(WidgetText);
		}
	}
}

bool ATSBuildingActorBase::CanInteract(ATSCharacter* InstigatorCharacter)
{
	return true;
}

void ATSBuildingActorBase::Interact(ATSCharacter* InstigatorCharacter)
{
}

bool ATSBuildingActorBase::RunOnServer()
{
	return true;
}

void ATSBuildingActorBase::OnRep_ItemInstance()
{
	if (LastStaticDataID != ItemInstance.StaticDataID)
	{
		// 빌딩 데이터 가져오기
		FBuildingData BuildingInfo;
		if (CachedIDS && CachedIDS->GetBuildingDataSafe(ItemInstance.StaticDataID, BuildingInfo))
		{
			InitializeMesh(BuildingInfo);
		}
		LastStaticDataID = ItemInstance.StaticDataID;
	}
}

void ATSBuildingActorBase::InitializeMesh(const FBuildingData& BuildingInfo)
{
	// 메쉬 설정
	if (MeshComponent && !BuildingInfo.WorldMesh.IsNull())
	{
		UStaticMesh* Mesh = BuildingInfo.WorldMesh.LoadSynchronous();
		if (Mesh)
		{
			MeshComponent->SetStaticMesh(Mesh);
		}
		// 메시 높이에 따라 위젯 위치 설정
		float WidgetHeight = MeshComponent->Bounds.GetBox().GetExtent().Z;
		InteractionWidget->SetRelativeLocation(FVector(0.f, 0.f, WidgetHeight));
	}
}

void ATSBuildingActorBase::SendItemDurabilityEvent(UAbilitySystemComponent* ASC)
{
	// ASC 이벤트 태그 전송 
	FGameplayEventData EventData;
	EventData.EventTag = ItemTags::TAG_Event_Item_Tool_Harvest;
	EventData.EventMagnitude = 0.0f;
	EventData.Instigator = ASC->GetAvatarActor();
	EventData.Target = ASC->GetAvatarActor();
	ASC->HandleGameplayEvent(ItemTags::TAG_Event_Item_Tool_Harvest, &EventData);
}

void ATSBuildingActorBase::Multicast_PlayDestroyEffect_Implementation() const
{
	//1. 메시 숨기기
	MeshComponent->SetVisibility(false);
	MeshComponent->SetSimulatePhysics(false);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	// 2. 이펙트 재생
	if (DestroyEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), DestroyEffect, GetActorLocation());
	}
	// 3. 사운드 재생
	if (DestroySound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, DestroySound, GetActorLocation());
	}
}
