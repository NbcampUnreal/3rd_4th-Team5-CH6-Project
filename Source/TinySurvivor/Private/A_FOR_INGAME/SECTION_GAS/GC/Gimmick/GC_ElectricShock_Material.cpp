// All CopyRight From YulRyongGameStudio //


#include "A_FOR_INGAME/SECTION_GAS/GC/Gimmick/GC_ElectricShock_Material.h"
#include "A_FOR_COMMON/Tag/GameplayCueTags.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

//======================================================================================================================
#pragma region 라이프_사이클
	
	
	//━━━━━━━━━━━━━━━━━━━━
	// 라이프 사이클
	//━━━━━━━━━━━━━━━━━━━━

AGC_ElectricShock_Material::AGC_ElectricShock_Material()
{
	// 태그, 틱, 자동 제거
	GameplayCueTag = CueTags::TAG_GameplayCue_ElectricShock_Material;
	PrimaryActorTick.bCanEverTick = false;
	bAutoDestroyOnRemove = false; 
}

bool AGC_ElectricShock_Material::OnActive_Implementation(AActor* InMyTarget, const FGameplayCueParameters& InParameters)
{
	// 타겟 확인
	if (!IsValid(InMyTarget)) return false;
	
	// 타이머가 남아있을 수 있으므로 정리
	ClearBlinkTimer_internal();
	
	// 이전 상태 완전 초기화 (재사용 시 안전성 확보)
	ResetState_internal();
	
	// 타겟 캐싱
	TargetActorPtr = InMyTarget;

	// 원본 머티리얼 저장 및 해골 스킨 감지
	CacheOriginalMaterials_internal(InMyTarget);	

	// 깜빡임용 해골 머티리얼 로드 (Dynamic Material 대신 skin4 로드)
	LoadMaterials_internal();						

	// 깜빡임 시작
	StartBlinkEffect_internal(InMyTarget);
	
	// 사운드 재생
	StartAudio_internal(InMyTarget);
	
	return Super::OnActive_Implementation(InMyTarget, InParameters);
}

bool AGC_ElectricShock_Material::WhileActive_Implementation( AActor* InMyTarget, const FGameplayCueParameters& InParameters)
{
	return Super::WhileActive_Implementation(InMyTarget, InParameters);
}

bool AGC_ElectricShock_Material::OnRemove_Implementation(AActor* InMyTarget, const FGameplayCueParameters& InParameters)
{
	// 타겟 확인
	if (!IsValid(InMyTarget)) return false;
	
	// 타이머 정리
	ClearBlinkTimer_internal();
	
	// 사운드 정지
	StopAudio_internal();
	
	// 원본 머티리얼 복원
	RestoreOriginalMaterials_internal(InMyTarget);
	
	// 캐시 정리
	ResetState_internal();
	
	return Super::OnRemove_Implementation(InMyTarget, InParameters);
}

	
#pragma endregion
//======================================================================================================================
#pragma region 내부_동작_API
	
	
	//━━━━━━━━━━━━━━━━━━━━
	// 내부 동작 API
	//━━━━━━━━━━━━━━━━━━━━

void AGC_ElectricShock_Material::LoadMaterials_internal()
{
	// 해골 머티리얼 로드 (검은 배경 + 흰 해골) - 일반 스킨용
	BlackSkullMaterial = Cast<UMaterialInterface>(StaticLoadObject(UMaterialInterface::StaticClass() , nullptr, *BlackSkullMaterialPath));
	if (!IsValid(BlackSkullMaterial)) UE_LOG(LogTemp, Warning, TEXT("BlackSkullMaterial 로드 실패"));
	
	// 반전 해골 머티리얼 로드 (화이트 배경 + 검은 해골) - 해골 스킨용
	WhiteSkullMaterial = Cast<UMaterialInterface>(StaticLoadObject(UMaterialInterface::StaticClass(),nullptr, *WhiteSkullMaterialPath));
	if (!IsValid(WhiteSkullMaterial)) UE_LOG(LogTemp, Warning,TEXT("WhiteSkullMaterial 로드 실패"));
}

void AGC_ElectricShock_Material::CacheOriginalMaterials_internal(AActor* InTarget)
{
	// 타겟 확인
	if (!IsValid(InTarget)) return;
	
	// 스켈레탈 메시 가져오기
	USkeletalMeshComponent* MeshComp = InTarget->FindComponentByClass<USkeletalMeshComponent>();
	if (!IsValid(MeshComp)) return;
	
	// 데이터 초기화
	OriginalMaterialsArray.Empty();
	bIsSkullSkin = false;
	
	// 모든 머티리얼 배열에 넣기 
	for (int32 i = 0; i < MeshComp->GetNumMaterials(); ++i)
	{
		// 머티리얼 검증
		UMaterialInterface* CurrentMat = MeshComp->GetMaterial(i);
		if (!CurrentMat) continue;
		
		// 해골 스킨 감지 (skin1 키워드 포함 여부)
		if (CurrentMat->GetPathName().Contains(SkullSkinKeyword)) bIsSkullSkin = true;
		
		// 배열에 추가
		OriginalMaterialsArray.Add(CurrentMat);
	}
}

