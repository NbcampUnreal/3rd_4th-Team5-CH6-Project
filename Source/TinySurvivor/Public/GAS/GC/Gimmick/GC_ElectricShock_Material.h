// GC_ElectricShock_Material.h
#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Actor.h"
#include "Sound/SoundCue.h"
#include "GC_ElectricShock_Material.generated.h"

/*
	전기 감전 상태의 머티리얼 효과 GameplayCue
	- 해골 모양의 머티리얼로 깜빡임 효과와 감전 사운드 적용
	- Replicated 환경에서 모든 클라이언트에서 실행됨
*/
UCLASS()
class TINYSURVIVOR_API AGC_ElectricShock_Material : public AGameplayCueNotify_Actor
{
	GENERATED_BODY()
	
#pragma region AGC_Poison_Status
public:
	AGC_ElectricShock_Material();
#pragma endregion
	
#pragma region OnActive
public:
	/*
		GameplayCue가 활성화될 때 호출되는 함수
		
		동작 흐름:
			1. MyTarget 유효성 체크
			2. TargetActor 캐시
			3. LoadMaterials()로 필요한 머티리얼 로드
			4. CacheOriginalMaterials()로 원본 머티리얼 저장
			5. StartBlinkEffect()로 깜빡임 타이머 시작
			6. ElectricSoundCue가 있으면 사운드 재생
			
		특징:
			- 서버/클라이언트 모두에서 실행 가능
			- 깜빡임과 사운드 동시 처리
	*/
	virtual bool OnActive_Implementation(
		AActor* MyTarget,
		const FGameplayCueParameters& Parameters) override;
#pragma endregion
	
#pragma region WhileActive
public:
	/*
		GameplayCue가 활성화되어 있는 동안 호출되는 함수
		
		동작 흐름:
			1. MyTarget 유효성 체크
			2. SkullMaterial이 있으면 타겟에 적용
			
		특징:
			- 전기 감전 상태 동안 해골 머티리얼 유지
	*/
	virtual bool WhileActive_Implementation(
		AActor* MyTarget,
		const FGameplayCueParameters& Parameters) override;
#pragma endregion
	
#pragma region OnRemove
public:
	/*
		GameplayCue가 종료될 때 호출되는 함수
		
		동작 흐름:
			1. BlinkTimerHandle이 활성화되어 있으면 정리
			2. ElectricAudioComponent가 재생 중이면 정지
			3. RestoreOriginalMaterials()로 원본 머티리얼 복원
			4. OriginalMaterials, CurrentBlinkCount, TargetActor 캐시 초기화
			
		특징:
			- 안전하게 타겟 액터가 사라져도 동작
			- 사운드, 머티리얼 모두 복원
	*/
	virtual bool OnRemove_Implementation(
		AActor* MyTarget,
		const FGameplayCueParameters& Parameters) override;
#pragma endregion
	
#pragma region MaterialSettings
private:
	// 기본 머티리얼 경로
	UPROPERTY(EditDefaultsOnly, Category = "Material Settings",
		meta = (AllowPrivateAccess = "true"))
	FString DefaultMaterialPath =
		TEXT("/Game/ThirdParty/CardboardWarrior/Materials/Mat_Cardboard_warrior_Body");
	
	// 해골 머티리얼 경로
	UPROPERTY(EditDefaultsOnly, Category = "Material Settings",
		meta = (AllowPrivateAccess = "true"))
	FString SkullMaterialPath =
		TEXT("/Game/ThirdParty/Cardboard_Boy_SkinPack/Materials/Mat_Cardboard_warrior_Body_skin1");
	
	// 화이트 머티리얼 (런타임 생성용 베이스)
	UPROPERTY(EditDefaultsOnly, Category = "Material Settings",
		meta = (AllowPrivateAccess = "true"))
	//FString WhiteMaterialPath = TEXT("/Engine/EngineMaterials/DefaultMaterial");
	FString WhiteMaterialPath =
		TEXT("/Game/ThirdParty/CardboardWarrior/Materials/Mat_Cardboard_warrior_Body");
	
	// 깜빡임 횟수
	UPROPERTY(EditDefaultsOnly, Category = "Material Settings",
		meta = (AllowPrivateAccess = "true"))
	int32 BlinkCount = 2;
	
	// 각 깜빡임 간격 (초)
	UPROPERTY(EditDefaultsOnly, Category = "Material Settings",
		meta = (AllowPrivateAccess = "true"))
	float BlinkInterval = 0.3f;
	
