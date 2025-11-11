// Fill out your copyright notice in the Description page of Project Settings.

#include "System/Erosion/ErosionStateInfo.h"
#include "System/Erosion/TSErosionSubSystem.h"
#include "Net/UnrealNetwork.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//-----------------------------
	// AErosionStateInfo 라이프사이클
	//-----------------------------

AErosionStateInfo::AErosionStateInfo()
{
	bReplicates = true;
	bAlwaysRelevant = true;           
	bNetLoadOnClient = true;          
	SetReplicatingMovement(false);    
	SetNetUpdateFrequency(1.f);
}

void AErosionStateInfo::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(AErosionStateInfo, CurrentErosion, COND_None, REPNOTIFY_OnChanged);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//-----------------------------
	// AErosionStateInfo 변수 업데이트
	//-----------------------------

void AErosionStateInfo::ServerSetErosion(float NewValue)
{
	if (HasAuthority())
	{
		CurrentErosion = NewValue;

		UTSErosionSubSystem* ErosionSubSystem = UTSErosionSubSystem::GetErosionSubSystem(this);
		if (IsValid(ErosionSubSystem) && ErosionSubSystem->GetShowDebug())
		{
			UE_LOG(ErosionManager, Error, TEXT("[Server] ServerSetErosion 호출됨 — CurrentErosion = %.2f"), CurrentErosion);
		}
		
		OnErosionChangedUIDelegate.Broadcast(CurrentErosion); // 호스트 (서버)
	}
	else
	{
		UE_LOG(ErosionManager, Error, TEXT("[Client] ServerSetErosion 호출 시도됨 (권한 없음) — 무시됨"));
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//-----------------------------
	// AErosionStateInfo UI 업데이트 (델리게이트, OnRep
	//-----------------------------
	
void AErosionStateInfo::OnRep_CurrentErosion()
{
	UTSErosionSubSystem* ErosionSubSystem = UTSErosionSubSystem::GetErosionSubSystem(this);
	if (IsValid(ErosionSubSystem))
	{
		UE_LOG(ErosionManager, Error, TEXT("[Client] OnRep_CurrentErosion 호출됨 — CurrentErosion = %.2f"), CurrentErosion);
	}
	
	OnErosionChangedUIDelegate.Broadcast(CurrentErosion); // 클라이언트 
}
