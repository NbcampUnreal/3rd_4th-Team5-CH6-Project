// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "TSGA_BaseAbility.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "TSGA_BaseForOneMontage.generated.h"

/**
 * 
 */
UCLASS()
class TINYSURVIVOR_API UTSGA_BaseForOneMontage : public UTSGA_BaseAbility
{
	GENERATED_BODY()
	
//======================================================================================================================	
#pragma region 라이프_사이클
	
	//━━━━━━━━━━━━━━━━━━━━
	// 라이프 사이클
	//━━━━━━━━━━━━━━━━━━━━		
public:
	UTSGA_BaseForOneMontage();
	
#pragma endregion
//======================================================================================================================	
#pragma region 몽타주
	
	//━━━━━━━━━━━━━━━━━━━━
	// 몽타주
	//━━━━━━━━━━━━━━━━━━━━	
protected:	
	
	/** * 공통 몽타주 실행 함수 
	 * @param InMontage 실행할 몽타주
	 * @param InPlayRate 재생 속도
	 * @param StartSection 시작 섹션 이름
	 */
	UFUNCTION(BlueprintCallable, Category = "TS | GAS")
	virtual UAbilityTask_PlayMontageAndWait* PlayStandardMontageAndWait(
		UAnimMontage* InMontage, float InPlayRate = 1.0f, FName InStartSection = NAME_None);

	// 델리게이트를 바인딩할 가상 함수들 (자식 클래스에서 필요시 Override)
	UFUNCTION() virtual void OnMontageFinished();
	UFUNCTION() virtual void OnMontageInterrupted();
	UFUNCTION() virtual void OnMontageCancelled();

#pragma endregion
//======================================================================================================================		
};
