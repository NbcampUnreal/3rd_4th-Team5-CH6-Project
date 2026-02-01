//
// rdInstPCGSpawner.cpp
//
// Copyright (c) 2024 Recourse Design ltd. All rights reserved.
//
// Creation Date: 13th August 2024
// Last Modified: 30th June 2025
//
// More information on the rdInst SourceCode can be found at https://recourse.nz/index.php/rdinst-source-code/
//

#include "rdInstPCGSpawner.h"
#include "rdInst.h"
#include "rdInstSubsystem.h"
#include "PCGComponent.h"
#include "PCGCustomVersion.h"
#include "PCGManagedResource.h"
#include "Data/PCGPointData.h"
#include "Data/PCGSpatialData.h"
#include "MeshSelectors/PCGMeshSelectorByAttribute.h"
#include "Elements/PCGStaticMeshSpawnerContext.h"
#include "Helpers/PCGActorHelpers.h"
#include "Helpers/PCGHelpers.h"
#include "InstanceDataPackers/PCGInstanceDataPackerBase.h"
#include "MeshSelectors/PCGMeshSelectorWeighted.h"
#include "Components/InstancedStaticMeshComponent.h"

#define LOCTEXT_NAMESPACE "rdInstPCG"

//--------------------------------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------------------------------
UPCGrdInstStaticMeshSpawnerSettings::UPCGrdInstStaticMeshSpawnerSettings(const FObjectInitializer &ObjectInitializer) : UPCGStaticMeshSpawnerSettings(ObjectInitializer) {

	MeshSelectorType=UPCGMeshSelectorWeighted::StaticClass();
}

#if WITH_EDITOR
//--------------------------------------------------------------------------------------------------
// GetDefaultNodeTitle
//--------------------------------------------------------------------------------------------------
FText UPCGrdInstStaticMeshSpawnerSettings::GetDefaultNodeTitle() const {
	return LOCTEXT("NodeTitle", "rdInst Static Mesh Spawner");
}

//--------------------------------------------------------------------------------------------------
// ApplyDeprecation
//--------------------------------------------------------------------------------------------------
void UPCGrdInstStaticMeshSpawnerSettings::ApplyDeprecation(UPCGNode* InOutNode) {

	check(InOutNode);

	if(DataVersion<FPCGCustomVersion::StaticMeshSpawnerApplyMeshBoundsToPointsByDefault) {
		UE_LOG(LogPCG, Log, TEXT("Static Mesh Spawner node migrated from an older version. Disabling 'ApplyMeshBoundsToPoints' by default to match previous behavior."));
		bApplyMeshBoundsToPoints = false;
	}

	UPCGSettings::ApplyDeprecation(InOutNode);
}
#endif

//--------------------------------------------------------------------------------------------------
// CreateElement
//--------------------------------------------------------------------------------------------------
FPCGElementPtr UPCGrdInstStaticMeshSpawnerSettings::CreateElement() const {
	return MakeShared<FPCGrdInstSpawnerElement>();
}

//--------------------------------------------------------------------------------------------------
// PostLoad
//--------------------------------------------------------------------------------------------------
void UPCGrdInstStaticMeshSpawnerSettings::PostLoad() {

	UPCGSettings::PostLoad();

	const EObjectFlags Flags=GetMaskedFlags(RF_PropagateToSubObjects)|RF_Transactional;
	if(!MeshSelectorParameters) {
		RefreshMeshSelector();
	} else {
		MeshSelectorParameters->SetFlags(Flags);
	}

	if(!InstanceDataPackerParameters) {
		RefreshInstancePacker();
	} else {
		InstanceDataPackerParameters->SetFlags(Flags);
	}
}

#if WITH_EDITOR
//--------------------------------------------------------------------------------------------------
// PostEditChangeProperty
//--------------------------------------------------------------------------------------------------
void UPCGrdInstStaticMeshSpawnerSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) {

	if(PropertyChangedEvent.Property) {
		const FName& PropertyName = PropertyChangedEvent.Property->GetFName();
		if(PropertyName==GET_MEMBER_NAME_CHECKED(UPCGrdInstStaticMeshSpawnerSettings,MeshSelectorType)) {
			RefreshMeshSelector();
		} else if(PropertyName==GET_MEMBER_NAME_CHECKED(UPCGrdInstStaticMeshSpawnerSettings,InstanceDataPackerType)) {
			RefreshInstancePacker();
		}
	}
	UPCGSettings::PostEditChangeProperty(PropertyChangedEvent);
}