void AGC_ElectricShock_Material::StartBlinkEffect_internal(AActor* InTarget)
{
	// 타겟 확인
	if (!IsValid(InTarget) || !IsValid(GetWorld())) return;
	
	// 카운트 초기화
	CurrentBlinkCount = 0;
	
	// 깜빡임 타이머 시작
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	TimerManager.SetTimer(BlinkTimerHandle,this, &AGC_ElectricShock_Material::ToggleBlink_internal,BlinkInterval, true );
}

void AGC_ElectricShock_Material::StartAudio_internal(AActor* InTarget)
{
	// 검증 및 사운드 컴포넌트 생성
	if (!IsValid(ElectricSoundCue) && !IsValid(InTarget)) return;
	ElectricAudioComponent = UGameplayStatics::SpawnSoundAttached(ElectricSoundCue, InTarget->GetRootComponent());
}

void AGC_ElectricShock_Material::ToggleBlink_internal()
{
	// 검증
	if (!TargetActorPtr.IsValid() || !IsValid(TargetActorPtr.Get()))
	{
		ClearBlinkTimer_internal();
		return;
	}
	
	// 스켈레탈 메시 가져오기
	USkeletalMeshComponent* MeshComp = TargetActorPtr.Get()->FindComponentByClass<USkeletalMeshComponent>();
	if (!IsValid(MeshComp))
	{
		ClearBlinkTimer_internal();
		return;
	}

	// 홀수 
	if (CurrentBlinkCount % 2 != 0)
	{
		// 원본 머티리얼 적용
		ApplyMaterialToActor_internal( TargetActorPtr.Get(), OriginalMaterialsArray);
	}
	
	// 짝수
	else if (CurrentBlinkCount % 2 == 0)
	{
		// 해골 스킨 : 화이트 해골 머티리얼(skin4) 적용
		// 일반 스킨 : 검은 해골 머티리얼(skin1) 적용
		UMaterialInterface* TargetMat = bIsSkullSkin ? WhiteSkullMaterial : BlackSkullMaterial;
		if (!IsValid(TargetMat))
		{
			ClearBlinkTimer_internal();
			return;
		}
		
		for (int32 i = 0; i < MeshComp->GetNumMaterials(); ++i)
		{
			MeshComp->SetMaterial(i, TargetMat);
		}
	}	
	
	// 카운트 증가
	++CurrentBlinkCount;
	
	// 지정된 깜빡임 횟수 완료
	if (CurrentBlinkCount >= BlinkCount * 2)
	{
		ClearBlinkTimer_internal();
		RestoreOriginalMaterials_internal( TargetActorPtr.Get());
	}
}

	//--------------------
	// 머티리얼 적용 
	//--------------------

void AGC_ElectricShock_Material::ApplyMaterialToActor_internal(AActor* Target, const TArray<UMaterialInterface*>& Materials)
{
	// 타겟 확인
	if (!IsValid(Target) || Materials.Num() <= 0) return;
	
	// 스켈레탈 메쉬 가져오기
	USkeletalMeshComponent* MeshComp = Target->FindComponentByClass<USkeletalMeshComponent>();
	if (!IsValid(MeshComp)) return;
	
	// 머티리얼 적용
	for (int32 i = 0; i < Materials.Num() && i < MeshComp->GetNumMaterials(); ++i)
	{
		if (!IsValid(Materials[i])) continue;
		MeshComp->SetMaterial(i, Materials[i]);
	}
}

	//--------------------
	// 정리 
	//--------------------

void AGC_ElectricShock_Material::RestoreOriginalMaterials_internal(AActor* InTarget)
{
	// 타겟 검증
	if (!IsValid(InTarget) || OriginalMaterialsArray.Num() <= 0) return;
	
	// 머티리얼 적용
	ApplyMaterialToActor_internal(InTarget, OriginalMaterialsArray);
}

bool AGC_ElectricShock_Material::ClearBlinkTimer_internal()
{
	// 타임 매니저 캐싱
	if (!IsValid(GetWorld())) return false;
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	
	// 타이머 정리
	if (BlinkTimerHandle.IsValid()) TimerManager.ClearTimer(BlinkTimerHandle);
	return true;
}

void AGC_ElectricShock_Material::ResetState_internal()
{
	// 초기화 목록 : 캐싱한 머티리얼, 깜빡임 카운트, 타겟 포인터, 해골 스킨 여부 
	OriginalMaterialsArray.Empty();
	CurrentBlinkCount = 0;
	TargetActorPtr.Reset();
	bIsSkullSkin = false;
}

void AGC_ElectricShock_Material::StopAudio_internal()
{
	// 검증
	if (!IsValid(ElectricAudioComponent)) return;

	// 사운드 정지
	ElectricAudioComponent->Stop();
	ElectricAudioComponent = nullptr;
}


#pragma endregion
//======================================================================================================================