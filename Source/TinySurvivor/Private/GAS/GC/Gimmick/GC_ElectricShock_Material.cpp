// GC_ElectricShock_Material.cpp
#include "GAS/GC/Gimmick/GC_ElectricShock_Material.h"

#include "Components/SkeletalMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"
#include "GameplayTags/GameplayCueTags.h"

DEFINE_LOG_CATEGORY_STATIC(LogGCElectricShockMaterial, Log, All); // 로그 카테고리 정의

#pragma region AGC_ElectricShock_Material
AGC_ElectricShock_Material::AGC_ElectricShock_Material()
{
	/*
		GameplayCueTag 명시적 지정
		꼭 필요한 방어 코드: 생성자에서 명시적으로 태그를 지정 -> 안정성 확보
		C++ GameplayCue는 클래스명 + 태그 불일치 시 동작 안 하는 버그가 존재
	*/
	GameplayCueTag = CueTags::TAG_GameplayCue_ElectricShock_Material;
	
	PrimaryActorTick.bCanEverTick = false;
	
	bAutoDestroyOnRemove = false; // 수동으로 관리
}
#pragma endregion

#pragma region OnActive
bool AGC_ElectricShock_Material::OnActive_Implementation(
	AActor* MyTarget,
	const FGameplayCueParameters& Parameters)
{
	if (!MyTarget)
	{
		return false;
	}
	
	// 이전 상태 완전 초기화 (재사용 시 안전성 확보)
	OriginalMaterials.Empty();
	CurrentBlinkCount = 0;
	TargetActor.Reset();
	
	// 타이머가 남아있을 수 있으므로 정리
	if (BlinkTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(BlinkTimerHandle);
	}
	
	TargetActor = MyTarget;
	LoadMaterials();
	CacheOriginalMaterials(MyTarget);
	StartBlinkEffect(MyTarget);
	
	// 사운드 재생
	if (ElectricSoundCue)
	{
		ElectricAudioComponent = UGameplayStatics::SpawnSoundAttached(
			ElectricSoundCue,
			MyTarget->GetRootComponent()
		);
	}
	
	return Super::OnActive_Implementation(MyTarget, Parameters);
}
#pragma endregion

#pragma region WhileActive
bool AGC_ElectricShock_Material::WhileActive_Implementation(
	AActor* MyTarget,
	const FGameplayCueParameters& Parameters)
{
	// While Active에서는 해골 머티리얼 유지
	if (MyTarget && SkullMaterial)
	{
		ApplyMaterialToActor(MyTarget, SkullMaterial);
	}
	
	return Super::WhileActive_Implementation(MyTarget, Parameters);
}
#pragma endregion

#pragma region OnRemove
bool AGC_ElectricShock_Material::OnRemove_Implementation(
	AActor* MyTarget,
	const FGameplayCueParameters& Parameters)
{
	// 타이머 정리
	if (BlinkTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(BlinkTimerHandle);
	}
	
	// 사운드 정지
	if (ElectricAudioComponent)
	{
		ElectricAudioComponent->Stop();
		ElectricAudioComponent = nullptr;
	}
	
	// 원본 머티리얼 복원
	if (MyTarget)
	{
		RestoreOriginalMaterials(MyTarget);
	}
	
	// 캐시 정리
	OriginalMaterials.Empty();
	CurrentBlinkCount = 0;
	TargetActor.Reset();
	
	return Super::OnRemove_Implementation(MyTarget, Parameters);
}
#pragma endregion

#pragma region LoadMaterials
void AGC_ElectricShock_Material::LoadMaterials()
{
	// 기본 머티리얼 로드
	DefaultMaterial = Cast<UMaterialInterface>(
		StaticLoadObject(UMaterialInterface::StaticClass(),
		nullptr,
		*DefaultMaterialPath));
	if (!DefaultMaterial)
	{
		UE_LOG(LogGCElectricShockMaterial, Warning,
			TEXT("DefaultMaterial 로드 실패: %s, Fallback 사용"), *DefaultMaterialPath);
		DefaultMaterial = UMaterial::GetDefaultMaterial(MD_Surface); // 엔진 기본 머티리얼
	}
	
	// 해골 머티리얼 로드
	SkullMaterial = Cast<UMaterialInterface>(
		StaticLoadObject(UMaterialInterface::StaticClass(),
		nullptr,
		*SkullMaterialPath));
	if (!SkullMaterial)
	{
		UE_LOG(LogGCElectricShockMaterial, Warning,
			TEXT("SkullMaterial 로드 실패: %s, DefaultMaterial 사용"), *SkullMaterialPath);
		SkullMaterial = DefaultMaterial; // fallback
	}
	
	// 화이트 머티리얼 생성 (다이나믹 머티리얼 인스턴스)
	UMaterialInterface* BaseMaterial = Cast<UMaterialInterface>(
		StaticLoadObject(UMaterialInterface::StaticClass(),
		nullptr,
		*WhiteMaterialPath));
	if (!BaseMaterial)
	{
		UE_LOG(LogGCElectricShockMaterial, Warning,
			TEXT("WhiteMaterial 로드 실패: %s, DefaultMaterial 사용"), *WhiteMaterialPath);
		BaseMaterial = DefaultMaterial;
	}
	
	// Dynamic 생성
	if (BaseMaterial)
	{
		WhiteMaterialInstance = UMaterialInstanceDynamic::Create(BaseMaterial, this);
		if (WhiteMaterialInstance)
		{
			// 화이트 색상 설정 (Emissive를 사용하여 밝은 흰색 구현)
			WhiteMaterialInstance->SetVectorParameterValue(FName("BaseColor"), WhiteColor);
			WhiteMaterialInstance->SetScalarParameterValue(FName("Emissive"), 10.0f); // 밝게
		}
	}
}
#pragma endregion

