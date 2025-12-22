// GC_Poison_Material.cpp
#include "GAS/GC/Gimmick/GC_Poison_Material.h"

#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "GameplayTags/GameplayCueTags.h"

DEFINE_LOG_CATEGORY_STATIC(LogGCPoisonMaterial, Log, All);

#pragma region AGC_Poison_Material
AGC_Poison_Material::AGC_Poison_Material()
{
	/*
		GameplayCueTag 명시적 지정
		꼭 필요한 방어 코드: 생성자에서 명시적으로 태그를 지정 -> 안정성 확보
		C++ GameplayCue는 클래스명 + 태그 불일치 시 동작 안 하는 버그가 존재
	*/
	GameplayCueTag = CueTags::TAG_GameplayCue_Poison_Material;
	
	PrimaryActorTick.bCanEverTick = false;
	
	/*
		기본 머티리얼 설정 (Blueprint에서 설정 가능)
		PoisonOverlayMaterial을
		코드에서 ConstructorHelpers로 로드 가능하다는데 에디터에서 지정하겠음.
	*/
}
#pragma endregion

#pragma region OnActive
bool AGC_Poison_Material::OnActive_Implementation(
	AActor* MyTarget,
	const FGameplayCueParameters& Parameters)
{
	if (!MyTarget)
	{
		return false;
	}
	
// 	// 로컬 플레이어는 제외 (PostProcess만 보임)
// 	if (IsLocalPlayerTarget(MyTarget))
// 	{
// #if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
// 		UE_LOG(LogGCPoisonMaterial, Log,
// 			TEXT("[PoisonMaterial] 로컬 플레이어는 머티리얼 변경 제외: %s"),
// 			*MyTarget->GetName());
// #endif
// 		return true;
// 	}
	
	ApplyPoisonMaterial(MyTarget);
	StartFadeIn();
	
	return true;
}
#pragma endregion

#pragma region OnRemove
bool AGC_Poison_Material::OnRemove_Implementation(
	AActor* MyTarget,
	const FGameplayCueParameters& Parameters)
{
	if (!MyTarget)
	{
		return false;
	}
	
	// // 로컬 플레이어는 애초에 적용 안 됨
	// if (IsLocalPlayerTarget(MyTarget))
	// {
	// 	return true;
	// }
	
	StartFadeOut();
	
	return true;
}
#pragma endregion

