// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/TSInteractionActorBase.h"
#include "Character/TSCharacter.h"
#include "Components/TextBlock.h"
#include "Components/WidgetComponent.h"
#include "Item/System/ItemDataSubsystem.h"
#include "Net/UnrealNetwork.h"

class UTextBlock;
// Sets default values
ATSInteractionActorBase::ATSInteractionActorBase()
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
	MeshComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block); // 플레이어 통과 못함
	MeshComponent->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap); // 오버랩 감지용!
	MeshComponent->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECR_Ignore); // 지면 트레이스는 무시

	InteractionWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractionWidget"));
	InteractionWidget->SetupAttachment(RootComponent);
	InteractionWidget->SetWidgetSpace(EWidgetSpace::Screen);
	InteractionWidget->SetDrawSize(FVector2D(300.f, 60.f));
	InteractionWidget->SetVisibility(false);
	InteractionWidget->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ATSInteractionActorBase::BeginPlay()
{
	Super::BeginPlay();
	if (HasAuthority() && ItemInstance.CreationServerTime == 0.0)
	{
		ItemInstance.CreationServerTime = GetWorld()->GetTimeSeconds();
	}

	InitializeFromItemData();

	if (InteractionWidget && InteractionWidgetClass)
	{
		InteractionWidget->SetWidgetClass(InteractionWidgetClass);
	}
}

void ATSInteractionActorBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (!Tags.Contains(FName("BlockBuilding")))
	{
		// 빌딩 오버랩 감지용 태그 추가
		Tags.Add(FName("BlockBuilding"));
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

void ATSInteractionActorBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ATSInteractionActorBase, ItemInstance);
}

void ATSInteractionActorBase::ShowInteractionWidget(ATSCharacter* InstigatorCharacter)
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

void ATSInteractionActorBase::HideInteractionWidget()
{
	if (!InteractionWidget)
	{
		return;
	}
	InteractionWidget->SetVisibility(false);
}

void ATSInteractionActorBase::SetInteractionText(FText WidgetText)
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

bool ATSInteractionActorBase::CanInteract(ATSCharacter* InstigatorCharacter)
{
	return true;
}

void ATSInteractionActorBase::Interact(ATSCharacter* InstigatorCharacter)
{
}

bool ATSInteractionActorBase::RunOnServer()
{
	return true;
}

void ATSInteractionActorBase::OnRep_ItemInstance()
{
	InitializeFromItemData();
}

void ATSInteractionActorBase::InitializeFromItemData()
{
	if (ItemInstance.StaticDataID == 0)
	{
		return;
	}

	UGameInstance* GI = GetWorld()->GetGameInstance();
	if (!GI)
	{
		return;
	}

	UItemDataSubsystem* IDS = GI->GetSubsystem<UItemDataSubsystem>();
	if (!IDS)
	{
		return;
	}

	FItemData ItemInfo;
	if (!IDS->GetItemDataSafe(ItemInstance.StaticDataID, ItemInfo))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to get ItemData for ID: %d"), ItemInstance.StaticDataID);
		return;
	}

	if (MeshComponent && ItemInfo.IsWorldMeshValid())
	{
		UStaticMesh* Mesh = ItemInfo.WorldMesh.LoadSynchronous();
		if (Mesh)
		{
			MeshComponent->SetStaticMesh(Mesh);
		}
	}
}