#pragma region CacheOriginalMaterials
void AGC_ElectricShock_Material::CacheOriginalMaterials(AActor* Target)
{
	if (!Target)
	{
		return;
	}
	
	// 캐릭터의 메시 컴포넌트 찾기
	USkeletalMeshComponent* MeshComp = Target->FindComponentByClass<USkeletalMeshComponent>();
	if (MeshComp)
	{
		OriginalMaterials.Empty();
		for (int32 i = 0; i < MeshComp->GetNumMaterials(); ++i)
		{
			UMaterialInterface* CurrentMat = MeshComp->GetMaterial(i);
			
			// 이미 해골/화이트 머티리얼이 적용되어 있으면 DefaultMaterial 사용
			if (CurrentMat == SkullMaterial ||
				CurrentMat == WhiteMaterialInstance ||
				(SkullMaterialPath.Len() > 0 && CurrentMat &&
				 CurrentMat->GetPathName().Contains(TEXT("skin1"))))
			{
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
				UE_LOG(LogGCElectricShockMaterial, Warning,
					TEXT("[%s] 이미 효과 머티리얼이 적용됨. DefaultMaterial로 대체: Slot %d"),
					*GetName(), i);
#endif
				
				OriginalMaterials.Add(DefaultMaterial);
			}
			else
			{
				OriginalMaterials.Add(CurrentMat);
			}
		}
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
		UE_LOG(LogGCElectricShockMaterial, Log,
			TEXT("[%s] 원본 머티리얼 캐시 완료 - Target: %s, Count: %d"),
			*GetName(), *Target->GetName(), OriginalMaterials.Num());
#endif
	}
}
#pragma endregion

#pragma region StartBlinkEffect
void AGC_ElectricShock_Material::StartBlinkEffect(AActor* Target)
{
	if (!Target || !GetWorld())
	{
		return;
	}
	
	CurrentBlinkCount = 0;
	
	// 깜빡임 타이머 시작
	GetWorld()->GetTimerManager().SetTimer(
		BlinkTimerHandle,
		this,
		&AGC_ElectricShock_Material::ToggleBlink,
		BlinkInterval,
		true // 반복
	);
}
#pragma endregion

#pragma region ToggleBlink
void AGC_ElectricShock_Material::ToggleBlink()
{
	if (!TargetActor.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(BlinkTimerHandle);
		return;
	}
	
	AActor* Target = TargetActor.Get();
	
	// 짝수: 화이트, 홀수: 해골
	if (CurrentBlinkCount % 2 == 0)
	{
		// 화이트 머티리얼 적용
		if (WhiteMaterialInstance)
		{
			ApplyMaterialToActor(Target, WhiteMaterialInstance);
		}
	}
	else
	{
		// 해골 머티리얼 적용
		if (SkullMaterial)
		{
			ApplyMaterialToActor(Target, SkullMaterial);
		}
	}
	
	CurrentBlinkCount++;
	
	// 지정된 깜빡임 횟수 완료
	if (CurrentBlinkCount >= BlinkCount * 2)
	{
		GetWorld()->GetTimerManager().ClearTimer(BlinkTimerHandle);
		
		// 깜빡임 종료 후 원본 머티리얼 복원
		RestoreOriginalMaterials(Target);
	}
}
#pragma endregion

#pragma region ApplyMaterialToActor
void AGC_ElectricShock_Material::ApplyMaterialToActor(
	AActor* Target,
	UMaterialInterface* Material)
{
	if (!Target || !Material)
	{
		return;
	}
	
	USkeletalMeshComponent* MeshComp = Target->FindComponentByClass<USkeletalMeshComponent>();
	if (MeshComp)
	{
		for (int32 i = 0; i < MeshComp->GetNumMaterials(); ++i)
		{
			MeshComp->SetMaterial(i, Material);
		}
	}
}
#pragma endregion

#pragma region RestoreOriginalMaterials
void AGC_ElectricShock_Material::RestoreOriginalMaterials(AActor* Target)
{
	if (!Target || OriginalMaterials.Num() == 0)
	{
		return;
	}
	
	USkeletalMeshComponent* MeshComp = Target->FindComponentByClass<USkeletalMeshComponent>();
	if (MeshComp)
	{
		for (int32 i = 0; i < OriginalMaterials.Num() && i < MeshComp->GetNumMaterials(); ++i)
		{
			if (OriginalMaterials[i])
			{
				MeshComp->SetMaterial(i, OriginalMaterials[i]);
			}
		}
	}
}
#pragma endregion