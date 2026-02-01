// GC_Material_Frankenstein.cpp
#include "GAS/GC/Gimmick/GC_Material_Frankenstein.h"

#include "Components/SkeletalMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "TimerManager.h"

DEFINE_LOG_CATEGORY_STATIC(LogGCMaterialFrankenstein, Log, All);

AGC_Material_Frankenstein::AGC_Material_Frankenstein()
{
	// 기본값 설정 (에디터에서 변경 가능)
	OriginalMaterialPath = FSoftObjectPath(TEXT("/Game/ThirdParty/CardboardWarrior/Materials/Mat_Cardboard_warrior_Body"));
	SkinMaterialPath = FSoftObjectPath(TEXT("/Game/ThirdParty/Cardboard_Boy_SkinPack/Materials/Mat_Cardboard_warrior_Body_skin3"));
	FadeDuration = 0.5f;
	TargetMeshComponentName = NAME_None;
	
	// Actor가 제거되지 않고 재사용되도록 설정
	bAutoDestroyOnRemove = false;
}

bool AGC_Material_Frankenstein::OnActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
	if (!MyTarget)
	{
		UE_LOG(LogGCMaterialFrankenstein, Warning, TEXT("OnActive - MyTarget가 null입니다."));
		return false;
	}
	
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	UE_LOG(LogGCMaterialFrankenstein, Log, TEXT("OnActive - 타겟: %s"),
		*MyTarget->GetName());
#endif
	
	// 스킨 머티리얼 로드
	UMaterialInterface* SkinMaterial = Cast<UMaterialInterface>(SkinMaterialPath.TryLoad());
	if (!SkinMaterial)
	{
		UE_LOG(LogGCMaterialFrankenstein, Error,
			TEXT("OnActive - 경로 %s에서 스킨 머티리얼 로드 실패"),
			*SkinMaterialPath.ToString());
		return false;
	}
	
	ExecuteFadeIn(MyTarget, SkinMaterial);
	return true;
}

bool AGC_Material_Frankenstein::OnRemove_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
	if (!MyTarget)
	{
		UE_LOG(LogGCMaterialFrankenstein, Warning, TEXT("OnRemove - MyTarget가 null입니다."));
		return false;
	}
	
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	UE_LOG(LogGCMaterialFrankenstein, Log, TEXT("OnRemove - 타겟: %s"), *MyTarget->GetName());
#endif
	
	ExecuteFadeOut(MyTarget);
	return true;
}

