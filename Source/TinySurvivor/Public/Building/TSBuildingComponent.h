// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TSBuildingComponent.generated.h"


class UItemDataSubsystem;
struct FBuildingData;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TINYSURVIVOR_API UTSBuildingComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UTSBuildingComponent();
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	// 위젯에서 호출
	UFUNCTION(BlueprintCallable, Category = "Building")
	void StartBuildingMode(int32 BuildingDataID);
	UFUNCTION(BlueprintCallable, Category = "Building")
	void EndBuildingMode();
	// 빌딩모드 진입 여부 리턴
	bool IsBuildingMode() { return bIsBuildingMode; }
	// 좌클릭으로 설치 확정
	void ConfirmPlacement();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	// 프리뷰 메시 생성 및 삭제
	void CreatePreviewMesh(int32 BuildingDataID);
	void DestroyPreviewMesh();
	// 라인 트레이스 및 위치 업데이트
	void UpdatePreviewMesh(float DeltaTime);
	FHitResult BuildingLineTrace();
	// 설치 가능 여부 검증
	bool ValidatePlacement(FHitResult HitResult);
	// 서버에서 액터 스폰
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSpawnBuilding(int32 BuildingDataID, FTransform SpawnTransform);
	// 빌딩 데이터 조회
	bool GetBuildingData(int32 BuildingDataID, FBuildingData& OutBuildingData);
	UItemDataSubsystem* GetItemDataSubsystem() const;

	// 오버랩 체크
	bool CheckOverlap(const FVector& Location, const FVector& Extent);
	// 오버랩 체크 시 사용할 충돌 채널
	UPROPERTY(EditDefaultsOnly, Category = "Building|Settings")
	TEnumAsByte<ECollisionChannel> OverlapChannel = ECC_Pawn;

	// 빌딩 관련 변수
	bool bIsBuildingMode = false;
	bool bCanPlace = false;
	int32 CurrentBuildingDataID = 0;

	// 프리뷰 메시 관련 변수
	UPROPERTY()
	TObjectPtr<UStaticMeshComponent> PreviewMeshComp;
	UPROPERTY(EditDefaultsOnly)
	UMaterialInterface* PreviewMaterial;
	// 마지막 유효한 설치 위치
	FTransform LastTransform;

	mutable UItemDataSubsystem* CachedIDS = nullptr;
};
