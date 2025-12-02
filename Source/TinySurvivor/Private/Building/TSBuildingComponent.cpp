// Fill out your copyright notice in the Description page of Project Settings.

#include "Building/TSBuildingComponent.h"

#include "Engine/World.h"
#include "CollisionQueryParams.h"
#include "GameFramework/Pawn.h"
#include "Item/Data/BuildingData.h"
#include "Item/System/ItemDataSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"


// Sets default values for this component's properties
UTSBuildingComponent::UTSBuildingComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

// Called when the game starts
void UTSBuildingComponent::BeginPlay()
{
	Super::BeginPlay();
	// 아이템 데이터 서브시스템 초기화
	CachedIDS = GetItemDataSubsystem();
	if (!CachedIDS)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to get ItemDataSubsystem!"));
	}
}


// Called every frame
void UTSBuildingComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                         FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!bIsBuildingMode)
	{
		return;
	}
	UpdatePreviewMesh(DeltaTime);
}

void UTSBuildingComponent::StartBuildingMode(int32 BuildingDataID)
{
	bIsBuildingMode = true;
	CurrentBuildingDataID = BuildingDataID;
	SetComponentTickEnabled(true);
	CreatePreviewMesh(BuildingDataID);
}

void UTSBuildingComponent::EndBuildingMode()
{
	bIsBuildingMode = false;
	SetComponentTickEnabled(false);
	bCanPlace = false;
	CurrentBuildingDataID = 0;
	DestroyPreviewMesh();
}

void UTSBuildingComponent::CreatePreviewMesh(int32 BuildingDataID)
{
	// 프리뷰 메시 컴포넌트 제거하고 시작
	if (PreviewMeshComp)
	{
		DestroyPreviewMesh();
	}

	// 빌딩 데이터 조회
	FBuildingData BuildingData;
	if (!GetBuildingData(BuildingDataID, BuildingData))
	{
		return;
	}

	if (BuildingData.WorldMesh.IsNull())
	{
		return;
	}

	// 프리뷰 메시 컴포넌트 생성
	PreviewMeshComp = NewObject<UStaticMeshComponent>(GetOwner());
	PreviewMeshComp->RegisterComponent();
	PreviewMeshComp->AttachToComponent(
		GetOwner()->GetRootComponent(),
		FAttachmentTransformRules::KeepRelativeTransform
	);
	PreviewMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 메시 로드
	UStaticMesh* PreviewMesh = BuildingData.WorldMesh.LoadSynchronous();
	if (PreviewMesh)
	{
		PreviewMeshComp->SetStaticMesh(PreviewMesh);
	}
}

void UTSBuildingComponent::DestroyPreviewMesh()
{
	if (PreviewMeshComp)
	{
		PreviewMeshComp->DestroyComponent();
		PreviewMeshComp = nullptr;
	}
}


void UTSBuildingComponent::UpdatePreviewMesh(float DeltaTime)
{
	FHitResult HitResult = BuildingLineTrace();
	if (!HitResult.IsValidBlockingHit() || !PreviewMeshComp)
	{
		return;
	}
	PreviewMeshComp->SetWorldLocation(HitResult.Location);
	bCanPlace = ValidatePlacement(HitResult);
	if (bCanPlace)
	{
		LastTransform = FTransform(FRotator::ZeroRotator, HitResult.Location);
	}
	// 설치 가능 여부로 프리뷰 메시 색상 변경
	if (PreviewMaterial)
	{
		UMaterialInstanceDynamic* DynamicMaterial = PreviewMeshComp->CreateDynamicMaterialInstance(0, PreviewMaterial);
		if (DynamicMaterial)
		{
			DynamicMaterial->SetVectorParameterValue(FName("Color"),
			                                         bCanPlace ? FLinearColor::Green : FLinearColor::Red);
		}
	}
}

FHitResult UTSBuildingComponent::BuildingLineTrace()
{
	APlayerController* PC = Cast<APlayerController>(GetOwner()->GetInstigatorController());
	if (!IsValid(PC) || !PC->IsLocalController())
	{
		return FHitResult();
	}

	// 화면 중앙 가져오기
	FVector2D ViewPortSize = FVector2D::ZeroVector;
	GEngine->GameViewport->GetViewportSize(ViewPortSize);
	const FVector2D ViewportCenter = ViewPortSize / 2.f;

	FVector TraceStart; // 시작 점 
	FVector Forward;

	// 2D 스크린 좌표를 3D 월드 좌표로 변환
	bool DeprojectScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
		GetWorld()->GetFirstPlayerController(), ViewportCenter, TraceStart, Forward);
	if (!DeprojectScreenToWorld)
	{
		return FHitResult();
	}

	FVector TraceEnd = TraceStart + Forward * 500.f;

	// 충돌 무시 설정 (플레이어 자신 무시)
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(GetOwner());

	FHitResult HitResult;
	GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_GameTraceChannel3, QueryParams);
	return HitResult;
}

