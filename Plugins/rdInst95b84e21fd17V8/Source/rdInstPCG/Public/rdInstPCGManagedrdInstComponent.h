// rdInstPCGComponent.h - Copyright (c) 2024 Recourse Design ltd.
#pragma once

#include "rdInstPCG.h"
#include "PCGManagedResource.h"
#include "rdInstPCGManagedrdInstComponent.generated.h"

#define RDINST_PLUGIN_API DLLEXPORT

UCLASS(BlueprintType)
class RDINST_PLUGIN_API UPCGManagedrdInstComponent : public UPCGManagedISMComponent {

	GENERATED_BODY()
public:
	virtual void PostLoad() override;
	virtual bool Release(bool bHardRelease,TSet<TSoftObjectPtr<AActor>>& OutActorsToDelete) override;
	virtual bool ReleaseIfUnused(TSet<TSoftObjectPtr<AActor>>& OutActorsToDelete) override;
	virtual void ResetComponent() override;
	virtual bool SupportsComponentReset() const override{ return true; }
	virtual void MarkAsUsed() override;
	virtual void MarkAsReused() override;
	virtual void ForgetComponent() override;

	UInstancedStaticMeshComponent* GetComponent() const;

	void SetDescriptor(const FISMComponentDescriptor& InDescriptor);
	const FISMComponentDescriptor& GetDescriptor() const { return Descriptor; }

//#if ENGINE_MINOR_VERSION<6
	//uint64 GetSettingsUID() const { return SettingsUID; }
	//void SetSettingsUID(uint64 InSettingsUID) { SettingsUID=InSettingsUID; }
//#endif

	TArray<int32> instances;
	UStaticMesh*  mesh=nullptr;
};
