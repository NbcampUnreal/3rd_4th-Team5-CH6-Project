// rdInstPCGSpawner.h - Copyright (c) 2024 Recourse Design ltd.
#pragma once

#include "rdInstPCG.h"
#include "Elements/PCGStaticMeshSpawner.h"
#include "Elements/PCGStaticMeshSpawnerContext.h"
#include "rdSpawnActor.h"
#include "rdInstPCGSpawner.generated.h"

#define RDINST_PLUGIN_API DLLEXPORT

UCLASS(BlueprintType,ClassGroup=(Procedural))
class RDINST_PLUGIN_API UPCGrdInstStaticMeshSpawnerSettings : public UPCGStaticMeshSpawnerSettings {
	GENERATED_BODY()
public:
	UPCGrdInstStaticMeshSpawnerSettings(const FObjectInitializer &ObjectInitializer);
#if WITH_EDITOR
#if ENGINE_MINOR_VERSION>5
	virtual bool				DisplayExecuteOnGPUSetting() const override { return false; }
	TArray<FPCGPinProperties>	InputPinProperties() const;
	FPCGContext*				CreateContext();
#endif
	virtual FName				GetDefaultNodeName() const override { return FName(TEXT("rdInst StaticMeshSpawner")); }
	virtual FText				GetDefaultNodeTitle() const override;
	virtual void				ApplyDeprecation(UPCGNode* node) override;
#endif
protected:
	virtual FPCGElementPtr		CreateElement() const override;
public:
	virtual void				PostLoad() override;
#if WITH_EDITOR
	virtual void				PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#if ENGINE_MINOR_VERSION>3
	virtual bool				CanEditChange(const FProperty* InProperty) const override;
#endif
#endif
public:
	// Select a SpawnActor from the level to add the mesh instance to that
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category=Settings)
	TSoftObjectPtr<ArdSpawnActor> SpawnActor=nullptr;

protected:
	void RefreshMeshSelector();
	void RefreshInstancePacker();
};

class RDINST_PLUGIN_API FPCGrdInstSpawnerElement : public IPCGElement {
public:
#if ENGINE_MINOR_VERSION<4
	virtual FPCGContext* Initialize(const FPCGDataCollection& inData,TWeakObjectPtr<UPCGComponent> srcComp,const UPCGNode* node) override;
#endif
	virtual bool		 CanExecuteOnlyOnMainThread(FPCGContext* Context) const { return true; }
	virtual bool		 IsCacheable(const UPCGSettings* InSettings) const override { return false; }
protected:
	virtual bool		 PrepareDataInternal(FPCGContext* ctx) const override;
	virtual bool		 ExecuteInternal(FPCGContext* ctx) const override;	
#if ENGINE_MINOR_VERSION>3
	virtual void		 AbortInternal(FPCGContext* Context) const override;
#endif
	void				 spawnISMs(FPCGStaticMeshSpawnerContext* ctx,const FPCGMeshInstanceList& instList,AActor* owner,const FPCGPackedCustomData& packedCustomData) const;
#if ENGINE_MINOR_VERSION>3
	virtual FPCGContext* CreateContext() override;
#endif
};