#pragma region ApplyPoisonMaterial
void AGC_Poison_Material::ApplyPoisonMaterial(AActor* Target)
{
	if (!Target)
	{
		return;
	}
	
	// 모든 메시 컴포넌트 찾기
	TArray<UMeshComponent*> MeshComponents;
	GetAllMeshComponents(Target, MeshComponents);
	
	if (MeshComponents.Num() == 0)
	{
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
		UE_LOG(LogGCPoisonMaterial, Warning,
			TEXT("[PoisonMaterial] 메시 컴포넌트를 찾을 수 없음: %s"),
				*Target->GetName());
#endif
		return;
	}
	
	for (UMeshComponent* MeshComp : MeshComponents)
	{
		if (!MeshComp)
		{
			continue;
		}
		
		// 원본 머티리얼 저장
		TArray<UMaterialInterface*> OrigMats;
		int32 NumMaterials = MeshComp->GetNumMaterials();
		
		for (int32 i = 0; i < NumMaterials; i++)
		{
			UMaterialInterface* OrigMat = MeshComp->GetMaterial(i);
			OrigMats.Add(OrigMat);
			
			// Dynamic Material Instance 생성
			UMaterialInstanceDynamic* DynMat = nullptr;
			
			if (PoisonOverlayMaterial)
			{
				// 커스텀 Overlay Material 사용
				DynMat = UMaterialInstanceDynamic::Create(PoisonOverlayMaterial, this);
				
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
				UE_LOG(LogGCPoisonMaterial, Log,
					TEXT("[PoisonMaterial] PoisonOverlayMaterial 사용: %s"),
					*PoisonOverlayMaterial->GetName());
#endif
			}
			else
			{
				// 원본 머티리얼을 기반으로 MID 생성
				if (OrigMat)
				{
					DynMat = UMaterialInstanceDynamic::Create(OrigMat, this);
					
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
					UE_LOG(LogGCPoisonMaterial, Warning,
						TEXT("[PoisonMaterial] PoisonOverlayMaterial이 없어 원본 Material 사용: %s"),
						*OrigMat->GetName());
#endif
				}
			}
			
			if (DynMat)
			{
				// 초록색 파라미터 설정
				DynMat->SetVectorParameterValue(PoisonColorParamName, PoisonColor);
				DynMat->SetScalarParameterValue(PoisonIntensityParamName, 0.0f); // Fade In용
				
				// 적용
				MeshComp->SetMaterial(i, DynMat);
				DynamicMaterials.Add(DynMat);
				
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
				UE_LOG(LogGCPoisonMaterial, Log,
					TEXT("[PoisonMaterial] Material Slot %d 적용: PoisonColor=(%.2f,%.2f,%.2f), Intensity=0.0"),
					i, PoisonColor.R, PoisonColor.G, PoisonColor.B);
#endif
			}
		}
		
		OriginalMaterials.Add(MeshComp, OrigMats);
	}
	
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	UE_LOG(LogGCPoisonMaterial, Log,
		TEXT("[PoisonMaterial] 독 머티리얼 적용 완료: %s (메시: %d개, DynMat: %d개)"),
		*Target->GetName(), MeshComponents.Num(), DynamicMaterials.Num());
#endif
}
#pragma endregion

#pragma region RestoreOriginalMaterials
void AGC_Poison_Material::RestoreOriginalMaterials()
{
	for (auto& Pair : OriginalMaterials)
	{
		UMeshComponent* MeshComp = Pair.Key;
		TArray<UMaterialInterface*>& OrigMats = Pair.Value;
		
		if (!MeshComp || !IsValid(MeshComp))
		{
			continue;
		}
		
		for (int32 i = 0; i < OrigMats.Num(); i++)
		{
			if (OrigMats[i])
			{
				MeshComp->SetMaterial(i, OrigMats[i]);
			}
		}
	}
	
	OriginalMaterials.Empty();
	DynamicMaterials.Empty();
	
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	UE_LOG(LogGCPoisonMaterial, Log, TEXT("[PoisonMaterial] 원본 머티리얼 복구 완료"));
#endif
}
#pragma endregion

#pragma region StartFadeIn
void AGC_Poison_Material::StartFadeIn()
{
	bIsFadingIn = true;
	bIsFadingOut = false;
	FadeElapsedTime = 0.0f;
	const float TimerInterval = 1.0f / 60.0f; // 60 FPS
	
	GetWorldTimerManager().SetTimer(
		FadeTimerHandle,
		this,
		&AGC_Poison_Material::UpdateFade,
		TimerInterval,
		true
	);
}
#pragma endregion

#pragma region StartFadeOut
void AGC_Poison_Material::StartFadeOut()
{
	bIsFadingIn = false;
	bIsFadingOut = true;
	FadeElapsedTime = 0.0f;
	const float TimerInterval = 1.0f / 60.0f; // 60 FPS
	
	GetWorldTimerManager().SetTimer(
		FadeTimerHandle,
		this,
		&AGC_Poison_Material::UpdateFade,
		TimerInterval,
		true
	);
}
#pragma endregion