//--------------------------------------------------------------------------------------------------
// CanEditChange
//--------------------------------------------------------------------------------------------------
#if ENGINE_MINOR_VERSION>3
bool UPCGrdInstStaticMeshSpawnerSettings::CanEditChange(const FProperty* InProperty) const {
	if(InProperty->GetFName()==GET_MEMBER_NAME_CHECKED(UPCGrdInstStaticMeshSpawnerSettings,StaticMeshComponentPropertyOverrides)) {
		if(!MeshSelectorType->IsChildOf(UPCGMeshSelectorByAttribute::StaticClass())) {
			return false;
		}
	}
	return UPCGSettings::CanEditChange(InProperty);
}
#endif
#endif
//--------------------------------------------------------------------------------------------------
// RefreshMeshSelector
//--------------------------------------------------------------------------------------------------
void UPCGrdInstStaticMeshSpawnerSettings::RefreshMeshSelector() {
	
	if(MeshSelectorType) {
		ensure(IsInGameThread());
		if(MeshSelectorParameters) {
#if WITH_EDITOR
#if ENGINE_MINOR_VERSION>4
			MeshSelectorParameters->Rename(nullptr, GetTransientPackage(), REN_DontCreateRedirectors);
#else
			MeshSelectorParameters->Rename(nullptr,GetTransientPackage(),REN_DontCreateRedirectors|REN_ForceNoResetLoaders);
#endif
#endif
			MeshSelectorParameters->MarkAsGarbage();
			MeshSelectorParameters=nullptr;
		}

		const EObjectFlags Flags=GetMaskedFlags(RF_PropagateToSubObjects);
		MeshSelectorParameters=NewObject<UPCGMeshSelectorBase>(this,MeshSelectorType,NAME_None,Flags);
	} else {
		MeshSelectorParameters=nullptr;
	}
}

//--------------------------------------------------------------------------------------------------
// RefreshInstancePacker
//--------------------------------------------------------------------------------------------------
void UPCGrdInstStaticMeshSpawnerSettings::RefreshInstancePacker() {

	if(InstanceDataPackerType) {
		if(InstanceDataPackerParameters) {
#if WITH_EDITOR
#if ENGINE_MINOR_VERSION>4
			InstanceDataPackerParameters->Rename(nullptr, GetTransientPackage(), REN_DontCreateRedirectors);
#else
			InstanceDataPackerParameters->Rename(nullptr, GetTransientPackage(), REN_DontCreateRedirectors | REN_ForceNoResetLoaders);
#endif
#endif
			InstanceDataPackerParameters->MarkAsGarbage();
			InstanceDataPackerParameters = nullptr;
		}

		const EObjectFlags Flags=GetMaskedFlags(RF_PropagateToSubObjects);
		InstanceDataPackerParameters=NewObject<UPCGInstanceDataPackerBase>(this, InstanceDataPackerType, NAME_None, Flags);
	} else {
		InstanceDataPackerParameters = nullptr;
	}
}

#if WITH_EDITOR
#if ENGINE_MINOR_VERSION>5
TArray<FPCGPinProperties> UPCGrdInstStaticMeshSpawnerSettings::InputPinProperties() const {

	// Note: If executing on the GPU, we need to prevent multiple connections on inputs, since it is not supported at this time.
	// Also note: Since the ShouldExecuteOnGPU() is already tied to structural changes, we don't need to implement any logic for this in GetChangeTypeForProperty()
	const bool bAllowMultipleConnections = !ShouldExecuteOnGPU();

	TArray<FPCGPinProperties> Properties;
	FPCGPinProperties& InputPinProperty = Properties.Emplace_GetRef(PCGPinConstants::DefaultInputLabel, EPCGDataType::Point, bAllowMultipleConnections);
	InputPinProperty.SetRequiredPin();

	return Properties;
}

