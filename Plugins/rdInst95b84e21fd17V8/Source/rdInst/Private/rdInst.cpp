//
// rdInst.cpp
//
// Copyright (c) 2022 Recourse Design ltd. All rights reserved.
//
// Version 1.51
//
// Creation Date: 1st October 2022
// Last Modified: 30th June 2025
//
// This handles the Startup and Shutdown of this Plugin.
//
// More information on the rdInst SourceCode can be found at https://recourse.nz/index.php/rdinst-source-code/
//
// TODO: (next version)
//  * ToDo:     Add timings to stats, including proxy scan time
//  * ToDo:     get proxies to update transform in prefabs
//  * ToDo:     Move rooms into place routine directly in editor
//  * ToDo:     Add peak usage to graph
//  * ToDo:		PCG node to spawn prefabs
//  * ToDo:		PCG node to feed the pointdata
//  * ToDo:		Add SpawnHarvested function
//  * ToDo:		Make a system for the proxy to be grouped with multiple instances
//  * ToDo:		Add routines to swap proxies within a radius of a location
//  * ToDo:     Add tag for auto-inst to use the tagged BP as the proxy and harvest (use centermost instance as the proxy swap instance)
//  * ToDo:     Implement the rows/cols for CreateSpawnActorFromSelection
//  * ToDo:     Finish alteration system
//  * ToDo:     add option to MoveToGround to either place at actors Z or individual Z
//  * ToDo:     finish off new proxyswap spawn on server
//  * ToDo:     some BPs not getting harvest with "Create Prefab from", but do with "Convert to Blueprint"
//  * ToDo:     stencil buffer highlighting for pickups
//  * ToDo:     dragging prefab into level, then undo is not removing the instances
//  * ToDo:     clean up the property categories
//  * ToDo:     add convert level to spawnactor (add icon for style)
//  * ToDo:     Scan ImpactProxies in the SpawnActors
//  * ToDo:     Add way to auto-instance streamed in levels
//  * ToDo:     add auto-inst conversion of CPD to PICD
//  * ToDo:     finish the update SpawnActor tool - implement the Update routine
//  * ToDo:     implement the copy spawnactors to level tool
//  * ToDo:     Add Create SpawnActor from Folder
//  * ToDo:     Select SpawnActor after creating (give options of what to select in create window)
//  * ToDo:     streamed in levels have a 2nd rdInstBaseActor
//
// version history:
//
// Version 1.51
//  * Added:    Tools to convert Prefabs data between the Fast Tables (don't evaluate in the Editor Outliner) and Arrays
//  * Added:	New routines for building the visibility in for actors and folders
//  * Added:    Builds soft-reference array for sids to package and migrate correctly
//  * Changed:  Class Picker now has a "None" option.
//  * Changed:  rdSetActorVisibility and rdSetFolderVisibility now use fast hiding/showing rather than rebuilding
//  * Changed:  Prefab Proxies are now generically Actor Proxies (any StaticMeshActor in the level can have a proxy)
//  * Fixed:    Proxies not swapping back to instances when outside of radius
//  * Fixed:    UE5.6 was rendering dragged in prefabs and causing collision issues
//  * Fixed:    Removed doubled up long distance scan value
//  * Fixed:    Show Probability was not working in prefabs
//  * Fixed:    Playing level with nested prefabs was not randomizing them (was setting seed all the same)
//  * Fixed:    Creating Prefab from old style mesh (non-X) wasn't working
//  * Fixed:    Show Probability for ChildActorComponents was not working
//  * Fixed:    Nested-Nested prefabs were not getting their locations correct
//  * Fixed:    Nested-Nested prefabs were not always hiding/showing correctly
//  * Fixed:    Text in TextActor in SpawnActors not being saved correctly
//  * Fixed:    Having 2 spawnactors with the same dataasset messing up pickups and TextActors
//  * Fixed:    Pickup from SpawnActor picking up same object twice
//  * Fixed:    Dragging a spawndata asset into the level is getting the z position wrong
//  * Fixed:    PCGSpawnMeshWithIndices crash with index out of bounds (line 513) (fixed by adding check)
//  * Fixed:    TextRenderActor/Actors with SpawnActor disappearing when playing

