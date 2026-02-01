// GC_Poison_Material.h

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Actor.h"
#include "GC_Poison_Material.generated.h"

/*
	독 상태 머티리얼 효과 GameplayCue
	- 독 상태인 플레이어를 초록빛 머티리얼로 적용
	- Replicated 환경에서 모든 클라이언트에서 실행됨
*/
UCLASS()
class TINYSURVIVOR_API AGC_Poison_Material : public AGameplayCueNotify_Actor
{
	GENERATED_BODY()
	
#pragma region AGC_Poison_Status
public:
	/*
		생성자
		- GameplayCueTag 명시적 지정으로 안정성 확보
		- Tick 비활성화 (Fade는 Timer로 처리)
	*/
	AGC_Poison_Material();
#pragma endregion
	
#pragma region OnActive
public:
	/*
		GameplayCue가 활성화될 때 호출되는 함수
		- Target 액터에 독 머티리얼 적용
		- FadeIn 시작
		- Target이 유효하지 않으면 false 반환
	*/
	virtual bool OnActive_Implementation(
		AActor* MyTarget,
		const FGameplayCueParameters& Parameters) override;
#pragma endregion
	
#pragma region OnRemove
public:
	/*
		GameplayCue가 종료될 때 호출되는 함수
		- FadeOut 시작
		- FadeOut 완료 시 원본 머티리얼 복구
		- Target이 유효하지 않으면 false 반환
	*/
	virtual bool OnRemove_Implementation(
		AActor* MyTarget,
		const FGameplayCueParameters& Parameters) override;
#pragma endregion
	
#pragma region MaterialSettings
private:
	// Poison 머티리얼 파라미터 이름
	UPROPERTY(EditDefaultsOnly, Category = "Material Settings|Parameters",
		meta = (AllowPrivateAccess = "true"))
	FName PoisonColorParamName = "PoisonColor";

	UPROPERTY(EditDefaultsOnly, Category = "Material Settings|Parameters",
		meta = (AllowPrivateAccess = "true"))
	FName PoisonIntensityParamName = "PoisonIntensity";
	
	// 독 상태 머티리얼 (MID를 생성할 베이스)
	UPROPERTY(EditDefaultsOnly, Category = "Material Settings",
		meta = (AllowPrivateAccess = "true"))
	UMaterialInterface* PoisonOverlayMaterial;
	
	// 독 색상 강도
	UPROPERTY(EditDefaultsOnly, Category = "Material Settings",
		meta = (AllowPrivateAccess = "true"))
	float PoisonColorIntensity = 0.6f;
	
	// 독 색상 (초록색 톤)
	UPROPERTY(EditDefaultsOnly, Category = "Material Settings",
		meta = (AllowPrivateAccess = "true"))
	FLinearColor PoisonColor = FLinearColor(0.0f, 1.0f, 0.0f, 1.0f);
	
	// Fade In/Out 시간 설정
	UPROPERTY(EditDefaultsOnly, Category = "Material Settings|Fade",
		meta = (AllowPrivateAccess = "true"))
	float FadeInDuration = 0.5f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Material Settings|Fade",
		meta = (AllowPrivateAccess = "true"))
	float FadeOutDuration = 0.8f;
#pragma endregion
	
#pragma region RuntimeData
private:
	// 원본 머티리얼 저장 (FadeOut 후 복구용)
	//UPROPERTY()
	TMap<UMeshComponent*, TArray<UMaterialInterface*>> OriginalMaterials;
	
	// 생성된 Dynamic Material Instance 목록 (FadeIn/Out 시 파라미터 변경)
	UPROPERTY()
	TArray<UMaterialInstanceDynamic*> DynamicMaterials;
	
	// 현재 효과가 적용된 Target 추적
	// 같은 Target에 재적용 시 원본 머티리얼 중복 저장 방지
	UPROPERTY()
	AActor* CurrentTarget = nullptr;
	
	// Fade 타이머 핸들 및 상태
	FTimerHandle FadeTimerHandle;
	float FadeElapsedTime = 0.0f;
	bool bIsFadingIn = false;
	bool bIsFadingOut = false;
#pragma endregion
	
#pragma region ApplyPoisonMaterial
private:
	/*
		Target 액터에 독 머티리얼 적용
		- 모든 메시 컴포넌트를 찾아 MID 적용
		- PoisonOverlayMaterial 없으면 원본 머티리얼 기반 MID 생성
		- 초기 Intensity 0으로 설정하여 FadeIn 준비
	*/
	void ApplyPoisonMaterial(AActor* Target);
#pragma endregion
	
#pragma region RestoreOriginalMaterials
private:
	/*
		원본 머티리얼 복구
		- DynamicMaterials 및 OriginalMaterials 초기화
		- FadeOut 완료 후 호출
	*/
	void RestoreOriginalMaterials();
#pragma endregion
	
#pragma region UpdateFade
private:
	/*
		Fade 업데이트 함수
		- FadeIn/FadeOut 상태에 따라 PoisonIntensity 조정
		- Alpha를 SmoothStep으로 계산하여 자연스러운 변화
		- Fade 완료 시 타이머 제거 및 원본 머티리얼 복구
	*/
	UFUNCTION()
	void UpdateFade();
#pragma endregion
	
#pragma region StartFadeIn
private:
	/*
		FadeIn 시작
		- FadeElapsedTime 초기화
		- Timer 등록 (60fps)
	*/
	void StartFadeIn();
#pragma endregion
	
#pragma region StartFadeOut
private:
	/*
		FadeOut 시작
		- FadeElapsedTime 초기화
		- Timer 등록 (60fps)
		- 완료 시 RestoreOriginalMaterials 호출
	*/
	void StartFadeOut();
#pragma endregion
	
#pragma region IsLocalPlayerTarget
private:
	/*
		Target이 로컬 플레이어인지 확인
		- 로컬 컨트롤러라면 true 반환
	*/
	bool IsLocalPlayerTarget(AActor* Target) const;
#pragma endregion
	
#pragma region GetAllMeshComponents
private:
	/*
		Target의 모든 MeshComponent 수집
		- ACharacter이면 GetMesh() 포함
		- SkeletalMeshComponent와 StaticMeshComponent 모두 수집
		- 중복 추가 방지
	*/
	void GetAllMeshComponents(AActor* Target, TArray<UMeshComponent*>& OutMeshes);
#pragma endregion
};

