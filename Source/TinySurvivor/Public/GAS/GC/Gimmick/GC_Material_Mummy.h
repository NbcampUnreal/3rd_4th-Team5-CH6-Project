// GC_Material_Mummy.h
#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Actor.h"
#include "GC_Material_Mummy.generated.h"

// 액터별 머티리얼 페이드 정보를 저장하는 구조체
USTRUCT()
struct FMaterialFadeInfo_Mummy
{
	GENERATED_BODY()
	
	UPROPERTY()
	TArray<UMaterialInterface*> OriginalMaterials;
	
	UPROPERTY()
	TArray<UMaterialInstanceDynamic*> DynamicMaterials;
	
	FTimerHandle FadeInHandle;
	FTimerHandle FadeOutHandle;
};

/*
	"Mummy" 형태로 머티리얼 변환 및 페이드 효과를 처리하는 GameplayCue
	
	- 특정 액터의 머티리얼을 "Mummy" 형태로 변경하고, 페이드 효과 적용
	- "OnActive"에서 스킨 머티리얼을 적용하고, Fade In으로 부드럽게 전환
	- "OnRemove"에서 원본 머티리얼로 복원하고, Fade Out으로 부드럽게 복귀
	- 타겟 메시 컴포넌트 또는 모든 SkeletalMesh에 머티리얼 변경 적용
	- 각 액터의 페이드 타이머를 독립적으로 관리하여 자연스러운 전환 구현
	- FadeDuration에 따라 불투명도(Opacity)를 서서히 조정
	- 액터별로 원본 및 다이나믹 머티리얼 정보를 저장하여 복원 가능
*/
UCLASS()
class TINYSURVIVOR_API AGC_Material_Mummy : public AGameplayCueNotify_Actor
{
	GENERATED_BODY()
	
public:
	AGC_Material_Mummy();
	
protected:
	// 원본 머티리얼 경로
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Material Settings")
	FSoftObjectPath OriginalMaterialPath;
	
	// 변경할 스킨 머티리얼 경로
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Material Settings")
	FSoftObjectPath SkinMaterialPath;
	
	// Fade 지속 시간
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Material Settings")
	float FadeDuration;
	
	// 머티리얼을 변경할 메시 컴포넌트 이름 (비어있으면 모든 SkeletalMesh)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Material Settings")
	FName TargetMeshComponentName;
	
	// GE 적용 시 호출 (FadeIn)
	virtual bool OnActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) override;
	
	// GE 제거 시 호출 (FadeOut)
	virtual bool OnRemove_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) override;
	
private:
	// 액터별 페이드 정보 저장
	UPROPERTY()
	TMap<TWeakObjectPtr<AActor>, FMaterialFadeInfo_Mummy> ActorFadeInfoMap;
	
	// Fade In 실행
	void ExecuteFadeIn(AActor* TargetActor, UMaterialInterface* SkinMaterial);
	
	// Fade Out 실행
	void ExecuteFadeOut(AActor* TargetActor);
};