// Version 1.50
//  * Added:    rdBaseActor now fully supports override materials and reverse culling
//  * Added:    rdActor now fully supports override materials and reverse culling
//  * Added:    Placement structure now has an override material list and tickbox for reverse culling
//  * Added:    Auto Pickup and Highlight system
//  * Added:    PCG node options to add directly to rdSpawnActor Volumes
//  * Added:    More stats to CalcInstanceCounts (num comps, recycle counts etc)
//  * Added:    More Auto-Inst filtering
//  * Added:    AutoInstance filter for physics enabled objects
//  * Added:    Minimum num SMs before instancing setting for auto-instancing
//  * Added:    Function in rdSpawnActor to return the indices used by a mesh
//  * Added:    Randomization Settings Window
//  * Added:    CreateSpawnActor window and tool
//  * Added:    Proxy Settings Window
//  * Added:    Procedural Actor can now spawn on static meshes
//  * Added:    Auto-grow to the pooling
//  * Added:    DebugHUD for instance stats and graphs
//  * Added:    Delegates for Proxy swapping In and Out
//  * Added:    Over-all max cull distance
//  * Added:    Exposed proxy scan instance and actor results as public
//  * Added:    Reseat BaseActor for very large worlds
//  * Added:    Recyling and growth to the component pooling
//  * Added:    Support for MegaLights swapping shadow rendering to VSM in proximity
//  * Added:    Functions to Add/Remove/Change instances, recording and saving to rdActor and rdSpawnActor for BakedData
//  * Added:    Added DataAssets for Placement Data Objects, Proxies, Instance Settings and Randomization
//  * Added:    SpawnActors can now spawn relative to their location/rotation
//  * Added:    Runtime Harvest actors into prefab in rdActor
//  * Added:    Added Spawn and Procedural actors to the "Add Actors" panel and menu
//  * Added:    Function to return size of pools
//  * Added:    Function rdGrowActorPoolBy(x)
//  * Added:    Dragged in DataAssets Populates their contents to the rdSpawn/rdProcedural Actors
//  * Changed:  Categorized the base and subsystem methods
//  * Changed:  Made swapout proxy distance slightly larger
//  * Changed:  Moved auto-instancing over to base actor instances
//  * Changed:  Impact Proxies now pass on their CollisionHit to the newly spawned ProxyActors
//  * Fixed:    CalcInstanceCounts not including spawner instances
//  * Fixed:    Converting level to WorldPartition would fail with error about casting rdInstBaseActor to MetaData
//  * Fixed:    Building HLODs in a WorldPartition would fail with error about casting rdInstBaseActor to MetaData
//  * Fixed:    Crash when re-loading level with populate actor
//  * Fixed:    Instance Index passed into LoadProxyState Event was incorrect for all-mesh proxies
//  * Fixed:    Pickup Interact Msg Remove was not being called when moving away from object but still looking at it
//  * Fixed:    Pressing Interact would pick up objects already picked up
//  * Fixed:    Proxies and Highlighted PickUps are refreshed after a proxy or instance/actor change

//
// Version 1.44
//  * Added:    Added auto grow to Pooling
//