#pragma region UpdateFade
void AGC_Poison_Material::UpdateFade()
{
	//FadeElapsedTime += 1.0f / 60.0f;
	const float DeltaTime = GetWorld()->GetDeltaSeconds();
	FadeElapsedTime += DeltaTime;
	
	float Alpha = 0.0f;
	
	if (bIsFadingIn)
	{
		// FadeOutDuration: 에디터에서 0.0으로 설정되면 문제가 생김!
		//Alpha = FMath::Clamp(FadeElapsedTime / FadeInDuration, 0.0f, 1.0f);
		const float SafeFadeInDuration  = FMath::Max(FadeInDuration, KINDA_SMALL_NUMBER);
		Alpha = FMath::Clamp(FadeElapsedTime / SafeFadeInDuration ,0.0f,1.0f);
		
		Alpha = FMath::SmoothStep(0.0f, 1.0f, Alpha);
		
		// Intensity 증가
		float CurrentIntensity = Alpha * PoisonColorIntensity;
		
		for (UMaterialInstanceDynamic* DynMat : DynamicMaterials)
		{
			if (DynMat)
			{
				DynMat->SetScalarParameterValue(FName("PoisonIntensity"), CurrentIntensity);
			}
		}
		
		if (Alpha >= 1.0f)
		{
			bIsFadingIn = false;
			GetWorldTimerManager().ClearTimer(FadeTimerHandle);
		}
	}
	else if (bIsFadingOut)
	{
		//Alpha = FMath::Clamp(FadeElapsedTime / FadeOutDuration, 0.0f, 1.0f);
		const float SafeFadeOutDuration   = FMath::Max(FadeOutDuration, KINDA_SMALL_NUMBER);
		Alpha = FMath::Clamp(FadeElapsedTime / SafeFadeOutDuration ,0.0f,1.0f);
		
		Alpha = FMath::SmoothStep(0.0f, 1.0f, Alpha);
		
		// Intensity 감소
		float CurrentIntensity = (1.0f - Alpha) * PoisonColorIntensity;
		
		for (UMaterialInstanceDynamic* DynMat : DynamicMaterials)
		{
			if (DynMat)
			{
				DynMat->SetScalarParameterValue(FName("PoisonIntensity"), CurrentIntensity);
			}
		}
		
		if (Alpha >= 1.0f)
		{
			bIsFadingOut = false;
			GetWorldTimerManager().ClearTimer(FadeTimerHandle);
			RestoreOriginalMaterials();
		}
	}
}
#pragma endregion

#pragma region IsLocalPlayerTarget
bool AGC_Poison_Material::IsLocalPlayerTarget(AActor* Target) const
{
	if (!Target)
	{
		return false;
	}
	
	if (APawn* TargetPawn = Cast<APawn>(Target))
	{
		APlayerController* PC = Cast<APlayerController>(TargetPawn->GetController());
		if (PC && PC->IsLocalController())
		{
			return true;
		}
	}
	
	return false;
}
#pragma endregion

#pragma region GetAllMeshComponents
void AGC_Poison_Material::GetAllMeshComponents(
	AActor* Target,
	TArray<UMeshComponent*>& OutMeshes)
{
	if (!Target)
	{
		return;
	}
	
	// Character의 Mesh
	if (ACharacter* Character = Cast<ACharacter>(Target))
	{
		if (USkeletalMeshComponent* Mesh = Character->GetMesh())
		{
			OutMeshes.Add(Mesh);
		}
	}
	
	// 모든 SkeletalMesh와 StaticMesh 컴포넌트 찾기
	TArray<USkeletalMeshComponent*> SkeletalMeshes;
	Target->GetComponents<USkeletalMeshComponent>(SkeletalMeshes);
	
	TArray<UStaticMeshComponent*> StaticMeshes;
	Target->GetComponents<UStaticMeshComponent>(StaticMeshes);
	
	for (USkeletalMeshComponent* SkMesh : SkeletalMeshes)
	{
		if (SkMesh && !OutMeshes.Contains(SkMesh))
		{
			OutMeshes.Add(SkMesh);
		}
	}
	
	for (UStaticMeshComponent* StMesh : StaticMeshes)
	{
		if (StMesh)
		{
			OutMeshes.Add(StMesh);
		}
	}
}
#pragma endregion