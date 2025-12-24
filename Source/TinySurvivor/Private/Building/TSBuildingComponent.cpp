// Fill out your copyright notice in the Description page of Project Settings.

#include "Building/TSBuildingComponent.h"

#include "Engine/World.h"
#include "CollisionQueryParams.h"
#include "AI/Gaint/System/GiantSwitchingResourceArea.h"
#include "Building/System/BuildingRecipeDataSubsystem.h"
#include "GameFramework/Pawn.h"
#include "Inventory/TSInventoryMasterComponent.h"
#include "Building/Actor/TSBuildingActorBase.h"
#include "Item/System/ItemDataSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"
#include "System/Erosion/ErosionLightSourceSubActor.h"

// Sets default values for this component's properties
UTSBuildingComponent::UTSBuildingComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	SetIsReplicatedByDefault(true);
}

void UTSBuildingComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UTSBuildingComponent, bIsBuildingMode);
	DOREPLIFETIME(UTSBuildingComponent, CurrentRecipeID);
	DOREPLIFETIME(UTSBuildingComponent, CurrentBuildingDataID);
	DOREPLIFETIME(UTSBuildingComponent, RotationYaw);
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


void UTSBuildingComponent::ServerStartBuildingMode_Implementation(int32 RecipeID)
{
	int32 BuildingDataID = 0;
	// 재료 확인
	if (!CanBuild(RecipeID, BuildingDataID))
	{
		return;
	}
	// 레시피 결과물 빌딩 ID 확인
	if (BuildingDataID <= 0)
	{
		return;
	}
	// 빌딩 모드 시작
	bIsBuildingMode = true;
	CurrentRecipeID = RecipeID;
	CurrentBuildingDataID = BuildingDataID;
	bCanPlace = false;
	bLastCanPlace = false;

	// 호스트 플레이어인 경우 로컬에서도 프리뷰 메시 생성
	if (GetOwner()->GetInstigatorController() && GetOwner()->GetInstigatorController()->IsLocalController())
	{
		SetComponentTickEnabled(true);
		CreatePreviewMesh(BuildingDataID);
	}
}

bool UTSBuildingComponent::ServerStartBuildingMode_Validate(int32 RecipeID)
{
	return RecipeID > 0;
}

void UTSBuildingComponent::ServerEndBuildingMode_Implementation()
{
	bIsBuildingMode = false;
	bCanPlace = false;
	bLastCanPlace = false;
	CurrentRecipeID = 0;
	CurrentBuildingDataID = 0;
	RotationYaw = 0.f;

	// 호스트 플레이어인 경우 로컬에서도 프리뷰 메시 해제
	if (GetOwner()->GetInstigatorController() && GetOwner()->GetInstigatorController()->IsLocalController())
	{
		SetComponentTickEnabled(false);
		DestroyPreviewMesh();
	}
}

bool UTSBuildingComponent::ServerEndBuildingMode_Validate()
{
	return true;
}

