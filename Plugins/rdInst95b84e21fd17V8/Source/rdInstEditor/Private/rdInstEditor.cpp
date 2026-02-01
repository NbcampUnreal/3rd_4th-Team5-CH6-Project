//
// rdInstEditor.cpp
//
// Copyright (c) 2022 Recourse Design ltd. All rights reserved.
//
// Version 1.50
//
// Creation Date: 18th May 2025
// Last Modified: 27th June 2025
//
#include "rdInstEditor.h"
#include "Engine/StaticMeshActor.h"
#include "rdInstOptions.h"
#include "rdInstSubsystem.h"
#include "Editor.h"
#include "LevelEditor.h"
#include "Engine/Selection.h"
#include "SLevelViewport.h"
#include "LevelViewportClickHandlers.h"
#include "Styling/SlateStyle.h"
#include "Styling/SlateStyleRegistry.h"
#include "Interfaces/IPluginManager.h"
#include "Editor/ContentBrowser/Public/ContentBrowserModule.h"
#include "DetailLayoutBuilder.h"
#include "DetailCategoryBuilder.h"
#include "ToolMenus.h"
#include "DetailWidgetRow.h"
#if ENGINE_MAJOR_VERSION>4
#include "Subsystems/PlacementSubsystem.h"
#endif
#include "IPlacementModeModule.h"
#include "rdActor.h"
#include "Kismet/GameplayStatics.h"
#include "rdSpawnActor.h"
#include "rdProceduralActor.h"
#include "rdActorFactory.h"

#define LOCTEXT_NAMESPACE "FrdInstEditorModule"

ArdInstBaseActor* rdBaseActor=nullptr;

UrdMultiToken* rdNonDeterminantInstance=nullptr;

TArray<TObjectPtr<UObject>>		FrdInstEditorModule::selection;
bool							FrdInstEditorModule::selectionTriggered=false;
bool							FrdInstEditorModule::hasLevelLoaded=false;

TArray<TObjectPtr<ArdActor>>	FrdInstEditorModule::selectedActors;
TArray<TObjectPtr<ArdActor>>	FrdInstEditorModule::prefabsWithSelectedISMs;
bool							FrdInstEditorModule::ignoreSelChange=false;
bool							FrdInstEditorModule::rdSelectNone=false;
bool							FrdInstEditorModule::wasSelected=false;
TObjectPtr<AActor>				FrdInstEditorModule::rdDeselectActor=nullptr;
TObjectPtr<ArdActor>			FrdInstEditorModule::editInstanceActor=nullptr;

TObjectPtr<UInstancedStaticMeshComponent> FrdInstEditorModule::lastSelectedISMC=nullptr;
int32							FrdInstEditorModule::lastSelectedInstanceIndex=-1;
TObjectPtr<AActor>				FrdInstEditorModule::lastSelectedPrefab=nullptr;

TSharedPtr<FSlateStyleSet>		FPluginStyle::StyleSet=nullptr;