//FPCGElementPtr UPCGrdInstStaticMeshSpawnerWithIndicesSettings::CreateElement() const {
//	return MakeShared<FPCGrdInstSpawnerWithIndicesElement>();
//}

FPCGContext* UPCGrdInstStaticMeshSpawnerSettings::CreateContext() {
	return new FPCGStaticMeshSpawnerContext();
}
#endif
#endif

//--------------------------------------------------------------------------------------------------
// PrepareDataInternal
//--------------------------------------------------------------------------------------------------
bool FPCGrdInstSpawnerElement::PrepareDataInternal(FPCGContext* InContext) const {

	TRACE_CPUPROFILER_EVENT_SCOPE(FPCGStaticMeshSpawnerElement::PrepareDataInternal);
	
	FPCGStaticMeshSpawnerContext* Context=static_cast<FPCGStaticMeshSpawnerContext*>(InContext);
	UPCGrdInstStaticMeshSpawnerSettings* Settings=(UPCGrdInstStaticMeshSpawnerSettings*)Context->GetInputSettings<UPCGrdInstStaticMeshSpawnerSettings>();
	check(Settings);

	UPCGrdInstStaticMeshSpawnerSettings* set2=Cast<UPCGrdInstStaticMeshSpawnerSettings>(Settings);
	if(!set2) {
		return true;
	}

	if(!Settings->MeshSelectorParameters) {
		PCGE_LOG(Error, GraphAndLog, LOCTEXT("InvalidMeshSelectorInstance", "Invalid MeshSelector instance, try reselecting the MeshSelector type"));
		return true;
	}
#if ENGINE_MINOR_VERSION>5
	if(!Context->ExecutionSource.Get())	{
#else
	if(!Context->SourceComponent.Get())	{
#endif
		return true;
	}

#if WITH_EDITOR
	// In editor, we always want to generate this data for inspection & to prevent caching issues
	const bool bGenerateOutput=true;
#else
	const bool bGenerateOutput=true;//Context->Node && Context->Node->IsOutputPinConnected(PCGPinConstants::DefaultOutputLabel);
#endif

	bool& bSkippedDueToReuse = Context->bSkippedDueToReuse;

	if(!bGenerateOutput && bSkippedDueToReuse) {
		return true;
	}

#if ENGINE_MINOR_VERSION>5
	TArray<FPCGTaggedData> Inputs=Context->InputData.GetAllInputs();
#else
	TArray<FPCGTaggedData> Inputs=Context->InputData.GetInputs();
#endif
	TArray<FPCGTaggedData>& Outputs=Context->OutputData.TaggedData;

	while(Context->CurrentInputIndex<Inputs.Num()) {

		if(!Context->bCurrentInputSetup) {
			const FPCGTaggedData& Input=Inputs[Context->CurrentInputIndex];
			const UPCGSpatialData* SpatialData=Cast<UPCGSpatialData>(Input.Data);
			if(!SpatialData) {
				PCGE_LOG(Error,GraphAndLog,LOCTEXT("InvalidInputData","Invalid input data"));
				++Context->CurrentInputIndex;
				continue;
			}

			const UPCGPointData* PointData=SpatialData->ToPointData(Context);
			if(!PointData) {
				PCGE_LOG(Error,GraphAndLog,LOCTEXT("NoPointDataInInput","Unable to get point data from input"));
				++Context->CurrentInputIndex;
				continue;
			}

			AActor* TargetActor=Settings->TargetActor.Get()?Settings->TargetActor.Get():Context->GetTargetActor(nullptr);
			if(!TargetActor) {
				PCGE_LOG(Error,GraphAndLog,LOCTEXT("InvalidTargetActor","Invalid target actor. Ensure TargetActor member is initialized when creating SpatialData."));
				++Context->CurrentInputIndex;
				continue;
			}

			if(bGenerateOutput) {

				FPCGTaggedData& Output=Outputs.Add_GetRef(Input);
#if ENGINE_MINOR_VERSION>5
				UPCGBasePointData* OutputPointData=FPCGContext::NewPointData_AnyThread(Context);

				// @todo_pcg: this could probably be inherited, since not all selectors output all points this works for now.
				FPCGInitializeFromDataParams InitializeFromDataParams(PointData);
				InitializeFromDataParams.bInheritSpatialData = false;
				OutputPointData->InitializeFromDataWithParams(InitializeFromDataParams);

				OutputPointData->SetNumPoints(PointData->GetNumPoints());
				OutputPointData->AllocateProperties(PointData->GetAllocatedProperties());
#else
				UPCGPointData* OutputPointData=NewObject<UPCGPointData>();
				OutputPointData->InitializeFromData(PointData);
#endif

				if(OutputPointData->Metadata->HasAttribute(Settings->OutAttributeName)) {
					OutputPointData->Metadata->DeleteAttribute(Settings->OutAttributeName);
					PCGE_LOG(Verbose,LogOnly,FText::Format(LOCTEXT("AttributeOverwritten", "Metadata attribute '{0}' is being overwritten in the output data"), FText::FromName(Settings->OutAttributeName)));
				}
				OutputPointData->Metadata->CreateStringAttribute(Settings->OutAttributeName,FName(NAME_None).ToString(),false);

				Output.Data = OutputPointData;
				//check(!Context->CurrentOutputPointData);
				Context->CurrentOutputPointData = OutputPointData;
			}
#if ENGINE_MINOR_VERSION>5
			if(bGenerateOutput || !Context->bCurrentDataSkippedDueToReuse) {
				FPCGStaticMeshSpawnerContext::FPackedInstanceListData& InstanceListData = Context->MeshInstancesData.Emplace_GetRef();
				InstanceListData.TargetActor = TargetActor;
				InstanceListData.SpatialData = PointData;
				//InstanceListData.DataCrc = DataCrc;
				InstanceListData.bSkippedDueToReuse = Context->bSkippedDueToReuse || Context->bCurrentDataSkippedDueToReuse;

				Context->CurrentPointData = PointData;
				Context->bCurrentInputSetup = true;
			} else {
				// skip selection
				Context->bSelectionDone = true;
			}
#else
			FPCGStaticMeshSpawnerContext::FPackedInstanceListData& InstanceListData=Context->MeshInstancesData.Emplace_GetRef();
			InstanceListData.TargetActor=TargetActor;
			InstanceListData.SpatialData=PointData;

			Context->CurrentPointData=PointData;
			Context->bCurrentInputSetup=true;
#endif
		}

		if(!Context->bSelectionDone) {
			check(Context->CurrentPointData);
			const UPCGPointData* inPointData=Cast<UPCGPointData>(Context->CurrentPointData);
			UPCGPointData* outPointData=Cast<UPCGPointData>(Context->CurrentOutputPointData);

#if ENGINE_MINOR_VERSION>5
			Context->bSelectionDone=Settings->MeshSelectorParameters->SelectMeshInstances(*Context,(UPCGStaticMeshSpawnerSettings*)Settings,inPointData,Context->MeshInstancesData.Last().MeshInstances,outPointData);
#else
			Context->bSelectionDone=Settings->MeshSelectorParameters->SelectInstances(*Context,(UPCGStaticMeshSpawnerSettings*)Settings,inPointData,Context->MeshInstancesData.Last().MeshInstances,outPointData);
#endif

		//  Context->bSelectionDone=Settings->MeshSelectorParameters->SelectInstances(*Context,Settings,Context->CurrentPointData,Context->MeshInstancesData.Last().MeshInstances, Context->CurrentOutputPointData);
		//  FPCGSkinnedMeshSpawnerContext* Context = static_cast<FPCGSkinnedMeshSpawnerContext*>(InContext);

		}

#if ENGINE_MINOR_VERSION>5
		// TODO: If we know we re-use the ISMCs, we should not run the Selection, as it can be pretty costly.
		// At the moment, the selection is filling the output point data, so it is necessary to run it. But we should just hit the cache in that case.
		if(!Context->bSelectionDone) {
			check(Context->CurrentPointData);
			Context->bSelectionDone=Settings->MeshSelectorParameters->SelectMeshInstances(*Context, Settings, Context->CurrentPointData, Context->MeshInstancesData.Last().MeshInstances, Context->CurrentOutputPointData);
		}
#endif

		if(!Context->bSelectionDone) {
			return false;
		}

#if ENGINE_MINOR_VERSION>5
		// If we need the output but would otherwise skip the resource creation, we don't need to run the instance packing part of the processing
//		if(!bSkippedDueToReuse && !Context->bCurrentDataSkippedDueToReuse) {
		if(!Context->bCurrentDataSkippedDueToReuse) {
			TArray<FPCGPackedCustomData>& PackedCustomData = Context->MeshInstancesData.Last().PackedCustomData;
			const TArray<FPCGMeshInstanceList>& MeshInstances = Context->MeshInstancesData.Last().MeshInstances;

			if (PackedCustomData.Num() != MeshInstances.Num()) {
				PackedCustomData.SetNum(MeshInstances.Num());
			}

			if (Settings->InstanceDataPackerParameters) {
				for (int32 InstanceListIndex = 0; InstanceListIndex < MeshInstances.Num(); ++InstanceListIndex) {
					Settings->InstanceDataPackerParameters->PackInstances(*Context, Context->CurrentPointData, MeshInstances[InstanceListIndex], PackedCustomData[InstanceListIndex]);
				}
			}
		}
#else
		TArray<FPCGPackedCustomData>& PackedCustomData=Context->MeshInstancesData.Last().PackedCustomData;
		const TArray<FPCGMeshInstanceList>& MeshInstances=Context->MeshInstancesData.Last().MeshInstances;
		if(PackedCustomData.Num()!=MeshInstances.Num()) {
			PackedCustomData.SetNum(MeshInstances.Num());
		}

		if(Settings->InstanceDataPackerParameters) {
			for(int32 InstanceListIndex=0;InstanceListIndex<MeshInstances.Num();++InstanceListIndex) {
				Settings->InstanceDataPackerParameters->PackInstances(*Context,Context->CurrentPointData,MeshInstances[InstanceListIndex],PackedCustomData[InstanceListIndex]);
			}
		}
#endif
		// We're done - cleanup for next iteration if we still have time
		++Context->CurrentInputIndex;
		Context->ResetInputIterationData();

		// Continue on to next iteration if there is time left, otherwise, exit here
		if(Context->AsyncState.ShouldStop() && Context->CurrentInputIndex<Inputs.Num()) {
			return false;
		}
	}

#if ENGINE_MINOR_VERSION>3
	IPCGAsyncLoadingContext* AsyncLoadingContext = static_cast<IPCGAsyncLoadingContext*>(Context);
	if(Context->CurrentInputIndex == Inputs.Num() && !AsyncLoadingContext->WasLoadRequested() && !Context->MeshInstancesData.IsEmpty()) { // && !Settings->bSynchronousLoad)
#else
	if(Context->CurrentInputIndex == Inputs.Num() && !Context->MeshInstancesData.IsEmpty()) {
#endif
		TArray<FSoftObjectPath> ObjectsToLoad;
		for(const FPCGStaticMeshSpawnerContext::FPackedInstanceListData& InstanceData : Context->MeshInstancesData) {
			for(const FPCGMeshInstanceList& MeshInstanceList : InstanceData.MeshInstances) {
				if(!MeshInstanceList.Descriptor.StaticMesh.IsNull()) {
					ObjectsToLoad.AddUnique(MeshInstanceList.Descriptor.StaticMesh.ToSoftObjectPath());
				}

				for(const TSoftObjectPtr<UMaterialInterface>& OverrideMaterial : MeshInstanceList.Descriptor.OverrideMaterials) {
					if(!OverrideMaterial.IsNull()) {
						ObjectsToLoad.AddUnique(OverrideMaterial.ToSoftObjectPath());
					}
				}
			}
		}

#if ENGINE_MINOR_VERSION<4
		//Context->bDataPrepared=true;
		return true;
#else
		return AsyncLoadingContext->RequestResourceLoad(Context,std::move(ObjectsToLoad),true);
#endif
	}

	return true;
}

//--------------------------------------------------------------------------------------------------
// Initialize
//--------------------------------------------------------------------------------------------------
#if ENGINE_MINOR_VERSION<4
FPCGContext* FPCGrdInstSpawnerElement::Initialize(const FPCGDataCollection& inData,TWeakObjectPtr<UPCGComponent> srcComp,const UPCGNode* node) {
	FPCGStaticMeshSpawnerContext* Context=new FPCGStaticMeshSpawnerContext();
	Context->InputData=inData;
	Context->SourceComponent=srcComp;
	Context->Node=node;
	return Context;
}
#endif

//--------------------------------------------------------------------------------------------------
// CreateContext
//--------------------------------------------------------------------------------------------------
#if ENGINE_MINOR_VERSION>3
FPCGContext* FPCGrdInstSpawnerElement::CreateContext() {
	return new FPCGStaticMeshSpawnerContext();
}
#endif
//--------------------------------------------------------------------------------------------------
// CanExecuteOnlyOnMainThread
//--------------------------------------------------------------------------------------------------
/*
bool FPCGrdInstSpawnerElement::CanExecuteOnlyOnMainThread(FPCGContext* Context) const {

	// PrepareData can call UPCGManagedComponent::MarkAsReused which registers the ISMC, which can go into Chaos code that asserts if not on main thread.
	// TODO: We can likely re-enable multi-threading for PrepareData if we move the call to MarkAsReused to Execute. There should hopefully not be
	// wider contention on resources resources are not shared across nodes and are also per-component.
	return true;//Context->CurrentPhase == EPCGExecutionPhase::Execute || Context->CurrentPhase == EPCGExecutionPhase::PrepareData;
}
*/
//--------------------------------------------------------------------------------------------------
// ExecuteInternal
//--------------------------------------------------------------------------------------------------
bool FPCGrdInstSpawnerElement::ExecuteInternal(FPCGContext* InContext) const {

	TRACE_CPUPROFILER_EVENT_SCOPE(FPCGrdInstStaticMeshSpawnerElement::Execute);
	FPCGStaticMeshSpawnerContext* Context=static_cast<FPCGStaticMeshSpawnerContext*>(InContext);
	const UPCGrdInstStaticMeshSpawnerSettings* Settings=Context->GetInputSettings<UPCGrdInstStaticMeshSpawnerSettings>();
	check(Settings);

	while(!Context->MeshInstancesData.IsEmpty()) {

		const FPCGStaticMeshSpawnerContext::FPackedInstanceListData& InstanceList=Context->MeshInstancesData.Last();
		check(InstanceList.MeshInstances.Num()==InstanceList.PackedCustomData.Num());

		const bool bTargetActorValid = (InstanceList.TargetActor && IsValid(InstanceList.TargetActor));

		if(bTargetActorValid) {

			while(Context->CurrentDataIndex < InstanceList.MeshInstances.Num()) {
				const FPCGMeshInstanceList& MeshInstance = InstanceList.MeshInstances[Context->CurrentDataIndex];
				// We always have mesh instances, but if we are in re-use, we don't compute the packed custom data.
				static const FPCGPackedCustomData EmptyCustomData{};
				const FPCGPackedCustomData& PackedCustomData = InstanceList.PackedCustomData.IsValidIndex(Context->CurrentDataIndex) ? InstanceList.PackedCustomData[Context->CurrentDataIndex] : EmptyCustomData;
				spawnISMs(Context,MeshInstance,InstanceList.TargetActor,PackedCustomData);

				// Now that the mesh is loaded/spawned, set the bounds to out points if requested.
				if(MeshInstance.Descriptor.StaticMesh && Settings->bApplyMeshBoundsToPoints) {
#if ENGINE_MINOR_VERSION>3
#if ENGINE_MINOR_VERSION>5
					if(TMap<UPCGBasePointData*,TArray<int32>>* OutPointDataToPointIndex=Context->MeshToOutPoints.Find(MeshInstance.Descriptor.StaticMesh)) {
						const FBox Bounds=MeshInstance.Descriptor.StaticMesh->GetBoundingBox();
						for(TPair<UPCGBasePointData*,TArray<int32>>& It:*OutPointDataToPointIndex) {
							check(It.Key);
							TPCGValueRange<FVector> BoundsMinRange=It.Key->GetBoundsMinValueRange();
							TPCGValueRange<FVector> BoundsMaxRange=It.Key->GetBoundsMaxValueRange();
							int32 MaxIndex = FMath::Min(BoundsMinRange.Num(), BoundsMaxRange.Num());
							for(int32 Index:It.Value) {
								if(Index<MaxIndex) { // this is fixing a crash of index out of bounds (always on a 1024 boundary) in UE5.6
									BoundsMinRange[Index]=Bounds.Min;
									BoundsMaxRange[Index]=Bounds.Max;
								}
							}
						}
					}
#else
					if(TMap<UPCGPointData*,TArray<int32>>* OutPointDataToPointIndex=Context->MeshToOutPoints.Find(MeshInstance.Descriptor.StaticMesh)) {
						const FBox Bounds = MeshInstance.Descriptor.StaticMesh->GetBoundingBox();
						for(TPair<UPCGPointData*, TArray<int32>>& It : *OutPointDataToPointIndex) {
							check(It.Key);
							TArray<FPCGPoint>& outPoints=It.Key->GetMutablePoints();
							for(int32 Index:It.Value) {
								FPCGPoint& Point=outPoints[Index];
								Point.BoundsMin=Bounds.Min;
								Point.BoundsMax=Bounds.Max;
							}
						}
					}
#endif
#endif
				}
				++Context->CurrentDataIndex;
				if(Context->AsyncState.ShouldStop()) {
					break;
				}
			}
		}

		if(!bTargetActorValid || Context->CurrentDataIndex == InstanceList.MeshInstances.Num()) {

			Context->MeshInstancesData.RemoveAtSwap(Context->MeshInstancesData.Num() - 1);
			Context->CurrentDataIndex = 0;
		}

		if(Context->AsyncState.ShouldStop()) {
			break;
		}
	}

	const bool bFinishedExecution=Context->MeshInstancesData.IsEmpty();
#if ENGINE_MINOR_VERSION>3
	if(bFinishedExecution) {
		if(AActor* TargetActor=Settings->TargetActor.Get()?Settings->TargetActor.Get():Context->GetTargetActor(nullptr)) {
			for(UFunction* Function:PCGHelpers::FindUserFunctions(TargetActor->GetClass(),Settings->PostProcessFunctionNames,{ UPCGFunctionPrototypes::GetPrototypeWithNoParams() },Context)) {
				TargetActor->ProcessEvent(Function,nullptr);
			}
		}
	}
#endif
	return bFinishedExecution;
}

//--------------------------------------------------------------------------------------------------
// spawnISMs
//--------------------------------------------------------------------------------------------------
void FPCGrdInstSpawnerElement::spawnISMs(FPCGStaticMeshSpawnerContext* Context,const FPCGMeshInstanceList& InstanceList,AActor* TargetActor,const FPCGPackedCustomData& PackedCustomData) const {

	// Populate the (H)ISM from the previously prepared entries
	TRACE_CPUPROFILER_EVENT_SCOPE(FPCGStaticMeshSpawnerElement::Execute::PopulateISMs);

	if(InstanceList.Instances.Num()==0) {
		return;
	}

	// Will be synchronously loaded if not loaded. But by default it should already have been loaded asynchronously in PrepareData, so this is free.
	UStaticMesh* LoadedMesh=InstanceList.Descriptor.StaticMesh.LoadSynchronous();
	if(!LoadedMesh) {
		// Either we have no mesh (so nothing to do) or the mesh couldn't be loaded
		if(InstanceList.Descriptor.StaticMesh.IsValid()) {
			PCGE_LOG(Error, GraphAndLog, FText::Format(LOCTEXT("MeshLoadFailed", "Unable to load mesh '{0}'"), FText::FromString(InstanceList.Descriptor.StaticMesh.ToString())));
		}
		return;
	}

	for(TSoftObjectPtr<UMaterialInterface> OverrideMaterial : InstanceList.Descriptor.OverrideMaterials) {
		// Will be synchronously loaded if not loaded. But by default it should already have been loaded asynchronously in PrepareData, so this is free.
		if(OverrideMaterial.IsValid() && !OverrideMaterial.LoadSynchronous()) {
			PCGE_LOG(Error, GraphAndLog, FText::Format(LOCTEXT("OverrideMaterialLoadFailed", "Unable to load override material '{0}'"), FText::FromString(OverrideMaterial.ToString())));
			return;
		}
	}

#if ENGINE_MINOR_VERSION>4
	FPCGISMComponentBuilderParams parms;
	parms.Descriptor=InstanceList.Descriptor;
#else
	FPCGISMCBuilderParameters parms;
	parms.Descriptor = FISMComponentDescriptor(InstanceList.Descriptor);
#endif
	parms.NumCustomDataFloats=PackedCustomData.NumCustomDataFloats;

	// If the root actor we're binding to is movable, then the ISMC should be movable by default
	if(USceneComponent* SceneComponent=TargetActor->GetRootComponent()) {
		parms.Descriptor.Mobility=SceneComponent->Mobility;
	}

	const UPCGrdInstStaticMeshSpawnerSettings* Settings=Context->GetInputSettings<UPCGrdInstStaticMeshSpawnerSettings>();
	check(Settings);

	UrdInstSubsystem* rdInstSubsystem=GEngine?GEngine->GetEngineSubsystem<UrdInstSubsystem>():nullptr;
	if(!rdInstSubsystem) {
		PCGE_LOG(Error,GraphAndLog,LOCTEXT("failed","Failed to find rdInst Subsystem"));
		return;
	}

	if(Settings->SpawnActor) {
		UrdInstData* dat=rdInstSubsystem->rdGetBase()->GetInstUserData(LoadedMesh,false);
		FName sid=rdInstSubsystem->rdGetBase()->rdGetSMXsid(ErdSpawnType::UseDefaultSpawn,LoadedMesh,InstanceList.Descriptor.OverrideMaterials,false,(ErdCollision)(dat->collision+1));
		Settings->SpawnActor->rdAddBakedInstances(sid,InstanceList.Instances);
		return;
	}

	UInstancedStaticMeshComponent* ISMC=rdInstSubsystem->rdGetInstanceGen(LoadedMesh);
	check(ISMC);

	const int32 PreExistingInstanceCount=ISMC->GetInstanceCount();
	const int32 NewInstanceCount=InstanceList.Instances.Num();
	const int32 NumCustomDataFloats=PackedCustomData.NumCustomDataFloats;
	if(ISMC->NumCustomDataFloats<NumCustomDataFloats) {
		ISMC->SetNumCustomDataFloats(NumCustomDataFloats);
	}

	// The index in ISMC PerInstanceSMCustomData where we should pick up to begin inserting new floats
	const int32 PreviousCustomDataOffset=PreExistingInstanceCount*NumCustomDataFloats;

	// Populate the ISM instances
	rdInstSubsystem->rdAddInstancesFast(ISMC,InstanceList.Instances);

	// Copy new CustomData into the ISMC PerInstanceSMCustomData
	if(NumCustomDataFloats>0) {
		for (int32 NewIndex=0;NewIndex<NewInstanceCount;++NewIndex) {
			ISMC->SetCustomData(PreExistingInstanceCount+NewIndex,MakeArrayView(&PackedCustomData.CustomData[NewIndex*ISMC->NumCustomDataFloats],NumCustomDataFloats));
		}
	}
	ISMC->UpdateBounds();
	{
		PCGE_LOG(Verbose, LogOnly, FText::Format(LOCTEXT("GenerationInfo", "Added {0} instances of '{1}' on actor '{2}'"),
			InstanceList.Instances.Num(), FText::FromString(InstanceList.Descriptor.StaticMesh->GetFName().ToString()), FText::FromString(TargetActor->GetFName().ToString())));
	}
}

//--------------------------------------------------------------------------------------------------
// AbortInternal
//--------------------------------------------------------------------------------------------------
#if ENGINE_MINOR_VERSION>3
void FPCGrdInstSpawnerElement::AbortInternal(FPCGContext* InContext) const {
}
#endif