bool UTSBuildingComponent::ValidatePlacement(FHitResult HitResult)
{
	if (!HitResult.IsValidBlockingHit() || !PreviewMeshComp)
	{
		return false;
	}
	// 지면이 아닌 곳에 설치하는지 확인
	const float DotProduct = FVector::DotProduct(HitResult.Normal, FVector::UpVector);
	if (DotProduct < 0.7f)
	{
		return false;
	}

	// 충돌 체크
	FBuildingData BuildingData;
	FVector CheckExtent = FVector(100.f, 100.f, 100.f);
	if (GetBuildingData(CurrentBuildingDataID, BuildingData))
	{
		if (PreviewMeshComp && PreviewMeshComp->GetStaticMesh())
		{
			FBoxSphereBounds Bounds = PreviewMeshComp->GetStaticMesh()->GetBounds();
			CheckExtent = Bounds.BoxExtent * 0.9f;
		}
	}

	// 오버랩 체크
	if (!CheckOverlap(HitResult.Location, CheckExtent))
	{
		return false;
	}
	return true;
}

void UTSBuildingComponent::ConfirmPlacement()
{
	if (!bIsBuildingMode || !bCanPlace)
	{
		return;
	}
	ServerSpawnBuilding(CurrentBuildingDataID, LastTransform);
	EndBuildingMode();
}

void UTSBuildingComponent::ServerSpawnBuilding_Implementation(int32 BuildingDataID, FTransform SpawnTransform)
{
	// 빌딩 액터 스폰
	FBuildingData BuildingData;
	if (!GetBuildingData(BuildingDataID, BuildingData))
	{
		return;
	}
	if (!BuildingData.ActorClass)
	{
		return;
	}
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = GetOwner();
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(
		BuildingData.ActorClass,
		SpawnTransform,
		SpawnParams
	);
}

bool UTSBuildingComponent::ServerSpawnBuilding_Validate(int32 BuildingDataID, FTransform SpawnTransform)
{
	return BuildingDataID > 0 && SpawnTransform.IsValid();
}

bool UTSBuildingComponent::GetBuildingData(int32 BuildingDataID, FBuildingData& OutBuildingData)
{
	if (BuildingDataID == 0)
	{
		return false;
	}
	if (!CachedIDS)
	{
		CachedIDS = GetItemDataSubsystem();
	}
	if (CachedIDS && CachedIDS->GetBuildingDataSafe(BuildingDataID, OutBuildingData))
	{
		return true;
	}
	return false;
}

UItemDataSubsystem* UTSBuildingComponent::GetItemDataSubsystem() const
{
	if (UGameInstance* GI = GetWorld()->GetGameInstance())
	{
		return GI->GetSubsystem<UItemDataSubsystem>();
	}
	return nullptr;
}

bool UTSBuildingComponent::CheckOverlap(const FVector& Location, const FVector& Extent)
{
	// 설치 가능 여부 리턴

	// 무시할 액터 설정
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(GetOwner());

	TArray<AActor*> OutActors;
	// 여러 ObjectType 체크
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn)); // Pawn
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic)); // 동적 오브젝트 (구조물)
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic)); // 정적 오브젝트

	bool bHasOverlap = UKismetSystemLibrary::BoxOverlapActors(
		GetWorld(),
		Location,
		Extent,
		ObjectTypes,
		nullptr,
		IgnoreActors,
		OutActors);

	if (!bHasOverlap)
	{
		return true;
	}
	for (AActor* OverlappedActor : OutActors)
	{
		if (!OverlappedActor)
		{
			continue;
		}
		// 1. 플레이어 / 몬스터
		if (OverlappedActor->IsA(APawn::StaticClass()))
		{
			return false;
		}

		// 2. 설치된 구조물 (Building 태그 사용)
		if (OverlappedActor->ActorHasTag(FName("BlockBuilding")))
		{
			return false;
		}
	}
	return true;
}