//.............................................................................
//  StartupModule
//
// Most of the guff in here is for the Editor side of things.
//  It adds the menus and hooks needed for actor editing.
//  The hooks are added after the Editor has finished initializing via the 
//  OnPostEngineInit lamda.
//
//.............................................................................
void FrdInstEditorModule::StartupModule() {

//	if(!IsRunningGame()) {

		FPluginStyle::Initialize();
		FPluginStyle::ReloadTextures();

		// Menus

		// Content Browser Context Menu
		FContentBrowserModule& ContentBrowserModule=FModuleManager::LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"));
		TArray<FContentBrowserMenuExtender_SelectedAssets>& MenuExtenderDelegates=ContentBrowserModule.GetAllAssetViewContextMenuExtenders();
		MenuExtenderDelegates.Add(FContentBrowserMenuExtender_SelectedAssets::CreateRaw(this,&FrdInstEditorModule::rdMenuExtender));

		// Main Editor Tool Menu
		UToolMenu* menu=UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Tools");
#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>3
		FToolMenuSection& section=menu->FindOrAddSection("PROGRAMMING");
#else
		FToolMenuSection& section=menu->FindOrAddSection("Tools");
#endif
		FToolMenuEntry* rdToolsToolMenu=section.FindEntry("rdToolsToolMenu");
		if(!rdToolsToolMenu) {
			rdToolsToolMenu=&section.AddEntry(FToolMenuEntry::InitSubMenu("rdToolsToolMenu",LOCTEXT("rdTools_ToolMenu","rdTools"),LOCTEXT("rdTools_ToolMenu_Tooltip1","rdTools SubMenu"),FNewToolMenuChoice(),false,FSlateIcon(FPluginStyle::GetStyleSetName(),"rdTools.Icon","rdTools.Icon")));
		}
		UToolMenu* tmenu=UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Tools.rdToolsToolMenu");
		if(tmenu) {
			FToolMenuSection& tsec=tmenu->FindOrAddSection("rdTools");
			tsec.AddEntry(FToolMenuEntry::InitSubMenu("rdInst",LOCTEXT("rdInst_ToolMenu","rdInst"),LOCTEXT("rdInst_ToolMenu_Tooltip1","rdInst SubMenu"),FNewToolMenuDelegate::CreateRaw(this,&FrdInstEditorModule::ToolMenuSubMenu),false,FSlateIcon(FPluginStyle::GetStyleSetName(),"rdInst.Icon","rdInst.Icon")));
		}

		// LevelEditor Context Menu
		menu=UToolMenus::Get()->ExtendMenu("LevelEditor.ActorContextMenu");
#if ENGINE_MAJOR_VERSION>4			
		FToolMenuSection& lvlsection=menu->FindOrAddSection("ActorGeneral");
#else
		FToolMenuSection& lvlsection=menu->FindOrAddSection("ActorControl");
#endif
		lvlsection.AddEntry(FToolMenuEntry::InitSubMenu("rdInst",LOCTEXT("rdInst_LevelMenu","rdInst"),LOCTEXT("rdInst_LevelMenu_Main_Tooltip1","rdInst SubMenu"),FNewToolMenuDelegate::CreateRaw(this,&FrdInstEditorModule::LevelMenuSubMenu),false,FSlateIcon(FPluginStyle::GetStyleSetName(),"rdInst.Icon","rdInst.Icon")));

		// Hooks (via OnPostEngineInit)

		// Delegates
		FCoreDelegates::OnPostEngineInit.AddLambda([]() {
			GEngine->OnLevelActorDeleted().AddStatic(&FrdInstEditorModule::OnActorRemoved);
			GEngine->OnPostEditorTick().AddStatic(&FrdInstEditorModule::rdTick);
			GEngine->OnWorldAdded().AddStatic(&FrdInstEditorModule::rdOnWorldAdded);
			GEngine->OnLevelActorAdded().AddStatic(&FrdInstEditorModule::rdOnLevelActorAdded);

			GEngine->OnWorldDestroyed().AddStatic(&FrdInstEditorModule::rdOnWorldDestroyed);
			FLevelEditorModule& LevelEditorModule=FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
			LevelEditorModule.OnActorSelectionChanged().AddStatic(&FrdInstEditorModule::OnActorSelectionChanged);

#if ENGINE_MAJOR_VERSION>4
			LevelEditorModule.OnElementSelectionChanged().AddStatic(&FrdInstEditorModule::OnElementSelectionChanged);
#endif
			//	Handles events where an instance of an Instanced Static Mesh is about to be removed.
			//	LevelEditorModule.OnIsmInstanceRemoving(const FSMInstanceElementId& SMInstanceElementId, int32 InstanceIndex);

			// Add rdSpawnActor and rdProceduralActors to the AddActors panel
#if ENGINE_MAJOR_VERSION>4
			const FPlacementCategoryInfo pinfo(LOCTEXT("rdActors","rdActors"),FSlateIcon(FPluginStyle::GetStyleSetName(),"rdTools.Icon","rdTools.Icon"),"rdActors",TEXT("rdActors"),100);
#else 
			const FPlacementCategoryInfo pinfo(LOCTEXT("rdActors","rdActors"),"rdActors",TEXT("rdActors"),100);
#endif
			IPlacementModeModule& pmod=IPlacementModeModule::Get();
			pmod.RegisterPlacementCategory(pinfo);
			pmod.RegisterPlaceableItem(pinfo.UniqueHandle,MakeShared<FPlaceableItem>(nullptr,FAssetData(ArdSpawnActor::StaticClass())));
			pmod.RegisterPlaceableItem(pinfo.UniqueHandle,MakeShared<FPlaceableItem>(nullptr,FAssetData(ArdProceduralActor::StaticClass())));

			// Factory to drag Baked DataAssets and Placement DataAssets straight into the level
			//GEditor->GetEditorSubsystem<UPlacementSubsystem>()->RegisterAssetFactory(NewObject<UrdSpawnActorFactory>());

#if ENGINE_MAJOR_VERSION<5
			FEditorDelegates::PreSaveWorld.AddStatic(&FrdInstEditorModule::OnPreSaveWorld);
#else
			FEditorDelegates::PreSaveWorldWithContext.AddStatic(&FrdInstEditorModule::OnPreSaveWorld);
#endif
		});

		ignoreSelChange=false;

		// Details Panel Custom Property for rdSpawnActors
		FPropertyEditorModule& propModule=FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
		propModule.RegisterCustomClassLayout(ArdSpawnActor::StaticClass()->GetFName(),FOnGetDetailCustomizationInstance::CreateStatic(&FSpawnActorDetailsCustomization::MakeInstance));
//	}
}