// Version 1.43
//  * Added:    Add option to "Spawn on Server" for proxy swapping
//  * Added:    Now turns on collision and scan interval when a proxy is added and it's not already set
//  * Added:    Option to use CullDistanceVolume Cull Settings for AutoInstancing
//  * Added:    New functions to find instance index from location
//  * Added:    Option to call ProxyLoad/Save events even without a SavedState object
//  * Added:    PCG Custom Nodes to Spawn rdInst Instances
//  * Added:    Option to pin proxyActor to level with rdRemoveProxyByProxy
//  * Added:    New functions to get number of free isms in the recycle lists
//  * Added:    Added Instance settings window
//  * Added:    Added About window
//  * Added:    Make functions for each proxy type
//  * Added:    Pointer to the meshes ISMC/HISMC for proxies in rdActor and rdLoadProxyState Event
//  * Changed:  Use the currently possessed pawn for proxy distance scanning
//  * Changed:  ProxySetup structures now get passed with defaults in functions
//  * Fixed:    rdSpawnActors weren't initially spawning in packaged builds
//
// Version 1.42
//  * Added:    Agnostic function to return rdInst version number
//  * Added:    Add support for swapping in/out data layers from WP levels for proxies
//  * Added:    Add support for swapping in/out SM Instances for proxies
//  * Added:    Add support for Collision Profile name for ISMs
//  * Added:    Add support for Collision Profile name for SpawnActor place-on-ground
//  * Added:    Add Sphere/Box trace through list of ISMCs and change CustomData on overlaps
//  * Added:    Selection highlight around selected prefabs
//  * Fixed:    Undo was loosing the instances
//  * Fixed:    OnlyOne mode was not hiding instances from within childactors
//  * Fixed:    Hide/Show from editor not hiding/showing the prefabs
//  * Fixed:    Auto Instancing was broken
//  * Fixed:    Add path to SM name references
//
// Version 1.41
//  * Added:    Support for SaveStates in non rdActors
//  * Added:    Timeout for physics based proxies
//  * Fixed:    Proxies with States would loose some of their data in certain situations
//
// Version 1.40
//  * Added:    Tool to convert all actors into ISMs at BeginPlay
//  * Added:    Methods/Nodes to set/update custom data for ISMCs
//  * Added:    New simplified pool system for tricky components
//  * Added:    Flag to SetCustomData to Batch
//  * Added:    CustomData routines/nodes to set customdata for unmanaged instance components
//  * Added:    Routines to set all CustomData from Array
//  * Added:    Flag to recycle pooled items when alloted objects maxed out
//  * Added:    Flag to use ISMCs rather than HISMCs for Nanite meshes
//  * Added:    Fast rdFillCustomData routines
//  * Changed:  The rdInstBase is now visible in Outliner and contains all settings for rdInst
//  * Changed:  Using rdAddInstanceLong now turns off bCreateFromArrays if it's true
//  * Changed:  Made the SetHarvest flag default to False in the function
//	* Fixed:    bCreateFromArrays was defaulting to true by mistake
//  * Fixed:    Added constructors with FObjectInitializer objects
//  * Fixed:    Reseating meshes with scale and rotation was broken
//  * Fixed:    Proxies were not swapping for baked spawn data
//  * Fixed:    Baked Proxy data was not saving with the level
//
// Version 1.36
//  * Added:    Official support for Android
//	* Added:    Made baking transforms for Volumes optional and added a return list of created FrdBakedSpawnItems
//	* Added:    Function to rdSpawnActor for tracing to landscape
//  * Added:    Offset Vector to spline population
//  * Added:    Gap value for spline population
//  * Added:    Offset Transform for destroyed proxy
//  * Added:    Routine to switch to destroyed mesh or prefab
//  * Added:    Implemented the "On NavMesh" option
//  * Added:    Ability to edit individual instance transforms
//  * Added:    Function to test is playing
//  * Added:    Routine to rdActor to get all instances for mesh in actor
//  * Added:    Routines to rdActor to set Per-Instance data for instances in the rdActor
//  * Changed:  Moved the IgnorePhysicalActors from rdProcedural to rdSpawnActor
//  * Fixed:    Remove proxies was not working for volumes
//  * Fixed:    Baking now bakes all scalability levels
//  * Fixed:    Slopes weren't getting the line trace exact for the lower trace location
//  * Fixed:    Hiding PCG instances was not working
//  * Fixed:    Removed Python warning message when packaging project with rdInst
//
// Version 1.35
//  * Added:	System to show/hide instances
//  * Added:	Proxy System to convert from instances to ProxyActors in proximity - per instance or mesh
//  * Added:	Proxy System to convert from instances ProxyActors in Long-Distance scans - for AI etc
//  * Added:	Proxy System to handle swapping to Destruction Geometry Collections
//  * Added:	Runtime Spline split/join routines
//  * Added:	Sphere/Box tracing for actors/instances
//  * Added:	Hook to execute after construction
//	* Added:	System to delete attached actors on destruction
//  * Added:	Visiblefolders optionally recurse with a flag
//  * Added:	Functions to move spline points
//  * Added:	Functions to swap between ISM and SM
//  * Added:    State system to the proxies that can be saved in each instance
//  * Added:	Add agnostic function to shift specific meshes by a certain amount
//  * Added:    rdSpawnActor
//  * Added:    rdPopulateActor
//	* Added:    Baked instance data for fast Proxy scanning
//  * Changed:	Get harvest returning valid names
//
// Version 1.3
//  * Added:	Option to use either HISMs or ISMs
//  * Added:	Scalable Procedural Foliage Population system
//  * Added:	Return type morphing for Spawn and Pool nodes
//  * Changed:	New system for manually adding instances from blueprint
//  * Added:	rdInstSubsystem
//
// Version 1.24
//  * Added:	Function called "rdSetActorVisibility" to actor like simple Actor Visibility
//  * Added:	Function called "rdTestActorChanged" for updating instances transform and visiblity
//  * Added:	New Actor Mode called "BuildInstances" which just maintains the InstanceList
//	* Added:	Helper functions to wait for asset compilation, find the number of waiting assets and force all materials to compile
//	* Added:	OnlyOne parsing to handle assets without folders
//  * Fixed:	Base Actor would stream with world partition in UE5
//	* Fixed:	Incorrect instance index would be used in some situations
//	* Fixed:	shadows on instances not showing until playing
//	* Fixed:	rdActor not deleting instances when construction is run
//	* Changed:	make it default to settings that show an instance if you simply add one
//
// Version 1.22
//	* Added:	Function called "rdReturnActorToPool"
//  * Added:	Extra Debugging information
//	* Added:	Overridden HasFallenOffWorld to return actors to pool if they're pooled
//	* Added:	Method to test if an actor is pooled
//	* Fixed:	Exception on UE exit in certain situations
//  * Fixed:	Possible crash if pooled actor was null
//  * Fixed:	Actor Pooling events were receiving the base actor reference rather than the pooled actor
//
// Version 1.21
//	* Added:	Events for Pool and Depool, Actors and Components
//	* Added:	Pool Listener for Events not on rdActors
//	* Added:	Bool when creating Pools to enable Ticks when actors added to level from Pool
//	* Fixed:	GetBaseActor() was not showing in Blueprint Dropdown list when dragging from a execution pin
//	* Fixed:	Non-rdActors would not always stop and start their Ticks correctly
//
// Version 1.20
//  * Added:	rdActor: rdAddInstances(UStaticMesh* mesh,const TArray<FTransform>& transformList);
//  * Added:	rdActor: Distributed Transaction version of rdAddInstances
//  * Added:	rdInstBaseActor: int32 rdAddInstancesFast(UHierarchicalInstancedStaticMeshComponent* instGen,const TArray<FTransform>& transforms);
//  * Added:	rdInstBaseActor: int32 rdAddOwnedInstances(const AActor* instOwner,UHierarchicalInstancedStaticMeshComponent* instGen,const TArray<FTransform>& transforms); 
//
// Version 1.10 - released 30 January 2023
//	* Added:	Update Per-Instance Custom data routine (calls fix in 4.27 etc)
//	* Added:	Exposed rdBaseActor as protected getter rdGetBase()
//	* Added:	Routine that returns a reference to the TArray of Per-Instance CustomData
//	* Added:	Function to give ability to profile blueprint functions
//  * Added:	Actor Pooling
//  * Added:	Component Pooling
//	* Added:	Convert Instance to Actor (any actor class) method
//	* Added:	Convert Instance to Actor From Pool method
//
// Version 1.01 - released 
//
//   * Added:	Per-Instance Custom Array added to the Position Info to allow any amount of custom data to be passed to instances for materials
//	 * Fixed:	Added test for rougue actors before removing their instances
//
// Version 1.00 - released December 2022
//
#include "rdInst.h"
#include "Engine/StaticMeshActor.h"
#include "rdActor.h"
#include "Misc/OutputDeviceNull.h"
#include "Kismet/GameplayStatics.h"
#include "rdInstSubsystem.h"
#include "rdSpawnActor.h"
#include "rdProceduralActor.h"
#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>0
#include "AssetCompilingManager.h"
#include "Materials/MaterialInterface.h"
#else
#include "ShaderCompiler.h"
#endif
#include "rdDebugHUD.h"

