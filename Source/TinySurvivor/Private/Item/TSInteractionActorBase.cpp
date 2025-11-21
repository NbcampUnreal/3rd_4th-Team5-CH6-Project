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
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	MeshComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	MeshComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

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