//.............................................................................
//  ShutdownModule
//.............................................................................
void FrdInstEditorModule::ShutdownModule() {
	/*
	GEngine->OnLevelActorDeleted().RemoveAll(this);
	GEngine->OnPostEditorTick().RemoveAll(this);
	GEngine->OnWorldAdded().RemoveAll(this);
	GEngine->OnWorldDestroyed().RemoveAll(this);

	FLevelEditorModule& LevelEditorModule=FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	LevelEditorModule.OnActorSelectionChanged().RemoveAll(this);
	*/
}

//.............................................................................
//  OnPreSaveWorld
//
// Empties the rdInstBaseActors arrays of referenced assets (used for migrating and packaging)
//
//.............................................................................
#if ENGINE_MAJOR_VERSION<5
void FrdInstEditorModule::OnPreSaveWorld(uint32 SaveFlags,UWorld* world) {
#else
void FrdInstEditorModule::OnPreSaveWorld(UWorld* world,FObjectPreSaveContext ObjectSaveContext) {
#endif

	UrdInstSubsystem* rdInstSubsystem=GEngine?GEngine->GetEngineSubsystem<UrdInstSubsystem>():nullptr;
	if(rdInstSubsystem) {
		ArdInstBaseActor* rdBase=rdInstSubsystem->rdGetBase();
		if(rdBase) {
			rdBase->LevelReferencedMeshes.Empty();
			rdBase->LevelReferencedMaterials.Empty();
			rdBase->LevelReferencedActorClasses.Empty();
			rdBase->LevelReferencedDataLayers.Empty();
			rdBase->LevelReferencedVFX.Empty();

			// add any proxy objects in the level actor tags
			TArray<AActor*> actors;
			UGameplayStatics::GetAllActorsOfClass(world,AActor::StaticClass(),actors);
			FrdProxySetup proxy;
			for(AActor* a:actors) {
				for(FName tag:a->Tags) {
					FString stag=tag.ToString();
					if(stag.StartsWith(TEXT("rdProxy="))) {
						proxy.FromString(stag.RightChop(8));
						rdBase->AddProxyToReferencedAssets(proxy);
					}
				}
			}
		}
	}
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
void FrdInstEditorModule::rdTick(float deltaSeconds) {

	if(!rdNonDeterminantInstance) {

		FWorldContext* world=GEngine->GetWorldContextFromGameViewport(GEngine->GameViewport);
		UWorld* World=world->World();
		AActor* actor=UGameplayStatics::GetActorOfClass(World,ArdInstBaseActor::StaticClass());
		if(actor) {
			ArdInstBaseActor* baseActor=Cast<ArdInstBaseActor>(actor);
			if(baseActor) {
				rdNonDeterminantInstance=NewObject<UrdMultiToken>(baseActor,FName("Nondeterminant"),RF_Transient);
			}
		}
	}
/*
#if ENGINE_MAJOR_VERSION>4
	// Selection Changes
	if(hasLevelLoaded && selectionTriggered) {
		selectionTriggered=false;
		rdSelectionChanged();
	}
#endif
*/
}

//----------------------------------------------------------------------------------------------------------------
// OnActorRemoved
//----------------------------------------------------------------------------------------------------------------
void FrdInstEditorModule::OnActorRemoved(AActor* actor) {

	ArdActor* rdActor=Cast<ArdActor>(actor);
	if(rdActor) {
		rdActor->rdRemoveInstances();
		rdActor->rdDestroyAttachedActors();
	}
}

//----------------------------------------------------------------------------------------------------------------
// rdOnLevelActorAdded
//----------------------------------------------------------------------------------------------------------------
void FrdInstEditorModule::rdOnLevelActorAdded(AActor* actor) {

//	UClass* uclass=actor->StaticClass();

}

//----------------------------------------------------------------------------------------------------------------
// rdOnWorldAdded
// This is hooked to add the rdInstBase Actor to the level
//----------------------------------------------------------------------------------------------------------------
void FrdInstEditorModule::rdOnWorldAdded(UWorld* world) {

	hasLevelLoaded=true;
	selectedActors.Empty();
	prefabsWithSelectedISMs.Empty();
	
	if(!world || world->WorldType!=EWorldType::Editor) return;

	TArray<AActor*> existingBaseActors;
	UGameplayStatics::GetAllActorsOfClass(world,ArdInstBaseActor::StaticClass(),existingBaseActors);
	if(existingBaseActors.Num()>0) {
		rdBaseActor=Cast<ArdInstBaseActor>(existingBaseActors[0]);
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride=ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.ObjectFlags=RF_Transactional;//|RF_Transient//RF_Public
	SpawnParams.bAllowDuringConstructionScript=true;
	SpawnParams.Owner=nullptr;

	rdBaseActor=(ArdInstBaseActor*)world->SpawnActor<ArdInstBaseActor>(SpawnParams);

	if(!rdBaseActor) {
		return;
	}

	rdBaseActor->ClearFlags(RF_HasExternalPackage);

#if ENGINE_MAJOR_VERSION>4
	rdBaseActor->SetIsSpatiallyLoaded(false);
#endif
	rdBaseActor->SetActorLabel("rdInst Settings");

	rdBaseActor->SetActorLocation(FVector(0,0,0));
	rdBaseActor->Tags.Add(TEXT("rdBaseActor"));
}

//----------------------------------------------------------------------------------------------------------------
// rdOnWorldDestroyed
//----------------------------------------------------------------------------------------------------------------
void FrdInstEditorModule::rdOnWorldDestroyed(UWorld* world) {

	selectedActors.Empty();
//#if !UE_BUILD_SHIPPING
//	RemoveDebugHUD();
//#endif
	hasLevelLoaded=false;
}


//----------------------------------------------------------------------------------------------------------------
// OnIsmInstanceRemoving
//----------------------------------------------------------------------------------------------------------------
//#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>2
//void FrdInstEditorModule::OnIsmInstanceRemoving(const FSMInstanceElementId& instanceElementId,int32 instanceIndex) {
	//UE_LOG(LogTemp,Display,TEXT("OnIsmInstanceRemoving=%d"),instanceIndex);
//}
//#endif

//----------------------------------------------------------------------------------------------------------------
// OnActorSelectionChanged
//----------------------------------------------------------------------------------------------------------------
void FrdInstEditorModule::OnActorSelectionChanged(const TArray<UObject*>& inSelection,bool bForceRefresh) {

	if(ignoreSelChange) return;


	selection=inSelection;

	if(hasLevelLoaded) {
		//selectionTriggered=false;
		rdSelectionChanged();
	}
	//selectionTriggered=true;
}

//----------------------------------------------------------------------------------------------------------------
// OnElementSelectionChanged
//----------------------------------------------------------------------------------------------------------------
#if ENGINE_MAJOR_VERSION>4
void FrdInstEditorModule::OnElementSelectionChanged(const UTypedElementSelectionSet* SelectionSet,bool bForceRefresh) {
			
	if(ignoreSelChange) return;
}
#endif

//----------------------------------------------------------------------------------------------------------------
// rdSelectionChanged
//
// Only in Editor Mode
// This is used to either find the parent of an instance to select that, or in EditMode, to select the instance.
//
//----------------------------------------------------------------------------------------------------------------
void FrdInstEditorModule::rdSelectionChanged() {

#if ENGINE_MAJOR_VERSION>4

	//selectionTriggered=false;

	const bool menuOpen=FSlateApplication::Get().AnyMenusVisible();
	bool beingEdited=false,hadSelection=false;
	if(!menuOpen) {

		for(auto a:selectedActors) {

			// Debug Test (will remove)
			if(((uint64)a)==0x0000000000000000 || ((uint64)a)==0xFFFFFFFF00000000 || ((uint64)a)==0x00000000FFFFFFFF || ((uint64)a)==0xFFFFFFFFFFFFFFFF) {
				UE_LOG(LogTemp,Display,TEXT("Invalid Actor Pointer in Selection"));
				continue; 
			}

			if(!selection.Contains(a) && IsValid(a) && !a->bEditInstances) {
				a->HidePrefabOutline();
			}
			hadSelection=true;
		}

		TArray<ArdActor*> selectedActors2;
		for(auto s:selection) {

			// Debug Test (will remove)
			if(((uint64)s)==0x0000000000000000 || ((uint64)s)==0xFFFFFFFF00000000 || ((uint64)s)==0x00000000FFFFFFFF || ((uint64)s)==0xFFFFFFFFFFFFFFFF) {
				UE_LOG(LogTemp,Display,TEXT("Invalid Actor Pointer in Selection"));
				continue; 
			}

			ArdActor* rdActor=Cast<ArdActor>(s);
			if(rdActor && !selectedActors.Contains(rdActor) && IsValid(rdActor)) {
				rdActor->bEditInstances=false;
				rdActor->ShowPrefabOutline();
				selectedActors2.AddUnique(rdActor);
			}
		}

		selectedActors=selectedActors2;

		UrdInstSubsystem* rdInstSubsystem=GEngine?GEngine->GetEngineSubsystem<UrdInstSubsystem>():nullptr;
		if(rdInstSubsystem) {
			ArdInstBaseActor* rdBase=rdInstSubsystem->rdGetBase();
			if(rdBase && !rdBase->rdIsPlaying() && hasLevelLoaded) {
				prefabsWithSelectedISMs.Empty();
				TArray<ArdActor*> actors;
				int32 num=rdBase->GetActorsWithSelectedISMS(actors,beingEdited);
				for(auto a:actors) {
					prefabsWithSelectedISMs.Add(a);
				}
			}
		}
	}

	if(menuOpen && FrdInstEditorModule::lastSelectedPrefab) {

		ignoreSelChange=true;

		for(auto a:selectedActors) {
			GEditor->SelectActor(a,true,true);
		}
		prefabsWithSelectedISMs.Empty();

		TSharedPtr<SWindow> mnuWnd=FSlateApplication::Get().GetVisibleMenuWindow();
		TSharedPtr<SWidget> mnuWidget=FSlateApplication::Get().GetMenuHostWidget();

		FLevelEditorModule& levelEditor=FModuleManager::GetModuleChecked<FLevelEditorModule>("LevelEditor");
		SLevelViewport* levelViewport=levelEditor.GetFirstActiveLevelViewport().Get();
		FLevelEditorViewportClient& viewportClient=levelViewport->GetLevelViewportClient();
		FEditorViewportClient* evc=GLevelEditorModeTools().GetFocusedViewportClient();
		FViewport* viewport=viewportClient.Viewport;
		FSceneViewFamilyContext viewFamily(FSceneViewFamily::ConstructionValues(viewport,viewportClient.GetScene(),viewportClient.EngineShowFlags));
		FSceneView* view=viewportClient.CalcSceneView(&viewFamily);

		FKey key(TEXT("RightMouseButton"));
		FViewportClick click(view,evc,key,EInputEvent::IE_Released,viewport->GetMouseX(),viewport->GetMouseY());

		LevelViewportClickHandlers::ClickActor(&viewportClient,FrdInstEditorModule::lastSelectedPrefab,click,true);
		ignoreSelChange=false;

		selectedActors.Empty();

		return;
	}


	if(selection.Num()==0 && prefabsWithSelectedISMs.Num()==0 && !beingEdited) {
		if(hadSelection) {
			rdSelectNone=true;
			editInstanceActor=nullptr;
		}
	} else if(prefabsWithSelectedISMs.Num()==1 && prefabsWithSelectedISMs[0]->bEditInstances) {
		editInstanceActor=prefabsWithSelectedISMs[0];
		prefabsWithSelectedISMs.Empty();
	} else if(editInstanceActor) {
		editInstanceActor->bEditInstances=false;
		editInstanceActor=nullptr;
	} else if(selection.Num()>0 && prefabsWithSelectedISMs.Num()>0 &&  FrdInstEditorModule::lastSelectedPrefab) {
		if(wasSelected) {
			// Deselect
			rdDeselectActor=FrdInstEditorModule::lastSelectedPrefab;
		}
		FrdInstEditorModule::lastSelectedPrefab=nullptr;
	}

	if(rdSelectNone) {

		ignoreSelChange=true;
		GEditor->SelectNone(true,true);
		rdSelectNone=false;
		ignoreSelChange=false;

	} else if(editInstanceActor) {

		editInstanceActor->rdUpdateInstanceTransforms();

	} else if(prefabsWithSelectedISMs.Num()>0) {

		TArray<AActor*> selActors;
		for(FSelectionIterator it(*GEditor->GetSelectedActors());it;++it) {
			AActor* aa=Cast<AActor>(*it);
			if(aa) {
				if(!prefabsWithSelectedISMs.Contains(aa)) {
					selActors.Add(aa);
				}
			}
		}

		ignoreSelChange=true;
		GEditor->SelectNone(true,true);
		for(auto a:selActors) {
			if(a!=rdDeselectActor) {
				GEditor->SelectActor(a,true,true);
			} 
		}

		for(auto a:prefabsWithSelectedISMs) {
			if(a!=rdDeselectActor) {
				GEditor->SelectActor(a,true,true);
				a->ShowPrefabOutline();
				selectedActors.AddUnique(a);
			} else {
				a->HidePrefabOutline();
			}
		}
		prefabsWithSelectedISMs.Empty();
		ignoreSelChange=false;
	}
	rdDeselectActor=nullptr;
#endif
}

//.............................................................................
// rdGetPackage
//.............................................................................
UPackage* FrdInstEditorModule::rdGetPackage(const FString name) {

	UPackage* package=FindPackage(NULL,*name);
	if(package) {
		package->FullyLoad();
	} else {
		package=LoadPackage(NULL,*name,LOAD_None);
	}
	return package;
}

//----------------------------------------------------------------------------------------------------------------
// SetOptionDefaults
//----------------------------------------------------------------------------------------------------------------
bool FrdInstEditorModule::SetOptionDefaults(const FString& name,UrdInstOptions* Options) {

	FString str=*Options->ToString();

	check(GConfig && GConfig->IsReadyForUse());

	GConfig->SetString(TEXT("CurrentSettings"),*name,*str,configIni);
	GConfig->Flush(0);

	return true;
}

//----------------------------------------------------------------------------------------------------------------
// FillFromOptionDefaults
//----------------------------------------------------------------------------------------------------------------
bool FrdInstEditorModule::FillFromOptionDefaults(const FString& name,UrdInstOptions* Options) {

	FString str;

	check(GConfig && GConfig->IsReadyForUse());

	if(!GConfig->GetString(TEXT("CurrentSettings"),*name,str,configIni)) {
		return false;
	}

	Options->FromString(str);

	return true;
}



#define IMAGE_BRUSH(RelativePath,...) FSlateImageBrush(StyleSet->RootToContentDir(RelativePath,TEXT(".png")),__VA_ARGS__)
#define IMAGE_PLUGIN_BRUSH(RelativePath,...) FSlateImageBrush(FPluginStyle::InContent(RelativePath,".png"),__VA_ARGS__)
#define IMAGE_BRUSH_SVG(RelativePath,...) FSlateVectorImageBrush(StyleSet->RootToContentDir(RelativePath,TEXT(".svg")),__VA_ARGS__)
#define BOX_BRUSH(RelativePath,...) FSlateBoxBrush(StyleSet->RootToContentDir(RelativePath,TEXT(".png")),__VA_ARGS__)

//----------------------------------------------------------------------------------------------------------------
// FPluginStyle - class for the rdBPtools Icons
//----------------------------------------------------------------------------------------------------------------
void FPluginStyle::Initialize() {

	if(StyleSet.IsValid()) {
		return;
	}

	StyleSet=MakeShared<FSlateStyleSet>(GetStyleSetName());
	StyleSet->SetContentRoot(FPaths::EngineContentDir()/TEXT("Editor/Slate"));
	StyleSet->SetCoreContentRoot(FPaths::EngineContentDir()/TEXT("Slate"));

	const FVector2D Icon12x12(12.0f,12.0f);
	const FVector2D Icon20x20(20.0f,20.0f);
	const FVector2D Icon40x40(40.0f,40.0f);

	// Icons for the mode panel tabs
	StyleSet->Set("rdTools.Icon",new IMAGE_PLUGIN_BRUSH(TEXT("Icons/rdTools_20px"),Icon20x20));
	StyleSet->Set("rdInst",new IMAGE_PLUGIN_BRUSH(TEXT("Icons/rdInst_40px"),Icon40x40));
	StyleSet->Set("rdInst.Small",new IMAGE_PLUGIN_BRUSH(TEXT("Icons/rdInst_40px"),Icon20x20));
	StyleSet->Set("rdInst.Logo",new IMAGE_PLUGIN_BRUSH(TEXT("Icons/rdInst_350px"),FVector2D(140.0,140.0)));
	StyleSet->Set("rdInst.Icon",new IMAGE_PLUGIN_BRUSH(TEXT("Icons/rdInst_20px"),Icon20x20));
	StyleSet->Set("rdInst.RandomSettings",new IMAGE_BRUSH(TEXT("Icons/icon_axis_world_16px"), Icon20x20));
	StyleSet->Set("rdInst.SetRelyOnActors",new IMAGE_BRUSH(TEXT("Icons/contentbrowser_16x"), Icon20x20));
	StyleSet->Set("rdInst.RemoveRandomSettings",new IMAGE_BRUSH(TEXT("Icons/icon_delete_16px"), Icon20x20));
	StyleSet->Set("rdInst.InstanceSettings",new IMAGE_BRUSH(TEXT("Icons/icon_keyb_StaticMeshEditor_16px"), Icon20x20));
	StyleSet->Set("rdInst.ProxySettings",new IMAGE_BRUSH(TEXT("Icons/icon_Persona_Retarget_Manager_16x"), Icon20x20));
	StyleSet->Set("rdInst.ConvertLevel",new IMAGE_BRUSH(TEXT("Icons/icon_Persona_Retarget_Manager_16x"), Icon20x20));
	StyleSet->Set("rdInst.About",new IMAGE_BRUSH(TEXT("Icons/AssetIcons/DocumentationActor_16x"), Icon20x20));
	StyleSet->Set("rdInst.Settings",new IMAGE_BRUSH(TEXT("Icons/icon_axis_world_16px"), Icon20x20));

	FSlateStyleRegistry::RegisterSlateStyle(*StyleSet.Get());
}

//----------------------------------------------------------------------------------------------------------------
// Shutdown
//----------------------------------------------------------------------------------------------------------------
void FPluginStyle::Shutdown() {

	if(StyleSet.IsValid()) {

		FSlateStyleRegistry::UnRegisterSlateStyle(*StyleSet.Get());
		ensure(StyleSet.IsUnique());
		StyleSet.Reset();
	}
}

//.............................................................................
// UI Style Methods
//.............................................................................
TSharedPtr<class ISlateStyle> FPluginStyle::Get() {
	return StyleSet; 
}	

FName FPluginStyle::GetStyleSetName() {
	static FName StyleName("rdInstStyle");
	return StyleName;
}

FString FPluginStyle::InContent(const FString& RelativePath,const ANSICHAR* Extension) {
	static FString ContentDir=IPluginManager::Get().FindPlugin(TEXT("rdInst"))->GetContentDir();
	return (ContentDir/RelativePath)+Extension;
}

void FPluginStyle::ReloadTextures() {
	if(FSlateApplication::IsInitialized()) {
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

//.............................................................................
// Custom Properties Detail Class
//.............................................................................

#if ENGINE_MAJOR_VERSION<5 && ENGINE_MINOR_VERSION<25
TSharedPtr<SWidget> FSimpleRootObjectCustomization::CustomizeObjectHeader(const UObject* InRootObject) {
#else
TSharedPtr<SWidget> FSimpleRootObjectCustomization::CustomizeObjectHeader(const FDetailsObjectSet & InRootObjectSet) {
#endif
	return SNullWidget::NullWidget;
}

TSharedRef<IDetailCustomization> FSpawnActorDetailsCustomization::MakeInstance() {
	return MakeShareable(new FSpawnActorDetailsCustomization);
}

void FSpawnActorDetailsCustomization::CustomizeDetails(const TSharedPtr<IDetailLayoutBuilder>& DetailBuilder) {
	CachedDetailBuilder=DetailBuilder;
	CustomizeDetails(*DetailBuilder);
}

void FSpawnActorDetailsCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) {

	TArray<TWeakObjectPtr<UObject>> objs;
	DetailBuilder.GetObjectsBeingCustomized(objs);

	TSharedPtr<IPropertyHandle> prop=DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(ArdSpawnActor,spawnData));

	if(objs.Num()!=1) {
		prop->MarkHiddenByCustomization();
	} else {
		spawnActor=Cast<ArdSpawnActor>(objs[0]);
		if(ensure(spawnActor.IsValid())) {

			numItems=spawnActor->spawnData[spawnActor->currentSpawnDataIndex].items.Num();
			bForceShowSpawnData=spawnActor->bForceShowAllItemsDetails;

			if(numItems>100) {
				if(!bForceShowSpawnData) {
					prop->MarkHiddenByCustomization();
				}

				const FName SpawnDataCatName("1. Object Data");
				IDetailCategoryBuilder& spawnDataCategory=DetailBuilder.EditCategory(SpawnDataCatName);

				const FText ShowHideAllItemsText=LOCTEXT("ShowHideAllSpawnDataText","Show/Hide All SpawnData");
				const FText SlatePerformanceWarningText=LOCTEXT("SlatePerformanceWarningText","Slate Performance Warning");
				const FText TooltipText=LOCTEXT("ShowHideAllSpawnDataTooltip","Displays the SpawnActors SpawnData in the details. Can be very slow when there are lots of items.");
				FDetailWidgetRow& CustomRow=spawnDataCategory.AddCustomRow(ShowHideAllItemsText)
					.NameContent()
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
						.VAlign(VAlign_Center)
						.HAlign(HAlign_Left)
						.AutoWidth()
						.Padding(0.0f, 0.0f, 4.0f, 0.0f)
						[
							SNew(SImage)
							.Image(FCoreStyle::Get().GetBrush("Icons.Warning"))
							.ToolTipText(TooltipText)
						]
						+ SHorizontalBox::Slot()
						.VAlign(VAlign_Center)
						.HAlign(HAlign_Left)
						.AutoWidth()
						[
							SNew(STextBlock)
							.Font(IDetailLayoutBuilder::GetDetailFontItalic())
							.Text(SlatePerformanceWarningText)
							.ToolTipText(TooltipText)
						]
					]
				.ValueContent()
					.MaxDesiredWidth(120.f)
					[
						SNew(SButton)
						.OnClicked(this,&FSpawnActorDetailsCustomization::OnShowHideAllItemsClicked)
						.ToolTipText(TooltipText)
						.IsEnabled(this,&FSpawnActorDetailsCustomization::IsShowHideAllItemsEnabled)
						[
							SNew(STextBlock)
							.Font(IDetailLayoutBuilder::GetDetailFontItalic())
							.Text(this,&FSpawnActorDetailsCustomization::GetShowHideAllItemsText)
						]
					];
			}
		}

	}
}

FReply FSpawnActorDetailsCustomization::OnShowHideAllItemsClicked() {

	bForceShowSpawnData=!bForceShowSpawnData;

	// pass the new value the component, as the following refresh will instanciate a new FInstancedStaticMeshComponentDetails : 
	if(spawnActor.IsValid()) {
		spawnActor->bForceShowAllItemsDetails=bForceShowSpawnData;
	}

	// Here we can only take the ptr as ForceRefreshDetails() checks that the reference is unique.
	if(IDetailLayoutBuilder* DetailBuilder=CachedDetailBuilder.Pin().Get()) {
		DetailBuilder->ForceRefreshDetails();
	}
	return FReply::Handled();
}

bool FSpawnActorDetailsCustomization::IsShowHideAllItemsEnabled() const {
	return (numItems>0);
}

FText FSpawnActorDetailsCustomization::GetShowHideAllItemsText() const {
	return bForceShowSpawnData?FText::Format(LOCTEXT("HideAllItems","Hide All {0} Items"),numItems):FText::Format(LOCTEXT("ShowAllItems","Show All {0} IItems"),numItems);
}

//.............................................................................
#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FrdInstEditorModule, rdInstEditor)