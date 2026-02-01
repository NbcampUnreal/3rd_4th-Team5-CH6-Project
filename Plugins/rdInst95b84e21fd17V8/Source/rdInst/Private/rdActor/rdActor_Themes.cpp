//
// rdActor_Themes.cpp
//
// Copyright (c) 2022 Recourse Design ltd. All rights reserved.
//
// Creation Date: 9th September 2023 (moved from rdInstBaseActor.cpp)
// Last Modified: 9th September 2023
//
// More information on the rdInst SourceCode can be found at https://recourse.nz/index.php/rdinst-source-code/
//
#include "rdActor.h"

//.............................................................................
// rdGetTheme
//
// Returns the Current Theme for this Actor, if our Parent is an rdActor then
// it gets the Theme from that (unless overrideTheme is set to True)
//.............................................................................
FString ArdActor::rdGetTheme() {

	if(bOverrideTheme) {
		return currentTheme;
	}
	ArdActor* actor=Cast<ArdActor>(GetParentActor());
	if(actor) {
		return actor->rdGetTheme();
	}
	return currentTheme;
}

//.............................................................................
// rdFindFolderCompFromName
//
// Finds the USceneComponent attached to the RootComponent that is a Folder for ChildActors, finding it from the specified Name
//
//.............................................................................
USceneComponent* ArdActor::rdFindFolderCompFromName(const FString& folder) {

	TArray<USceneComponent*> comps;
	RootComponent->GetChildrenComponents(false,comps);
	for(auto c:comps) {
		if(c->GetName()==folder) return c;
	}

	return nullptr;
}

//.............................................................................
// rdFolderIncludedWithCurrentTheme
//
// Returns True if the passed in Folder (USceneComponent) belongs to the current theme.
//
//.............................................................................
bool ArdActor::rdFolderIncludedWithCurrentTheme(USceneComponent* comp) {

	FString theme=rdGetTheme();
	if(theme.IsEmpty()) return true;
	TArray<FString> themes;
	theme.ParseIntoArray(themes,TEXT(","));
	UrdRandomizeAssetUserData* ftag=rdInstLib::GetRandomizeUserData(comp,false);
	
	if(ftag && !ftag->themes.IsEmpty()) {
		TArray<FString> themeList;
		ftag->themes.ParseIntoArray(themeList,TEXT(","));
		if(themeList.Num()>0) {
			bool found=false;
			for(auto t:themes) {
				if(themeList.Contains(t)) {
					found=true;
					break;
				}
			}
			if(!found) return false;
		}
	}
	return true;
}

//.............................................................................
// rdFolderFromNameIncludedWithCurrentTheme
//
// Returns True if the passed in Folder (USceneComponent) is included with the current theme.
//
//.............................................................................
bool ArdActor::rdFolderFromNameIncludedWithCurrentTheme(const FString& folder) {

	FString theme=rdGetTheme();
	if(theme.IsEmpty()) return true;
	USceneComponent* comp=rdFindFolderCompFromName(folder);
	if(!comp) return false;
	UrdRandomizeAssetUserData* ftag=rdInstLib::GetRandomizeUserData(comp,false);
	TArray<FString> themes;
	theme.ParseIntoArray(themes,TEXT(","));
	
	if(ftag && !ftag->themes.IsEmpty()) {
		TArray<FString> themeList;
		ftag->themes.ParseIntoArray(themeList,TEXT(","));
		if(themeList.Num()>0) {
			bool found=false;
			for(auto t:themes) {
				if(themeList.Contains(t)) {
					found=true;
					break;
				}
			}
			if(!found) return false;
		}
	}
	return true;
}

//.............................................................................