void AGC_Material_Frankenstein::ExecuteFadeIn(AActor* TargetActor, UMaterialInterface* SkinMaterial)
{
	if (!TargetActor || !SkinMaterial)
	{
		return;
	}
	
	// 기존 페이드 정보가 있으면 타이머 정리
	if (FMaterialFadeInfo_Frankenstein* ExistingInfo = ActorFadeInfoMap.Find(TargetActor))
	{
		if (ExistingInfo->FadeInHandle.IsValid())
		{
			GetWorldTimerManager().ClearTimer(ExistingInfo->FadeInHandle);
		}
		if (ExistingInfo->FadeOutHandle.IsValid())
		{
			GetWorldTimerManager().ClearTimer(ExistingInfo->FadeOutHandle);
		}
	}
	
	FMaterialFadeInfo_Frankenstein FadeInfo;
	TArray<USkeletalMeshComponent*> MeshComponents;
	TargetActor->GetComponents<USkeletalMeshComponent>(MeshComponents);
	
	for (USkeletalMeshComponent* MeshComp : MeshComponents)
	{
		if (!MeshComp)
		{
			continue;
		}
		
		// 특정 메시만 타겟팅하는 경우
		if (!TargetMeshComponentName.IsNone() && MeshComp->GetFName() != TargetMeshComponentName)
		{
			continue;
		}
		
		int32 NumMaterials = MeshComp->GetNumMaterials();
		for (int32 i = 0; i < NumMaterials; ++i)
		{
			UMaterialInterface* OriginalMat = MeshComp->GetMaterial(i);
			FadeInfo.OriginalMaterials.Add(OriginalMat);
			
			// Dynamic Material Instance 생성
			UMaterialInstanceDynamic* DynMat = UMaterialInstanceDynamic::Create(SkinMaterial, MeshComp);
			if (DynMat)
			{
				FadeInfo.DynamicMaterials.Add(DynMat);
				MeshComp->SetMaterial(i, DynMat);
			}
		}
	}
	
	// 페이드 정보 저장
	ActorFadeInfoMap.Add(TargetActor, FadeInfo);
	
	// Fade In 타이머 시작 (각 액터별로 독립적인 타이머)
	FTimerHandle& FadeInHandle = ActorFadeInfoMap[TargetActor].FadeInHandle;
	float ElapsedTime = 0.0f;
	
	GetWorldTimerManager().SetTimer(
		FadeInHandle,
		[this, TargetActor, ElapsedTime]() mutable
		{
			if (!TargetActor || !ActorFadeInfoMap.Contains(TargetActor))
			{
				return;
			}
			
			ElapsedTime += 0.016f;
			float Alpha = FMath::Clamp(ElapsedTime / FadeDuration, 0.0f, 1.0f);
			
			FMaterialFadeInfo_Frankenstein& Info = ActorFadeInfoMap[TargetActor];
			for (UMaterialInstanceDynamic* DynMat : Info.DynamicMaterials)
			{
				if (DynMat)
				{
					DynMat->SetScalarParameterValue(FName("Opacity"), Alpha);
				}
			}
			
			if (Alpha >= 1.0f)
			{
				GetWorldTimerManager().ClearTimer(Info.FadeInHandle);
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
				UE_LOG(LogGCMaterialFrankenstein, Log, TEXT("Fade In 완료: %s"), *TargetActor->GetName());
#endif
			}
		},
		0.016f,
		true
	);
	
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	UE_LOG(LogGCMaterialFrankenstein, Log, TEXT("Fade In 시작: %s"), *TargetActor->GetName());
#endif
}

void AGC_Material_Frankenstein::ExecuteFadeOut(AActor* TargetActor)
{
	if (!TargetActor)
	{
		return;
	}
	
	FMaterialFadeInfo_Frankenstein* FadeInfoPtr = ActorFadeInfoMap.Find(TargetActor);
	if (!FadeInfoPtr)
	{
		UE_LOG(LogGCMaterialFrankenstein, Warning,
			TEXT("ExecuteFadeOut - %s에 대한 FadeInfo를 찾을 수 없습니다."), *TargetActor->GetName());
		return;
	}
	
	// 진행 중인 Fade In 타이머 정리
	if (FadeInfoPtr->FadeInHandle.IsValid())
	{
		GetWorldTimerManager().ClearTimer(FadeInfoPtr->FadeInHandle);
	}
	
	TArray<USkeletalMeshComponent*> MeshComponents;
	TargetActor->GetComponents<USkeletalMeshComponent>(MeshComponents);
	
	int32 MaterialIndex = 0;
	
	// 각 메시 컴포넌트의 머티리얼을 순회하며 원본으로 복원 준비
	for (USkeletalMeshComponent* MeshComp : MeshComponents)
	{
		if (!MeshComp)
		{
			continue;
		}
		
		if (!TargetMeshComponentName.IsNone() && MeshComp->GetFName() != TargetMeshComponentName)
		{
			continue;
		}
		
		int32 NumMaterials = MeshComp->GetNumMaterials();
		for (int32 i = 0; i < NumMaterials && MaterialIndex < FadeInfoPtr->OriginalMaterials.Num(); ++i, ++MaterialIndex)
		{
			// 원본 머티리얼로 즉시 복원 (Fade Out은 생략하고 즉시 복원)
			UMaterialInterface* OriginalMat = FadeInfoPtr->OriginalMaterials[MaterialIndex];
			if (OriginalMat)
			{
				MeshComp->SetMaterial(i, OriginalMat);
			}
		}
	}
	
	// 저장된 정보 제거
	ActorFadeInfoMap.Remove(TargetActor);
	
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	UE_LOG(LogGCMaterialFrankenstein, Log,
		TEXT("%s의 머티리얼이 복원되었습니다."), *TargetActor->GetName());
#endif

}