	// 화이트 머티리얼 색상
	UPROPERTY(EditDefaultsOnly, Category = "Material Settings",
		meta = (AllowPrivateAccess = "true"))
	FLinearColor WhiteColor = FLinearColor::White;
	
	// 전기 마비 사운드 큐
	UPROPERTY(EditDefaultsOnly, Category = "Material Settings|Sound",
		meta = (AllowPrivateAccess = "true"))
	USoundCue* ElectricSoundCue;
#pragma endregion
	
#pragma region Variables
private:
	// 타이머 핸들
	FTimerHandle BlinkTimerHandle;
	
	// 현재 깜빡임 카운트
	int32 CurrentBlinkCount = 0;
	
	// 타겟 액터 레퍼런스
	TWeakObjectPtr<AActor> TargetActor;
	
	// 원본 머티리얼 캐시
	TArray<UMaterialInterface*> OriginalMaterials;
	
	// 로드된 머티리얼들
	UPROPERTY()
	UMaterialInterface* DefaultMaterial;
	
	UPROPERTY()
	UMaterialInterface* SkullMaterial;
	
	UPROPERTY()
	UMaterialInstanceDynamic* WhiteMaterialInstance;
	
	// 재생 중인 오디오 컴포넌트
	UPROPERTY()
	UAudioComponent* ElectricAudioComponent;
#pragma endregion
	
#pragma region LoadMaterials
private:
	/*
		필요한 머티리얼 로드 및 Dynamic Material 생성
		
		동작 흐름:
			1. DefaultMaterial, SkullMaterial StaticLoad
			2. WhiteMaterialPath로 Dynamic Material Instance 생성
			3. BaseColor와 Emissive 파라미터 설정
			
		특징:
			- 로드 실패 시 안전하게 fallback 사용
			- Dynamic Material 사용으로 런타임 색상/밝기 제어 가능
	*/
	void LoadMaterials();
#pragma endregion
	
#pragma region CacheOriginalMaterials
private:
	/*
		타겟 액터의 원본 머티리얼 캐시
		
		동작 흐름:
			1. 타겟 액터의 SkeletalMeshComponent 획득
			2. GetMaterial() 반복으로 OriginalMaterials 배열에 저장
			
		특징:
			- 깜빡임 종료 후 원본 머티리얼 복원용
			- 메시 머티리얼 개수에 맞춰 안전하게 저장
	*/
	void CacheOriginalMaterials(AActor* Target);
#pragma endregion
	
#pragma region StartBlinkEffect
private:
	/*
		깜빡임 효과 시작
		
		동작 흐름:
			1. CurrentBlinkCount 초기화
			2. BlinkInterval 간격으로 ToggleBlink() 반복 타이머 시작
			
		특징:
			- Timer 기반 반복 처리
			- TargetActor 유효성 체크 후 적용
	*/
	void StartBlinkEffect(AActor* Target);
#pragma endregion
	
#pragma region ToggleBlink
private:
	/*
		실제 깜빡임 토글 처리
	
		동작 흐름:
			1. TargetActor 유효성 체크
			2. CurrentBlinkCount 짝수 -> WhiteMaterialInstance 적용
			3. CurrentBlinkCount 홀수 -> SkullMaterial 적용
			4. CurrentBlinkCount 증가
			5. BlinkCount*2 도달 시 타이머 정리 및 원본 복원
	
		특징:
			- BlinkCount에 따라 깜빡임 횟수 자동 계산
			- Dynamic Material과 SkullMaterial 번갈아 적용
	*/
	void ToggleBlink();
#pragma endregion
	
#pragma region ApplyMaterialToActor
private:
	/*
		지정한 머티리얼을 액터의 모든 메시 슬롯에 적용
	
		동작 흐름:
			1. Target, Material 유효성 체크
			2. SkeletalMeshComponent의 모든 슬롯에 SetMaterial
	
		특징:
			- 깜빡임이나 상태 변경 시 반복 사용
	*/
	void ApplyMaterialToActor(AActor* Target, UMaterialInterface* Material);
#pragma endregion
	
#pragma region RestoreOriginalMaterials
private:
	/*
		원본 머티리얼 복원
	
		동작 흐름:
			1. Target 유효성 및 OriginalMaterials 확인
			2. SkeletalMeshComponent 각 슬롯에 OriginalMaterials 적용
	
		특징:
			- 깜빡임 종료 후 원래 머티리얼로 안전하게 복원
	*/
	void RestoreOriginalMaterials(AActor* Target);
#pragma endregion
};