void UTSBuildingComponent::CreatePreviewMesh(int32 BuildingDataID)
{
	// 프리뷰 메시 컴포넌트 제거하고 시작
	if (PreviewMeshComp)
	{
		DestroyPreviewMesh();
	}

	// 빌딩 데이터 조회
	if (!GetBuildingData(BuildingDataID, CachedBuildingData))
	{
		return;
	}

	if (CachedBuildingData.WorldMesh.IsNull())
	{
		return;
	}

	// 프리뷰 메시 컴포넌트 생성
	PreviewMeshComp = NewObject<UStaticMeshComponent>(GetOwner());
	PreviewMeshComp->RegisterComponent();

	// 플레이어 앞쪽에 초기 배치
	FVector ForwardLocation = GetOwner()->GetActorLocation() + GetOwner()->GetActorForwardVector() * 100.f;
	PreviewMeshComp->SetWorldLocation(ForwardLocation);

	PreviewMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 메시 로드
	UStaticMesh* PreviewMesh = CachedBuildingData.WorldMesh.LoadSynchronous();
	if (PreviewMesh)
	{
		PreviewMeshComp->SetStaticMesh(PreviewMesh);
	}

	// CachedDynamicMaterials 비우기
	CachedDynamicMaterials.Empty();

	// DynamicMaterial 생성
	if (PreviewMaterial)
	{
		int32 MaterialCount = PreviewMeshComp->GetNumMaterials();
		for (int32 i = 0; i < MaterialCount; ++i)
		{
			UMaterialInstanceDynamic* DynamicMaterial = PreviewMeshComp->CreateDynamicMaterialInstance(
				i, PreviewMaterial);
			DynamicMaterial->SetVectorParameterValue(FName("Color"),
			                                         bCanPlace ? FLinearColor::Green : FLinearColor::Red);
			CachedDynamicMaterials.Add(DynamicMaterial);
		}
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
	// 로컬 플레이어만 실행
	APlayerController* PC = Cast<APlayerController>(GetOwner()->GetInstigatorController());
	if (!PC || !PC->IsLocalController())
	{
		return;
	}


	FHitResult HitResult = BuildingLineTrace();
	if (!HitResult.IsValidBlockingHit() || !PreviewMeshComp)
	{
		return;
	}

	// 위치, 회전 설정
	LastTransform.SetLocation(HitResult.Location);
	LastTransform.SetRotation(FRotator(0.f, RotationYaw, 0.f).Quaternion());
	// 프리뷰 메시 위치 설정
	PreviewMeshComp->SetWorldTransform(LastTransform);
	// 설치 가능 여부 검증
	bCanPlace = ValidatePlacement(HitResult);
	// 벽 설치인 경우 회전 조정
	if (bCanPlace && CachedBuildingData.bCanAttachToWall)
	{
		FVector WallNormal = HitResult.Normal;
		WallNormal.Normalize();
		RotationYaw = WallNormal.Rotation().Yaw;
		LastTransform.SetRotation(FRotator(0.f, RotationYaw, 0.f).Quaternion());
	}
	// 설치 가능 여부 변경 시 프리뷰 메시 색상 변경
	if (bCanPlace != bLastCanPlace && CachedDynamicMaterials.Num() > 0)
	{
		for (UMaterialInstanceDynamic* DynamicMaterial : CachedDynamicMaterials)
		{
			DynamicMaterial->SetVectorParameterValue(FName("Color"),
			                                         bCanPlace ? FLinearColor::Green : FLinearColor::Red);
		}
		bLastCanPlace = bCanPlace;
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
		PC, ViewportCenter, TraceStart, Forward);
	if (!DeprojectScreenToWorld)
	{
		return FHitResult();
	}

	FVector TraceEnd = TraceStart + Forward * BuildingRange;

	// 충돌 무시 설정
	if (CachedIgnoredActors.IsEmpty())
	{
		GetIgnoredActors();
	}
	FCollisionQueryParams QueryParams;
	for (const auto& IgnoredActor : CachedIgnoredActors)
	{
		QueryParams.AddIgnoredActor(IgnoredActor);
	}

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

	// 어디에 설치하는지 확인
	const float DotProduct = FVector::DotProduct(HitResult.Normal, FVector::UpVector);
	if (CachedBuildingData.bCanAttachToWall)
	{
		// 벽
		if (DotProduct > 0.3f)
		{
			return false;
		}
	}
	else
	{
		// 지면
		if (DotProduct < 0.7f)
		{
			return false;
		}
	}

	// 충돌 체크
	FVector CheckExtent = FVector(100.f, 100.f, 100.f);
	if (PreviewMeshComp && PreviewMeshComp->GetStaticMesh())
	{
		FBoxSphereBounds Bounds = PreviewMeshComp->GetStaticMesh()->GetBounds();
		CheckExtent = Bounds.BoxExtent * 0.9f;
	}

	// 오버랩 체크
	if (!CheckOverlap(HitResult.Location, CheckExtent))
	{
		return false;
	}

	// LightSource 범위 체크 / 라이팅 빌딩 제외
	if (CachedBuildingData.BuildingType != EBuildingType::LIGHT)
	{
		if (!IsInLightSourceRange(HitResult.Location))
		{
			return false;
		}
	}

	return true;
}

bool UTSBuildingComponent::CanBuild(int32 RecipeID, int32& OutResultID)
{
	// 레시피 데이터 조회
	UBuildingRecipeDataSubsystem* BuildingRecipeDataSub =
		UBuildingRecipeDataSubsystem::GetBuildingRecipeDataSubsystem(GetWorld());
	FBuildingRecipeData RecipeData;
	if (!BuildingRecipeDataSub->GetBuildingRecipeDataSafe(RecipeID, RecipeData))
	{
		return false;
	}

	// 결과물 ID 저장
	OutResultID = RecipeData.ResultItemID;

	// 플레이어 인벤토리 확인
	UTSInventoryMasterComponent* PlayerInventoryComp = Cast<UTSInventoryMasterComponent>(
		GetOwner()->GetComponentByClass(UTSInventoryMasterComponent::StaticClass()));
	if (!PlayerInventoryComp)
	{
		return false;
	}

	for (const FBuildingIngredientData& Ingredient : RecipeData.Ingredients)
	{
		// 아이템 개수 확인
		int32 ItemCount = PlayerInventoryComp->GetItemCount(Ingredient.MaterialID);
		if (ItemCount < Ingredient.Count)
		{
			return false;
		}
	}
	return true;
}

bool UTSBuildingComponent::ConsumeIngredients(int32 RecipeID)
{
	if (!GetOwner()->HasAuthority())
	{
		return false;
	}

	// 레시피 데이터 조회
	UBuildingRecipeDataSubsystem* BuildingRecipeDataSub =
		UBuildingRecipeDataSubsystem::GetBuildingRecipeDataSubsystem(GetWorld());
	FBuildingRecipeData RecipeData;
	if (!BuildingRecipeDataSub->GetBuildingRecipeDataSafe(RecipeID, RecipeData))
	{
		return false;
	}

	// 플레이어 인벤토리 확인
	UTSInventoryMasterComponent* PlayerInventoryComp = Cast<UTSInventoryMasterComponent>(
		GetOwner()->GetComponentByClass(UTSInventoryMasterComponent::StaticClass()));
	if (!PlayerInventoryComp)
	{
		return false;
	}

	// 재료 아이템 소비
	for (const FBuildingIngredientData& Ingredient : RecipeData.Ingredients)
	{
		PlayerInventoryComp->ConsumeItem(Ingredient.MaterialID, Ingredient.Count);
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
	ServerEndBuildingMode();
}

void UTSBuildingComponent::ServerRotateBuilding_Implementation(float InputValue)
{
	if (FMath::Abs(InputValue) < 0.01f)
	{
		return;
	}
	RotationYaw += FMath::Sign(InputValue) * 30.f;
	UE_LOG(LogTemp, Error, TEXT("[ROTATE] Input: %f, Current RotationYaw: %f"), InputValue, RotationYaw);
}

bool UTSBuildingComponent::ServerRotateBuilding_Validate(float InputValue)
{
	if (!bIsBuildingMode)
	{
		return false;
	}
	return true;
}

void UTSBuildingComponent::ServerSpawnBuilding_Implementation(int32 BuildingDataID, FTransform SpawnTransform)
{
	// 재료 소비
	if (!ConsumeIngredients(CurrentRecipeID))
	{
		return;
	}
	// 빌딩 액터 스폰
	if (!GetBuildingData(BuildingDataID, CachedBuildingData))
	{
		return;
	}
	if (!CachedBuildingData.ActorClass)
	{
		return;
	}
	// 액터 지연 스폰
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = GetOwner();
	SpawnParams.Instigator = Cast<APawn>(GetOwner());
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	ATSBuildingActorBase* SpawnedActor = GetWorld()->SpawnActorDeferred<ATSBuildingActorBase>(
		CachedBuildingData.ActorClass,
		SpawnTransform,
		SpawnParams.Owner,
		SpawnParams.Instigator,
		SpawnParams.SpawnCollisionHandlingOverride
	);
	// 액터 속성 설정
	if (SpawnedActor)
	{
		SpawnedActor->InitializeFromBuildingData(CachedBuildingData, BuildingDataID);
		SpawnedActor->FinishSpawning(SpawnTransform);
	}
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

void UTSBuildingComponent::GetIgnoredActors()
{
	CachedIgnoredActors.Empty();
	// 플레이어 자신 무시
	CachedIgnoredActors.Add(GetOwner());
	// GiantSwitchResourceArea 무시
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGiantSwitchingResourceArea::StaticClass(), CachedIgnoredActors);
}

bool UTSBuildingComponent::IsInLightSourceRange(const FVector& Location) const
{
	// 무시할 액터 설정
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(GetOwner());

	// 특정 반경 내의 LightSource 찾기
	TArray<AActor*> FoundActors;

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn)); // Pawn
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic)); // 동적 오브젝트 (구조물)
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic)); // 정적 오브젝트

	UKismetSystemLibrary::SphereOverlapActors(
		GetWorld(),
		Location,
		LightSourceDetectionRadius,
		ObjectTypes,
		nullptr,
		IgnoreActors,
		FoundActors);
	for (AActor* Actor : FoundActors)
	{
		AErosionLightSourceSubActor* LightSource = Cast<AErosionLightSourceSubActor>(Actor);
		if (!LightSource)
		{
			continue;
		}
		if (LightSource->GetLightRadius() >= FVector::Dist(Location, LightSource->GetActorLocation()))
		{
			float LightIntensity = LightSource->GetLightscale();
			if (LightIntensity > 0.f)
			{
				return true;
			}
		}
	}
	return false;
}

void UTSBuildingComponent::OnRep_IsBuildingMode()
{
	// 로컬 컨트롤러 체크
	if (!GetOwner()->GetInstigatorController() || !GetOwner()->GetInstigatorController()->IsLocalController())
	{
		return;
	}
	// 클라이언트에서 상태 변경 시 프리뷰 메시 처리
	if (bIsBuildingMode)
	{
		SetComponentTickEnabled(true);
		CreatePreviewMesh(CurrentBuildingDataID);
	}
	else
	{
		SetComponentTickEnabled(false);
		DestroyPreviewMesh();
	}
}
