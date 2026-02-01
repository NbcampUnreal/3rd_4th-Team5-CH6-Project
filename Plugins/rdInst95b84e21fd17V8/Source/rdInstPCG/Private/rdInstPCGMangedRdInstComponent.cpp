//
// rdInstPCGManagedrdInstComponent.cpp
//
// Copyright (c) 2024 Recourse Design ltd. All rights reserved.
//
// Creation Date: 24th August 2024 (moved from rdInstPCGSpawner.cpp)
// Last Modified: 6th June 2025
//
// More information on the rdInst SourceCode can be found at https://recourse.nz/index.php/rdinst-source-code/
//

#include "rdInstPCGManagedrdInstComponent.h"
#include "rdInst.h"
#include "rdInstSubsystem.h"
#include "PCGComponent.h"

#define LOCTEXT_NAMESPACE "rdInst"

//--------------------------------------------------------------------------------------------------
// PostLoad
//--------------------------------------------------------------------------------------------------
void UPCGManagedrdInstComponent::PostLoad() {

	Super::PostLoad();

	if(!bHasDescriptor) {
		if(UInstancedStaticMeshComponent* ISMC=GetComponent()) {
			FISMComponentDescriptor NewDescriptor;
			NewDescriptor.InitFrom(ISMC);
			SetDescriptor(NewDescriptor);
		}
	}

	// Cache raw ptr
	GetComponent();
}

//--------------------------------------------------------------------------------------------------
// ForgetComponent
//--------------------------------------------------------------------------------------------------
void UPCGManagedrdInstComponent::ForgetComponent() {
	//Super::ForgetComponent();
	ResetComponent();
}

//--------------------------------------------------------------------------------------------------
// SetDescriptor
//--------------------------------------------------------------------------------------------------
void UPCGManagedrdInstComponent::SetDescriptor(const FISMComponentDescriptor& InDescriptor) {
	bHasDescriptor=true;
	Descriptor=InDescriptor;
}

//--------------------------------------------------------------------------------------------------
// Release
//--------------------------------------------------------------------------------------------------
bool UPCGManagedrdInstComponent::Release(bool bHardRelease,TSet<TSoftObjectPtr<AActor>>& outActors) {

	ResetComponent();
	return false;
}

//--------------------------------------------------------------------------------------------------
// ReleaseIfUnused
//--------------------------------------------------------------------------------------------------
bool UPCGManagedrdInstComponent::ReleaseIfUnused(TSet<TSoftObjectPtr<AActor>>& OutActorsToDelete) {

	return instances.IsEmpty();
}

//--------------------------------------------------------------------------------------------------
// ResetComponent
//--------------------------------------------------------------------------------------------------
void UPCGManagedrdInstComponent::ResetComponent() {

	if(!instances.Num()) return;
	if(UInstancedStaticMeshComponent* ISMC=GetComponent()) {
		UrdInstSubsystem* rdInstSubsystem=GEngine?GEngine->GetEngineSubsystem<UrdInstSubsystem>():nullptr;
		if(rdInstSubsystem) {
			rdInstSubsystem->rdRemoveInstancesFast(GetComponent(),instances);
			instances.Empty();
		}
	}
}

//--------------------------------------------------------------------------------------------------
// MarkAsUsed
//--------------------------------------------------------------------------------------------------
void UPCGManagedrdInstComponent::MarkAsUsed() {

	const bool bWasMarkedUnused=bIsMarkedUnused;
	Super::MarkAsUsed();
	if(!bWasMarkedUnused) {
		return;
	}
}

//--------------------------------------------------------------------------------------------------
// MarkAsReused
//--------------------------------------------------------------------------------------------------
void UPCGManagedrdInstComponent::MarkAsReused() {

	Super::MarkAsReused();
}

//--------------------------------------------------------------------------------------------------
// GetComponent
//--------------------------------------------------------------------------------------------------
UInstancedStaticMeshComponent* UPCGManagedrdInstComponent::GetComponent() const {

	UrdInstSubsystem* rdInstSubsystem=GEngine?GEngine->GetEngineSubsystem<UrdInstSubsystem>():nullptr;
	if(rdInstSubsystem) {
		return rdInstSubsystem->rdGetHInstanceGen(mesh);
	}
	return nullptr;
}

//--------------------------------------------------------------------------------------------------
