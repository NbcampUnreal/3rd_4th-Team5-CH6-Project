// GC_Poison_Bubble.cpp
#include "GAS/GC/Gimmick/GC_Poison_Bubble.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "GameFramework/Character.h"
#include "GameplayTags/GameplayCueTags.h"

AGC_Poison_Bubble::AGC_Poison_Bubble()
{
	/*
		GameplayCueTag 명시적 지정
		꼭 필요한 방어 코드: 생성자에서 명시적으로 태그를 지정 -> 안정성 확보
		C++ GameplayCue는 클래스명 + 태그 불일치 시 동작 안 하는 버그가 존재
	*/
	GameplayCueTag = CueTags::TAG_GameplayCue_Poison_Bubble;
	
	PrimaryActorTick.bCanEverTick = false;
}

bool AGC_Poison_Bubble::OnActive_Implementation(
	AActor* MyTarget,
	const FGameplayCueParameters& Parameters)
{
	if (!MyTarget || !BubbleEffect)
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
	BubbleComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
		BubbleEffect,
		AttachComponent,
		"pelvis",//NAME_None, // 소켓 이름
		LocationOffset,
		FRotator::ZeroRotator,
		FVector(BubbleScale),
		EAttachLocation::KeepRelativeOffset,
		true, // Auto Destroy
		ENCPoolMethod::None,
		true  // Auto Activate
	);
	
	return BubbleComponent != nullptr;
}

bool AGC_Poison_Bubble::OnRemove_Implementation(
	AActor* MyTarget,
	const FGameplayCueParameters& Parameters)
{
	if (BubbleComponent && BubbleComponent->IsActive())
	{
		BubbleComponent->Deactivate();
		BubbleComponent = nullptr;
	}
	
	return true;
}