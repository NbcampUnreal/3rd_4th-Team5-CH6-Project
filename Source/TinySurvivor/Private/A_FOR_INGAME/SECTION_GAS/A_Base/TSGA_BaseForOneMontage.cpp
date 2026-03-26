// All CopyRight From YulRyongGameStudio //


#include "A_FOR_INGAME/SECTION_GAS/A_Base/TSGA_BaseForOneMontage.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"

//======================================================================================================================	
#pragma region 라이프_사이클
	
	//━━━━━━━━━━━━━━━━━━━━
	// 라이프 사이클
	//━━━━━━━━━━━━━━━━━━━━		

UTSGA_BaseForOneMontage::UTSGA_BaseForOneMontage()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

#pragma endregion
//======================================================================================================================	
#pragma region 몽타주
	
	//━━━━━━━━━━━━━━━━━━━━
	// 몽타주
	//━━━━━━━━━━━━━━━━━━━━	

UAbilityTask_PlayMontageAndWait* UTSGA_BaseForOneMontage::PlayStandardMontageAndWait(UAnimMontage* Montage, float PlayRate, FName StartSection)
{
	if (!IsValid(Montage)) return nullptr;;

	UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy
	(
	this,           // Owning Ability
	NAME_None,		// Task Name (디버깅용)
	Montage,        // Montage to play
	1.0f,           // Play Rate
	NAME_None       // Start Section Name
	);
	
	MontageTask->OnCompleted.AddDynamic(this, &UTSGA_BaseForOneMontage::OnMontageFinished);
	MontageTask->OnBlendOut.AddDynamic(this, &UTSGA_BaseForOneMontage::OnMontageFinished);
	MontageTask->OnInterrupted.AddDynamic(this, &UTSGA_BaseForOneMontage::OnMontageInterrupted);
	MontageTask->OnCancelled.AddDynamic(this, &UTSGA_BaseForOneMontage::OnMontageCancelled);
	MontageTask->ReadyForActivation();
	return MontageTask;
}

void UTSGA_BaseForOneMontage::OnMontageFinished()
{
	K2_EndAbility();
}

void UTSGA_BaseForOneMontage::OnMontageInterrupted()
{
	K2_EndAbility();

}

void UTSGA_BaseForOneMontage::OnMontageCancelled()
{
	K2_EndAbility();
}

#pragma endregion
//======================================================================================================================	