#define LOCTEXT_NAMESPACE "FrdInstModule"

ArdInstBaseActor* rdBaseActor=nullptr;

#if !UE_BUILD_SHIPPING
TAutoConsoleVariable<int32> CVrdInstDebug(TEXT("rdInst.Debug"),0,TEXT("Open the rdInst Debug HUD by setting to non-zero"));
TAutoConsoleVariable<float> CVrdInstScanTime(TEXT("rdInst.Scantime"),0.5f,TEXT("How often to scan the instance stats in seconds"));
TAutoConsoleVariable<int32> CVrdInstScanSize(TEXT("rdInst.ScanSize"),32,TEXT("The size of the stored stats table for graph rendering"));
FAutoConsoleCommand FCmdLogStats(TEXT("rdInst.LogStats"),TEXT("Outputs the Instance Stats to the Log"),FConsoleCommandDelegate::CreateLambda([](){FrdInstModule::LogInstanceStats();}));
FAutoConsoleCommand FCmdLogStatsVerbose(TEXT("rdInst.LogStatsVerbose"),TEXT("Outputs the Instance Stats to the Log with each instances transform"),FConsoleCommandDelegate::CreateLambda([](){FrdInstModule::LogInstanceStatsVerbose();}));
#endif

#if !UE_BUILD_SHIPPING
int32					FrdInstModule::lastDebugLevel=0;
float					FrdInstModule::debugScanLapse=0.0f;
int32					FrdInstModule::debugScanSize=32;
TObjectPtr<UrdDebugHUD> FrdInstModule::debugHUD=nullptr;
#endif

