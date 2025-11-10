// rdInstEditor.h - Copyright (c) 2022 Recourse Design ltd.
//
// See rdInstBPLibrary.cpp for main Documentation
//
#pragma once
#include "Modules/ModuleManager.h"
#include "ActorFactories/ActorFactory.h"
#include "Factories/Factory.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "AssetRegistry/AssetData.h"
#include "rdInstances.h"
#include "IDetailCustomization.h"
#include "rdInstEditor.generated.h"

#define RDINST_MAJOR_VERSION 1
#define RDINST_MINOR_VERSION 51
#define RDINST_RELEASEDATE "30th June 2025"

#define rdLog(m) UE_LOG(LogTemp,Display,TEXT(m))
#define rdLog1(m,v) UE_LOG(LogTemp,Display,TEXT(m),v)
#define rdLog2(m,v1,v2) UE_LOG(LogTemp,Display,TEXT(m),v1,v2)
#define rdLog3(m,v1,v2,v3) UE_LOG(LogTemp,Display,TEXT(m),v1,v2,v3)

class AActor;
class ArdActor;
class UrdInstData;
class UrdInstOptions;
class FrdInstEditorModule;
class UrdDebugHUD;
class ArdSpawnActor;
class UActorFactory;

#define RDINST_PLUGIN_API DLLEXPORT

UCLASS()
class UrdMultiToken : public UObject {
	GENERATED_BODY()
public:
};
extern UrdMultiToken* rdNonDeterminantInstance;

class FContentBrowserMenuExtension : public TSharedFromThis<FContentBrowserMenuExtension> {
public:
	FContentBrowserMenuExtension(const TArray<FAssetData,FDefaultAllocator> assets);
	void	AddMenuEntry(FMenuBuilder& MenuBuilder);
	void	ShowContentBrowserMenu(FMenuBuilder& MenuBuilder);
	void	ContextMenuItem_InstanceSettings_Selected();
	void	ContextMenuItem_RandomSettings_Selected();
	void	ContextMenuItem_ProxySettings_Selected();
	void	ContextMenuItem_PrefabDataToArrays_Selected();
	void	ContextMenuItem_PrefabDataToTables_Selected();
	void	ContextMenuItem_ConvertLevels_Selected();
	void   ContextMenuItem_UpdateFromLevels_Selected();
	FrdInstEditorModule* rdInstEditorMod=nullptr;
private:
	TArray<FAssetData>	selectedBPs;
	TArray<FAssetData>	selectedStaticMeshes;
	TArray<FAssetData>	selectedLevels;
};

class FSpawnActorDetailsCustomization : public IDetailCustomization {
public:
	static TSharedRef<IDetailCustomization> MakeInstance();
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailLayout) override;
	virtual void CustomizeDetails(const TSharedPtr<IDetailLayoutBuilder>& DetailBuilder) override;
private:
	FReply OnShowHideAllItemsClicked();
	bool IsShowHideAllItemsEnabled() const;
	FText GetShowHideAllItemsText() const;

	bool bForceShowSpawnData=false;
	TWeakObjectPtr<ArdSpawnActor> spawnActor=nullptr;
	int32 numItems=0;
	TWeakPtr<IDetailLayoutBuilder> CachedDetailBuilder;
};

class RDINST_PLUGIN_API FrdInstEditorModule : public IModuleInterface {
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	static void	rdTick(float deltaSeconds);
	static void	OnActorRemoved(AActor* InActor);
	static void	rdOnWorldAdded(UWorld* world);
	static void	rdOnLevelActorAdded(AActor* actor);
	static void	rdOnWorldDestroyed(UWorld* world);
	static void OnActorSelectionChanged(const TArray<UObject*>& selection,bool bForceRefresh);
#if ENGINE_MAJOR_VERSION<5
	static void OnPreSaveWorld(uint32 SaveFlags,UWorld* World);
#else
	static void OnPreSaveWorld(UWorld* InWorld,FObjectPreSaveContext ObjectSaveContext);
#endif

#if ENGINE_MAJOR_VERSION>4
	static void OnElementSelectionChanged(const UTypedElementSelectionSet* SelectionSet,bool bForceRefresh=false);
#endif
	static void rdSelectionChanged();


	void ShowMainSettings();
	void ShowAboutWindow();
	bool ShowAboutSettings();
	FString ShowDataAssetFilename();

	bool ShowStaticMeshInstanceSettings(bool bulk);
	bool ChangeInstanceSettings();
	void ChangeInstanceSettingsForStaticMesh(UStaticMesh* mesh,FScopedSlowTask* SlowTask);
	void ChangeInstanceSettingsForSelectedFiles();

	bool ShowProxySettings();
	void ChangeProxySettingsForStaticMesh(UStaticMesh* mesh,FScopedSlowTask* SlowTask);
	void ChangeProxySettingsForSelectedFiles();

