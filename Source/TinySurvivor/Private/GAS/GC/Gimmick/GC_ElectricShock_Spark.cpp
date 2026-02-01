// GC_ElectricShock_Spark.cpp
#include "GAS/GC/Gimmick/GC_ElectricShock_Spark.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "GameFramework/Character.h"
#include "GameplayTags/GameplayCueTags.h"

DEFINE_LOG_CATEGORY_STATIC(LogGCElectricShockSpark, Log, All); // 로그 카테고리 정의

AGC_ElectricShock_Spark::AGC_ElectricShock_Spark()
{
	/*
		GameplayCueTag 명시적 지정
		꼭 필요한 방어 코드: 생성자에서 명시적으로 태그를 지정 -> 안정성 확보
		C++ GameplayCue는 클래스명 + 태그 불일치 시 동작 안 하는 버그가 존재
	*/
	GameplayCueTag = CueTags::TAG_GameplayCue_ElectricShock_Spark;
	
	PrimaryActorTick.bCanEverTick = false;
}

bool AGC_ElectricShock_Spark::OnActive_Implementation(
	AActor* MyTarget,
	const FGameplayCueParameters& Parameters)
{
	if (!MyTarget || !SparkEffect)
	{
		return false;
	}
	
	// 캐릭터의 루트 또는 특정 소켓에 붙이기
	USceneComponent* AttachComponent = MyTarget->GetRootComponent();
	
	// Character라면 Mesh의 특정 소켓에 붙일 수도 있음
	if (ACharacter* Character = Cast<ACharacter>(MyTarget))
	{
		AttachComponent = Character->GetMesh();
	}
	
	if (!AttachComponent)
	{
		return false;
	}
	
	// 나이아가라 컴포넌트 생성 및 부착
	SparkComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
		SparkEffect,
		AttachComponent,
		AttachSocketName,
		LocationOffset,
		FRotator::ZeroRotator,
		FVector(SparkScale),
		EAttachLocation::KeepRelativeOffset,
		true, // Auto Destroy
		ENCPoolMethod::None,
		true  // Auto Activate
	);
	
	if (SparkComponent)
	{
		UE_LOG(LogGCElectricShockSpark, Log, TEXT("스파크 효과가 성공적으로 생성되었습니다"));
	}
	else
	{
		UE_LOG(LogGCElectricShockSpark, Error, TEXT("스파크 이펙트 생성 실패"));
	}
	
	return SparkComponent != nullptr;
}

bool AGC_ElectricShock_Spark::OnRemove_Implementation(
	AActor* MyTarget,
	const FGameplayCueParameters& Parameters)
{
	if (SparkComponent && SparkComponent->IsActive())
	{
		SparkComponent->Deactivate();
		SparkComponent = nullptr;
	}
	
	return true;
}