// rdActorStringss.h - Copyright (c) 2022 Recourse Design ltd.
//
// See rdInstBPLibrary.cpp for main Documentation
//
#pragma once

#include "CoreMinimal.h"

class AActor;
class ALight;
class ADecalActor;
class ALightActor;
class ASpotLight;
class ARectLight;
class APointLight;
class ALocalFogVolume;
class APostProcessVolume;
class ANiagaraActor;
class ATextRenderActor;

#define RDSTRING_VERSION 1
#define RDINST_PLUGIN_API DLLEXPORT

class FrdString : public FString {
public:

	static FString UpToFirst(const FString& s,char c) { int32 i; if(s.FindChar(c,i)) { return s.Left(i); } return FString(); }
	static FString AfterFirst(const FString& s,char c) { int32 i; if(s.FindChar(c,i)) { return s.RightChop(i+1); } return FString(); }
	static FString UpToLast(const FString& s,char c) { int32 i; if(s.FindLastChar(c,i)) {return s.Left(i); } return FString(); }
	static FString AfterLast(const FString& s,char c) { int32 i; if(s.FindLastChar(c,i)) { return s.RightChop(i+1); } return FString(); }

	static FString ToString(const ADecalActor* a);
	static int32 FromString(ADecalActor* a,const FString& str,int32 ver);
	static FString ToString(const ALocalFogVolume* a);
	static int32 FromString(ALocalFogVolume* a,const FString& str,int32 ver);
	static FString ToString(const ALight* a);
	static int32 FromString(ALight* a,const FString& str,int32 ver);
	static FString ToString(const ASpotLight* a);
	static int32 FromString(ASpotLight* a,const FString& str,int32 ver);
	static FString ToString(const ARectLight* a);
	static int32 FromString(ARectLight* a,const FString& str,int32 ver);
	static FString ToString(const APointLight* a);
	static int32 FromString(APointLight* a,const FString& str,int32 ver);
	static FString ToString(const APostProcessVolume* a);
	static int32 FromString(APostProcessVolume* a,const FString& str,int32 ver);
	static FString ToString(const ANiagaraActor* a);
	static int32 FromString(ANiagaraActor* a,const FString& str,int32 ver);
	static FString ToString(const ATextRenderActor* a);
	static int32 FromString(ATextRenderActor* a,const FString& str,int32 ver);
	static FString ToString(const AActor* a);
	static int32 FromString(AActor* a,const FString& str,int32 ver);
};