	void SetInstanceDataFromTag(UStaticMesh* mesh);
	void MergeInstanceDataFromTag(UStaticMesh* mesh);
	UrdInstData* GetInstUserData(const UStaticMesh* mesh,bool make);
	void SetInstUserData(const UStaticMesh* mesh,UrdInstData* instData);

	void RandomSubMenu(UToolMenu* menu);
	void ShowRandomSettings(bool bulk);
	void ChangeRandomActorSettings();
	void ChangeRandomSettingsForActor(AActor* actor);
	void ChangeRandomSettingsForSelectedActors();
	void ChangeRandomSettings();
	void ChangeRandomSettingsForSelectedAssets();
	void ChangeRelyOnActor(AActor* actor1,AActor* actor2);
	void ChangeRelyOnActorsForSelectedActors();
	void RemoveRandomSettingsForActor(AActor* actor);
	void RemoveRandomSettingsForSelectedActors();

	void ConvertSelectedAssetPrefabsToArrays();
	void ConvertSelectedAssetPrefabsToTables();
	void ConvertSelectedActorPrefabsToArrays();
	void ConvertSelectedActorPrefabsToTables();

	void fillProxySetupFromAssetData(UStaticMesh* mesh,FrdProxySetup& proxy);
	void ProxySettingsFromSelectedAssets();
	void ChangeProxySettingsForAssets();
	void ChangeProxySettingsForActors(TArray<AActor*>& selectedActors);
	void setProxySetupFromSettings(FrdProxySetup& proxy);
	void setSettingsFromProxySetup(FrdProxySetup& proxy);
	void MergeProxySetupData(FrdProxySetup& proxy);

	void SetRandomFromTags(TArray<FName>& tags);
	bool SetRandomFromTags(TArray<FName>& tags,FrdRandomSettings* options);
	void MergeRandomFromTags(TArray<FName>& tags);
	void CreateTagsFromRandom(TArray<FName>& tags,const FrdRandomSettings& random);
	void RemoveTagsThatStartWith(TArray<FName>& tags,const FString& tagStart);
	void RemoveRandomizeTags(TArray<FName>& tags);
	void SetValidFieldsForTags(TArray<FName>& tags);
	FTransform GetTransformFromTags(TArray<FName>& tags,bool& exists);

	bool ShowConvertToSpawnActorSettings();
	bool ShowUpdateSpawnActorSettings();
	void CreateSpawnActorFromSelection();
	void UpdateSpawnActorFromSelection();
	void CopySpawnActorsToLevel();
	void ProxySettingsFromSelection();
	void CreateSpawnActorsFromSelectedLevelAssets();
	void UpdateSpawnActorsFromSelectedLevelAssets();

	bool SetOptionDefaults(const FString& name,UrdInstOptions* Options);
	bool FillFromOptionDefaults(const FString& name,UrdInstOptions* Options);

	UPackage* rdGetPackage(const FString name);

	TSharedRef<FExtender>					rdMenuExtender(const TArray<FAssetData>& assets);
	void									ToolMenuSubMenu(UToolMenu* menu);
	void									LevelMenuSubMenu(UToolMenu* menu);

	TArray<FAssetData>						selectedAssets;
	TArray<AActor*>							selectedLevelActors;
	TObjectPtr<AActor>						selectedLevelActor=nullptr;
	TSharedPtr<FContentBrowserMenuExtension> rdMenuExtension;

	UrdInstOptions*							rdInstOptions=nullptr;
	FString									configIni;

	static TArray<TObjectPtr<UObject>>		selection;
	static bool								selectionTriggered;
	static bool								wasSelected;
	static bool								hasLevelLoaded;

	static TArray<TObjectPtr<ArdActor>>		selectedActors;
	static TArray<TObjectPtr<ArdActor>>		prefabsWithSelectedISMs;
	static bool								ignoreSelChange;
	static bool								rdSelectNone;
	static TObjectPtr<AActor>				rdDeselectActor;
	static TObjectPtr<ArdActor>				editInstanceActor;

	static TObjectPtr<UInstancedStaticMeshComponent> lastSelectedISMC;
	static int32				             lastSelectedInstanceIndex;
	static TObjectPtr<AActor>				 lastSelectedPrefab;
};

#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>0
#define GetAppFontStyle FAppStyle::GetFontStyle
#define GetAppStyle FAppStyle::Get
#define GetAppBrush FAppStyle::GetBrush
#else
#define GetAppFontStyle FEditorStyle::GetFontStyle
#define GetAppStyle FEditorStyle::Get
#define GetAppBrush FEditorStyle::GetBrush
#endif

//----------------------------------------------------------------------------------------------------------------
// FPluginStyle - class for the rdInst Editor Icons
//----------------------------------------------------------------------------------------------------------------
class FPluginStyle {
public:
	static void Initialize();
	static void Shutdown();
	static TSharedPtr<class ISlateStyle> Get();
	static FName GetStyleSetName();
	static void ReloadTextures();
private:
	static FString InContent(const FString& RelativePath,const ANSICHAR* Extension);
	static TSharedPtr<class FSlateStyleSet> StyleSet;
};