//.............................................................................
//  StartupModule
//.............................................................................
void FrdInstModule::StartupModule() {

#if WITH_EDITOR
	FCoreDelegates::OnPostEngineInit.AddLambda([]() {
		GEngine->OnPostEditorTick().AddStatic(&FrdInstModule::rdTick);
	});
#endif
}

//.............................................................................
//  ShutdownModule
//.............................................................................
void FrdInstModule::ShutdownModule() {
}

//----------------------------------------------------------------------------------------------------------------
// rdTick
//
// During construction of the levels Actors and their Components - the construction can be called many times.
//   Each viewport has the actors constructed too - so editor windows open causes more construction.
//
// To try and ease the pipeline a little (as we're wanting to be able to use the Editor with many rdInstances)
// the constructors just mark the actors as dirty by adding them to our global dirty list (Editor only).
//
// Then from tick, which gets called after all the construction has been done, we do the real work - it is a lot
// of processing for a tick, but it's only once, and is the one immediately after load/init has finished. (when I
// say a lot of processing, it's still only millis we're talking about)
//----------------------------------------------------------------------------------------------------------------
#if WITH_EDITOR
void FrdInstModule::rdTick(float deltaSeconds) {

#if !UE_BUILD_SHIPPING
	int32 debugLevel=CVrdInstDebug.GetValueOnGameThread();
	if(debugLevel!=lastDebugLevel) {
		lastDebugLevel=debugLevel;
		SetDebugHUDType(lastDebugLevel);		
		lastDebugLevel=debugLevel;
	}

	int32 scanSize=CVrdInstScanSize.GetValueOnGameThread();
	if(scanSize!=debugScanSize) {
		if(debugHUD && debugHUD->statsGraph) {
			debugScanSize=FMath::Max(1,scanSize);
			debugHUD->statsGraph->storedData.SetNumZeroed(debugScanSize*8);
		}
	}

	if(lastDebugLevel>0) {
		debugScanLapse+=deltaSeconds;
		if(debugScanLapse>CVrdInstScanTime.GetValueOnGameThread()) {
			debugScanLapse=0.0f;
			UpdateDebugHUD();
		}
	}
#endif

#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>3
	int32 numCompiling=0;//FAssetCompilingManager::Get().GetNumRemainingAssets();
#else
	int32 numCompiling=0;//GShaderCompilingManager->GetNumRemainingJobs();
#endif

	if(numCompiling==0) {

		// Dirty List
		TArray<ArdActor*> removeList;

		TArray<ArdActor*> tempList=ArdActor::rdGetDirtyList();
		for(auto i:tempList) {
			if(IsValid(i)) {
				if(i->dirtyCountDown<=0) {
					i->rdBuild();
					removeList.Add(i);
				} else {
					i->dirtyCountDown--;
				}
			}
		}
		for(auto i:removeList) ArdActor::rdGetDirtyList().Remove(i);
	}

	// TickHook List
	ArdInstBaseActor* rdBA=nullptr;
	bool isPlaying=false;
	TArray<ArdActor*> remList;
	FOutputDeviceNull ar;
	TArray<ArdActor*> hl=ArdActor::rdGetTickHookList();
	for(auto i:hl) {
		if(IsValid(i)) {
			if(!rdBA) {
				rdBA=i->rdGetBase();
				if(rdBA) isPlaying=rdBA->rdIsPlaying();
			}
			if(!isPlaying || i->bTickHookOneShot) {
				i->CallFunctionByNameWithArguments(TEXT("TickHook"),ar,NULL,true);
			}
			//i->rdBuild();
			if(i->bTickHookOneShot) remList.Add(i);
		}
	}
	for(auto a:remList) {
		ArdActor::rdGetTickHookList().Remove(a);
	}
}
#endif

//.............................................................................
#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FrdInstModule, rdInst)