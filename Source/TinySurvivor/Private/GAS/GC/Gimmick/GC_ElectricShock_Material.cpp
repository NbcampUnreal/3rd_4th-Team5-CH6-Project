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
	bIsSkullSkin = false;
	
	// 타이머가 남아있을 수 있으므로 정리
	if (BlinkTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(BlinkTimerHandle);
	}
	
	TargetActor = MyTarget;
	CacheOriginalMaterials(MyTarget); // 원본 머티리얼 저장 및 해골 스킨 감지
	LoadMaterials(); // 깜빡임용 해골 머티리얼 로드 (Dynamic Material 대신 skin4 로드)
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
	// WhileActive에서는 원본 머티리얼 유지
	if (MyTarget && OriginalMaterials.Num() > 0)
	{
		ApplyMaterialToActor(MyTarget, OriginalMaterials);
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
	bIsSkullSkin = false;
	
	return Super::OnRemove_Implementation(MyTarget, Parameters);
}
#pragma endregion

#pragma region LoadMaterials
void AGC_ElectricShock_Material::LoadMaterials()
{
	// 해골 머티리얼 로드 (검은 배경 + 흰 해골) - 일반 스킨용
	BlackSkullMaterial = Cast<UMaterialInterface>(
		StaticLoadObject(UMaterialInterface::StaticClass(),
		nullptr,
		*BlackSkullMaterialPath));
	
	if (!BlackSkullMaterial)
	{
		UE_LOG(LogGCElectricShockMaterial, Error,
			TEXT("BlackSkullMaterial 로드 실패: %s"), *BlackSkullMaterialPath);
	}
	
	// 반전 해골 머티리얼 로드 (화이트 배경 + 검은 해골) - 해골 스킨용
	WhiteSkullMaterial = Cast<UMaterialInterface>(
		StaticLoadObject(UMaterialInterface::StaticClass(),
		nullptr,
		*WhiteSkullMaterialPath));
	
	if (!WhiteSkullMaterial)
	{
		UE_LOG(LogGCElectricShockMaterial, Error,
			TEXT("WhiteSkullMaterial 로드 실패: %s"), *WhiteSkullMaterialPath);
	}
	
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	UE_LOG(LogGCElectricShockMaterial, Log,
		TEXT("[%s] 깜빡임 머티리얼 로드 완료 - 해골 스킨 여부: %s, BlackSkull: %s, WhiteSkull: %s"),
		*GetName(), 
		bIsSkullSkin ? TEXT("Yes") : TEXT("No"),
		BlackSkullMaterial ? TEXT("OK") : TEXT("Failed"),
		WhiteSkullMaterial ? TEXT("OK") : TEXT("Failed"));
#endif
}
#pragma endregion

#pragma region CacheOriginalMaterials
void AGC_ElectricShock_Material::CacheOriginalMaterials(AActor* Target)
{
	if (!Target)
	{
		return;
	}
	
	USkeletalMeshComponent* MeshComp = Target->FindComponentByClass<USkeletalMeshComponent>();
	if (!MeshComp)
	{
		return;
	}
	
	OriginalMaterials.Empty();
	bIsSkullSkin = false;
	
	for (int32 i = 0; i < MeshComp->GetNumMaterials(); ++i)
	{
		UMaterialInterface* CurrentMat = MeshComp->GetMaterial(i);
		OriginalMaterials.Add(CurrentMat);
		
		// 해골 스킨 감지 (skin1 키워드 포함 여부)
		if (CurrentMat && CurrentMat->GetPathName().Contains(SkullSkinKeyword))
		{
			bIsSkullSkin = true;
		}
	}
	
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	UE_LOG(LogGCElectricShockMaterial, Log,
		TEXT("[%s] 원본 머티리얼 캐시 완료 - Target: %s, Count: %d, 해골 스킨: %s"),
		*GetName(), *Target->GetName(), OriginalMaterials.Num(), 
		bIsSkullSkin ? TEXT("Yes") : TEXT("No"));
#endif
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
	USkeletalMeshComponent* MeshComp = Target->FindComponentByClass<USkeletalMeshComponent>();
	
	if (!MeshComp)
	{
		GetWorld()->GetTimerManager().ClearTimer(BlinkTimerHandle);
		return;
	}
	
	// 짝수: 깜빡임 머티리얼, 홀수: 원본 머티리얼
	if (CurrentBlinkCount % 2 == 0)
	{
		// 깜빡임 머티리얼 적용
		if (bIsSkullSkin)
		{
			// 해골 스킨 -> 화이트 해골 머티리얼(skin4) 적용
			if (WhiteSkullMaterial)
			{
				for (int32 i = 0; i < MeshComp->GetNumMaterials(); ++i)
				{
					MeshComp->SetMaterial(i, WhiteSkullMaterial);
				}
			}
		}
		else
		{
			// 일반 스킨: 검은 해골 머티리얼(skin1) 적용
			if (BlackSkullMaterial)
			{
				for (int32 i = 0; i < MeshComp->GetNumMaterials(); ++i)
				{
					MeshComp->SetMaterial(i, BlackSkullMaterial);
				}
			}
		}
	}
	else
	{
		// 원본 머티리얼 적용
		ApplyMaterialToActor(Target, OriginalMaterials);
	}
	
	CurrentBlinkCount++;
	
	// 지정된 깜빡임 횟수 완료
	if (CurrentBlinkCount >= BlinkCount * 2)
	{
		GetWorld()->GetTimerManager().ClearTimer(BlinkTimerHandle);
		RestoreOriginalMaterials(Target);
	}
}
#pragma endregion

#pragma region ApplyMaterialToActor
void AGC_ElectricShock_Material::ApplyMaterialToActor(
	AActor* Target,
	const TArray<UMaterialInterface*>& Materials)
{
	if (!Target || Materials.Num() == 0)
	{
		return;
	}
	
	USkeletalMeshComponent* MeshComp = Target->FindComponentByClass<USkeletalMeshComponent>();
	if (MeshComp)
	{
		for (int32 i = 0; i < Materials.Num() && i < MeshComp->GetNumMaterials(); ++i)
		{
			if (Materials[i])
			{
				MeshComp->SetMaterial(i, Materials[i]);
			}
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
	
	ApplyMaterialToActor(Target, OriginalMaterials);
	
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	UE_LOG(LogGCElectricShockMaterial, Log,
		TEXT("[%s] 원본 머티리얼 복원 완료 - Target: %s"),
		*GetName(), *Target->GetName());
#endif
}
#pragma endregion