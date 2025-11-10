//
// rdActorStrings.cpp
//
// Copyright (c) 2022 Recourse Design ltd. All rights reserved.
//
// Creation Date: 3rd December 2024
// Last Modified: 29th June 2025
//
#include "rdActorStrings.h"
#include "rdInstBaseActor.h"
#include "rdActor.h"
#include "Engine/Texture.h"
#include "UObject/SoftObjectPtr.h"
#include "NiagaraActor.h"
#include "Engine/PostProcessVolume.h"
#include "Engine/DecalActor.h"
#include "Engine/SpotLight.h"
#include "Engine/RectLight.h"
#include "Engine/PointLight.h"
#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>3
#include "Engine/LocalFogVolume.h"
#include "Components/LocalFogVolumeComponent.h"
#endif
#include "Components/DecalComponent.h"
#include "Components/LightComponent.h"
#include "Components/SpotLightComponent.h"
#include "Components/RectLightComponent.h"
#include "Components/PointLightComponent.h"
#include "Engine/TextRenderActor.h"
#include "Components/TextRenderComponent.h"
#include "Engine/Font.h"

//.............................................................................
// Decal ToString
//.............................................................................
FString FrdString::ToString(const ADecalActor* a) {
	UDecalComponent* dc=Cast<UDecalComponent>(a->FindComponentByClass(UDecalComponent::StaticClass()));;
	TSoftObjectPtr<UMaterialInterface> mat=dc->GetMaterial(0);
	return ToString((AActor*)a)+FString::Printf(TEXT("|%s|%d|%s|%s|%s|%s|%s|%d|%s|%s|%s|%s|%s|%s|%s"),
							*ArdInstBaseActor::rdPackMaterialName(mat.ToString()),//UMaterial
							dc->SortOrder,//int32
							*FString::SanitizeFloat(dc->FadeScreenSize),//float
							*FString::SanitizeFloat(dc->FadeStartDelay),//float
							*FString::SanitizeFloat(dc->FadeDuration),//float
							*FString::SanitizeFloat(dc->FadeInDuration),//float
							*FString::SanitizeFloat(dc->FadeInStartDelay),//float
							dc->bDestroyOwnerAfterFade,//bool
							*FString::SanitizeFloat(dc->DecalSize.X),//FVector
							*FString::SanitizeFloat(dc->DecalSize.Y),
							*FString::SanitizeFloat(dc->DecalSize.Z),
#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>1
							*FString::SanitizeFloat(dc->DecalColor.R),//FLinearColor
							*FString::SanitizeFloat(dc->DecalColor.G),
							*FString::SanitizeFloat(dc->DecalColor.B),
							*FString::SanitizeFloat(dc->DecalColor.A)
#else 
							TEXT(""),TEXT(""),TEXT(""),TEXT("")
#endif
						);
}

//.............................................................................
// Decal FromString
//.............................................................................
int32 FrdString::FromString(ADecalActor* a,const FString& str,int32 ver) {

	TArray<FString> vals;
	int32 num=str.ParseIntoArray(vals,TEXT("|"),false);
	if(num<15) {
		return 0;
	}

	UDecalComponent* dc=Cast<UDecalComponent>(a->FindComponentByClass(UDecalComponent::StaticClass()));
	int32 i=FromString((AActor*)a,str,ver);
	TSoftObjectPtr<UMaterialInterface> mat=TSoftObjectPtr<UMaterialInterface>(FSoftObjectPath(ArdInstBaseActor::rdUnpackMaterialName(vals[i++])));

	dc->SetMaterial(0,mat.LoadSynchronous());
	dc->SortOrder=FCString::Atoi(*vals[i++]);
	dc->FadeScreenSize=FCString::Atof(*vals[i++]);
	dc->FadeStartDelay=FCString::Atof(*vals[i++]);
	dc->FadeDuration=FCString::Atof(*vals[i++]);
	dc->FadeInDuration=FCString::Atof(*vals[i++]);
	dc->FadeInStartDelay=FCString::Atof(*vals[i++]);
	dc->bDestroyOwnerAfterFade=vals[i++].ToBool();
	dc->DecalSize.X=FCString::Atof(*vals[i++]);
	dc->DecalSize.Y=FCString::Atof(*vals[i++]);
	dc->DecalSize.Z=FCString::Atof(*vals[i++]);
#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>1
	dc->DecalColor.R=FCString::Atof(*vals[i++]);
	dc->DecalColor.G=FCString::Atof(*vals[i++]);
	dc->DecalColor.B=FCString::Atof(*vals[i++]);
	dc->DecalColor.A=FCString::Atof(*vals[i++]);
#else
	i+=4;
#endif

	return i;
}

//.............................................................................
// LocalFogVolume ToString
//.............................................................................
#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>3
FString FrdString::ToString(const ALocalFogVolume* a) {
	ULocalFogVolumeComponent* fc=a->GetComponent();
	return ToString((AActor*)a)+FString::Printf(TEXT("|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s"),
							*FString::SanitizeFloat(fc->RadialFogExtinction),//float
							*FString::SanitizeFloat(fc->HeightFogExtinction),//float
							*FString::SanitizeFloat(fc->HeightFogFalloff),//float
							*FString::SanitizeFloat(fc->HeightFogOffset),//float
							*FString::SanitizeFloat(fc->FogPhaseG),//float
							*FString::SanitizeFloat(fc->FogAlbedo.R),//FLinearColor
							*FString::SanitizeFloat(fc->FogAlbedo.G),
							*FString::SanitizeFloat(fc->FogAlbedo.B),
							*FString::SanitizeFloat(fc->FogAlbedo.A),
							*FString::SanitizeFloat(fc->FogEmissive.R),//FLinearColor
							*FString::SanitizeFloat(fc->FogEmissive.G),
							*FString::SanitizeFloat(fc->FogEmissive.B),
							*FString::SanitizeFloat(fc->FogEmissive.A),
							*FString::SanitizeFloat(fc->FogSortPriority)//int32
						);
}

//.............................................................................
// LocalFogVolume FromString
//.............................................................................
int32 FrdString::FromString(ALocalFogVolume* a,const FString& str,int32 ver) {

	TArray<FString> vals;
	int32 num=str.ParseIntoArray(vals,TEXT("|"),false);
	if(num<14) {
		return 0;
	}

	ULocalFogVolumeComponent* fc=a->GetComponent();
	int32 i=FromString((AActor*)a,str,ver);

	fc->RadialFogExtinction=FCString::Atof(*vals[i++]);
	fc->HeightFogExtinction=FCString::Atof(*vals[i++]);
	fc->HeightFogFalloff=FCString::Atof(*vals[i++]);
	fc->HeightFogOffset=FCString::Atof(*vals[i++]);
	fc->FogPhaseG=FCString::Atof(*vals[i++]);
	fc->FogAlbedo.R=FCString::Atof(*vals[i++]);
	fc->FogAlbedo.G=FCString::Atof(*vals[i++]);
	fc->FogAlbedo.B=FCString::Atof(*vals[i++]);
	fc->FogAlbedo.A=FCString::Atof(*vals[i++]);
	fc->FogEmissive.R=FCString::Atof(*vals[i++]);
	fc->FogEmissive.G=FCString::Atof(*vals[i++]);
	fc->FogEmissive.B=FCString::Atof(*vals[i++]);
	fc->FogEmissive.A=FCString::Atof(*vals[i++]);
	fc->FogSortPriority=FCString::Atoi(*vals[i++]);

	return i;
}
#endif

//.............................................................................
// ALight ToString
//.............................................................................
FString FrdString::ToString(const ALight* a) {
	ULightComponent* lc=a->GetLightComponent();
	FLightingChannels& chan=lc->LightingChannels;
	int32 chanI=(chan.bChannel0?0x1:0)+(chan.bChannel1?0x2:0)+(chan.bChannel2?0x4:0);
	return  ToString((AActor*)a)+FString::Printf(TEXT("|%d|%s|%s|%s|%s|%s|%d|%s|%d|%s|%s|%s|%s|%s|%s|%s|%s|%d|%s|%s|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%s"),
							a->bEnabled,//bool
							*FString::SanitizeFloat(lc->Intensity),//float
							*FString::SanitizeFloat(lc->LightColor.R),//FLinearColor
							*FString::SanitizeFloat(lc->LightColor.G),
							*FString::SanitizeFloat(lc->LightColor.B),
							*FString::SanitizeFloat(lc->LightColor.A),
							lc->bUseTemperature,//bool
							*FString::SanitizeFloat(lc->Temperature),//float
							lc->bAffectsWorld,//bool
							*FString::SanitizeFloat(lc->IndirectLightingIntensity),//float
							*FString::SanitizeFloat(lc->VolumetricScatteringIntensity),//float
							*FString::SanitizeFloat(lc->SpecularScale),//float
#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>4
							*FString::SanitizeFloat(lc->DiffuseScale),//float
#else
							TEXT(""),
#endif
							*FString::SanitizeFloat(lc->ShadowResolutionScale),//float
							*FString::SanitizeFloat(lc->ShadowBias),//float
							*FString::SanitizeFloat(lc->ShadowSlopeBias),//float
							*FString::SanitizeFloat(lc->ContactShadowLength),//float
							lc->ContactShadowLengthInWS,//bool
#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>1
							*FString::SanitizeFloat(lc->ContactShadowCastingIntensity),//float
							*FString::SanitizeFloat(lc->ContactShadowNonCastingIntensity),//float
#else
							TEXT(""),TEXT(""),
#endif
							lc->CastTranslucentShadows,//bool
							lc->bForceCachedShadowsForMovablePrimitives,//bool
#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>4
							lc->bAllowMegaLights,//bool
							(int32)lc->MegaLightsShadowMethod,//enum
#else
							0,0,
#endif
							chanI,//3tick
							lc->CastStaticShadows,//bool
							lc->CastDynamicShadows,//bool
							lc->bAffectTranslucentLighting,//bool
							lc->bTransmission,//bool
							lc->bCastVolumetricShadow,//bool
							lc->bCastDeepShadow,//bool
							*FString::SanitizeFloat(lc->DeepShadowLayerDistribution)//float
//								a->bCastShadows,//bool
//								lc->ShadowFilterSharpen,//float
//								(int32)lc->bCastRayTracedShadows,//enum
//								lc->bAffectRayTracingReflections,//bool
//								lc->bAffectRayTracingGlobalIllumination,//bool
						);
}

//.............................................................................
// ALight FromString
//.............................................................................
int32 FrdString::FromString(ALight* a,const FString& str,int32 ver) {

	TArray<FString> vals;
	int32 num=str.ParseIntoArray(vals,TEXT("|"),false);
	if(num<32) {
		return 0;
	}

	ULightComponent* lc=a->GetLightComponent();
	int32 i=FromString((AActor*)a,str,ver);

	a->bEnabled=vals[i++].ToBool();
	lc->Intensity=FCString::Atof(*vals[i++]);
	lc->LightColor.R=FCString::Atof(*vals[i++]);
	lc->LightColor.G=FCString::Atof(*vals[i++]);
	lc->LightColor.B=FCString::Atof(*vals[i++]);
	lc->LightColor.A=FCString::Atof(*vals[i++]);
	lc->bUseTemperature=vals[i++].ToBool();
	lc->Temperature=FCString::Atof(*vals[i++]);
	lc->bAffectsWorld=vals[i++].ToBool();
	lc->IndirectLightingIntensity=FCString::Atof(*vals[i++]);
	lc->VolumetricScatteringIntensity=FCString::Atof(*vals[i++]);
	lc->SpecularScale=FCString::Atof(*vals[i++]);
#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>4
	lc->DiffuseScale=FCString::Atof(*vals[i++]);
#else
	i++;
#endif
	lc->ShadowResolutionScale=FCString::Atof(*vals[i++]);
	lc->ShadowBias=FCString::Atof(*vals[i++]);
	lc->ShadowSlopeBias=FCString::Atof(*vals[i++]);
	lc->ContactShadowLength=FCString::Atof(*vals[i++]);
	lc->ContactShadowLengthInWS=vals[i++].ToBool();
#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>1
	lc->ContactShadowCastingIntensity=FCString::Atof(*vals[i++]);
	lc->ContactShadowNonCastingIntensity=FCString::Atof(*vals[i++]);
#else
	i+=2;
#endif
	lc->CastTranslucentShadows=vals[i++].ToBool();
	lc->bForceCachedShadowsForMovablePrimitives=vals[i++].ToBool();
#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>4
	lc->bAllowMegaLights=vals[i++].ToBool();
	lc->MegaLightsShadowMethod=(TEnumAsByte<EMegaLightsShadowMethod::Type>)FCString::Atoi(*vals[i++]);
#else
	i+=2;
#endif

	int32 ci=FCString::Atoi(*vals[i++]);
	FLightingChannels& chan=lc->LightingChannels;
	chan.bChannel0=!!(ci&0x1);
	chan.bChannel1=!!(ci&0x2);
	chan.bChannel2=!!(ci&0x4);
	lc->CastStaticShadows=vals[i++].ToBool();
	lc->CastDynamicShadows=vals[i++].ToBool();
	lc->bAffectTranslucentLighting=vals[i++].ToBool();
	lc->bTransmission=vals[i++].ToBool();
	lc->bCastVolumetricShadow=vals[i++].ToBool();
	lc->bCastDeepShadow=vals[i++].ToBool();
	lc->DeepShadowLayerDistribution=FCString::Atof(*vals[i++]);

	return i;
}

//.............................................................................
// SpotLight ToString
//.............................................................................
FString FrdString::ToString(const ASpotLight* a) {
	USpotLightComponent* lc=a->SpotLightComponent;
	return ToString((ALight*)a)+FString::Printf(TEXT("|%s|%s|%s|%s|%s|%d|%s|%s|%d|%s"),
							*FString::SanitizeFloat(lc->InnerConeAngle),//float
							*FString::SanitizeFloat(lc->OuterConeAngle),//float
							*FString::SanitizeFloat(lc->SourceRadius),//float
							*FString::SanitizeFloat(lc->SoftSourceRadius),//float
							*FString::SanitizeFloat(lc->SourceLength),//float
							lc->bUseInverseSquaredFalloff,//bool
							*FString::SanitizeFloat(lc->LightFalloffExponent),//float
							*FString::SanitizeFloat(lc->AttenuationRadius),//float
							(int32)lc->IntensityUnits,//enum
#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>0
							*FString::SanitizeFloat(lc->InverseExposureBlend)//float
#else
							TEXT("")
#endif
						);
}

//.............................................................................
// SpotLight FromString
//.............................................................................
int32 FrdString::FromString(ASpotLight* a,const FString& str,int32 ver) {

	TArray<FString> vals;
	int32 num=str.ParseIntoArray(vals,TEXT("|"),false);
	if(num<9) {
		return 0;
	}

	USpotLightComponent* lc=a->SpotLightComponent;
	int32 i=FromString((ALight*)a,str,ver);

	lc->InnerConeAngle=FCString::Atof(*vals[i++]);
	lc->OuterConeAngle=FCString::Atof(*vals[i++]);
	lc->SourceRadius=FCString::Atof(*vals[i++]);
	lc->SoftSourceRadius=FCString::Atof(*vals[i++]);
	lc->SourceLength=FCString::Atof(*vals[i++]);
	lc->bUseInverseSquaredFalloff=vals[i++].ToBool();
	lc->LightFalloffExponent=FCString::Atof(*vals[i++]);
	lc->AttenuationRadius=FCString::Atof(*vals[i++]);
	lc->IntensityUnits=(ELightUnits)FCString::Atoi(*vals[i++]);
#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>0
	lc->InverseExposureBlend=FCString::Atof(*vals[i++]);
#else
	i++;
#endif

	return i;
}

//.............................................................................
// RectLight ToString
//.............................................................................
FString FrdString::ToString(const ARectLight* a) {
	URectLightComponent* lc=a->RectLightComponent;
	TSoftObjectPtr<UTexture> tex=TSoftObjectPtr<UTexture>(lc->SourceTexture);
	return ToString((ALight*)a)+FString::Printf(TEXT("|%s|%s|%s|%s|%s|%s|%d|%s"),
							*FString::SanitizeFloat(lc->SourceWidth),//float
							*FString::SanitizeFloat(lc->SourceHeight),//float
							*FString::SanitizeFloat(lc->BarnDoorAngle),//float
							*FString::SanitizeFloat(lc->BarnDoorLength),//float
							*ArdInstBaseActor::rdPackMaterialName(tex.ToString()),//UTexture
							*FString::SanitizeFloat(lc->AttenuationRadius),//float
							(int32)lc->IntensityUnits,//enum
#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>0
							*FString::SanitizeFloat(lc->InverseExposureBlend)//float
#else
							TEXT("")
#endif
						);
}

//.............................................................................
// RectLight FromString
//.............................................................................
int32 FrdString::FromString(ARectLight* a,const FString& str,int32 ver) {

	TArray<FString> vals;
	int32 num=str.ParseIntoArray(vals,TEXT("|"),false);
	if(num<8) {
		return 0;
	}

	URectLightComponent* lc=a->RectLightComponent;
	int32 i=FromString((ALight*)a,str,ver);

	lc->SourceWidth=FCString::Atof(*vals[i++]);
	lc->SourceHeight=FCString::Atof(*vals[i++]);
	lc->BarnDoorAngle=FCString::Atof(*vals[i++]);
	lc->BarnDoorLength=FCString::Atof(*vals[i++]);
	TSoftObjectPtr<UTexture> tex=TSoftObjectPtr<UTexture>(FSoftObjectPath(*ArdInstBaseActor::rdUnpackMaterialName(vals[i++])));
	lc->SourceTexture=tex.LoadSynchronous();
	lc->AttenuationRadius=FCString::Atof(*vals[i++]);
	lc->IntensityUnits=(ELightUnits)FCString::Atoi(*vals[i++]);
#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>0
	lc->InverseExposureBlend=FCString::Atof(*vals[i++]);
#else
	i++;
#endif

	return i;
}

//.............................................................................
// PointLight ToString
//.............................................................................
FString FrdString::ToString(const APointLight* a) {
	UPointLightComponent* lc=a->PointLightComponent;
	return ToString((ALight*)a)+FString::Printf(TEXT("|%s|%s|%d|%s|%s|%d|%s"),
							*FString::SanitizeFloat(lc->SourceRadius),//float
							*FString::SanitizeFloat(lc->SourceLength),//float
							lc->bUseInverseSquaredFalloff,//bool
							*FString::SanitizeFloat(lc->LightFalloffExponent),//float
							*FString::SanitizeFloat(lc->AttenuationRadius),//float
							(int32)lc->IntensityUnits,//enum
#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>0
							*FString::SanitizeFloat(lc->InverseExposureBlend)//float
#else
							TEXT("")
#endif
						);
}

//.............................................................................
// PointLight FromString
//.............................................................................
int32 FrdString::FromString(APointLight* a,const FString& str,int32 ver) {

	TArray<FString> vals;
	int32 num=str.ParseIntoArray(vals,TEXT("|"),false);
	if(num<7) {
		return 0;
	}

	UPointLightComponent* lc=a->PointLightComponent;
	int32 i=FromString((ALight*)a,str,ver);

	lc->SourceRadius=FCString::Atof(*vals[i++]);
	lc->SourceLength=FCString::Atof(*vals[i++]);
	lc->bUseInverseSquaredFalloff=vals[i++].ToBool();
	lc->LightFalloffExponent=FCString::Atof(*vals[i++]);
	lc->AttenuationRadius=FCString::Atof(*vals[i++]);
	lc->IntensityUnits=(ELightUnits)FCString::Atoi(*vals[i++]);
#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>0
	lc->InverseExposureBlend=FCString::Atof(*vals[i++]);
#else
	i++;
#endif

	return i;
}

//.............................................................................
// PostProcess ToString
//.............................................................................
FString FrdString::ToString(const APostProcessVolume* a) {
//	FPostProcessSettings& pp=a->Settings;
	return ToString((AActor*)a);//+TEXT("|")+FString::Printf(TEXT(""),	);
}

//.............................................................................
// PostProcess FromString
//.............................................................................
int32 FrdString::FromString(APostProcessVolume* a,const FString& str,int32 ver) {
	TArray<FString> vals;
	int32 num=str.ParseIntoArray(vals,TEXT("|"),false);
	if(num<1) {
		return 0;
	}
	int32 i=FromString((AActor*)a,str,ver);

	return i;
}

//.............................................................................
// Niagara ToString
//.............................................................................
FString FrdString::ToString(const ANiagaraActor* a) {
//	UNiagaraComponent* c=a->GetNiagaraComponent();
	return ToString((AActor*)a);//+TEXT("|")+FString::Printf(TEXT(""),c->,);
}

//.............................................................................
// Niagara FromString
//.............................................................................
int32 FrdString::FromString(ANiagaraActor* a,const FString& str,int32 ver) {
	TArray<FString> vals;
	int32 num=str.ParseIntoArray(vals,TEXT("|"),false);
	if(num<1) {
		return 0;
	}
	int32 i=0;

	return i;
}

//.............................................................................
// TextRender ToString
//.............................................................................
FString FrdString::ToString(const ATextRenderActor* a) {

	UTextRenderComponent* c=a->GetTextRender();
	if(!c) return TEXT("");

	TSoftObjectPtr<UFont> sfont=c->Font?TSoftObjectPtr<UFont>(FSoftObjectPath(c->Font->GetPathName())):nullptr;
	TSoftObjectPtr<UMaterialInterface> smat=c->TextMaterial?TSoftObjectPtr<UMaterialInterface>(FSoftObjectPath(c->TextMaterial->GetPathName())):nullptr;

	return ToString((AActor*)a)+FString::Printf(TEXT("|%s|%d|%s|%s|%s|%s|%d|%d|%d|%d|%d|%s|%s|%s|%s"),
												*sfont.ToString(), // TObjectPtr<UFont>
												(int32)c->HorizontalAlignment, // TEnumAsByte<EHorizTextAligment>
												*FString::SanitizeFloat(c->HorizSpacingAdjust), // float
												*FString::SanitizeFloat(c->InvDefaultSize), // float
												*c->Text.ToString(), // FText
												*ArdInstBaseActor::rdPackMaterialName(smat.ToString()), // TObjectPtr<UMaterialInterface>
												c->TextRenderColor.R,c->TextRenderColor.G,c->TextRenderColor.B,c->TextRenderColor.A, // FColor
												(int32)c->VerticalAlignment, // TEnumAsByte<EVerticalTextAligment>
												*FString::SanitizeFloat(c->VertSpacingAdjust), // float
												*FString::SanitizeFloat(c->WorldSize), // float
												*FString::SanitizeFloat(c->XScale), // float
												*FString::SanitizeFloat(c->YScale) // float
											);
}

//.............................................................................
// TextRender FromString
//.............................................................................
int32 FrdString::FromString(ATextRenderActor* a,const FString& str,int32 ver) {

	UTextRenderComponent* c=a->GetTextRender();
	if(!c) return 0;

	TArray<FString> vals;
	int32 num=str.ParseIntoArray(vals,TEXT("|"),false);
	if(num<15) {
		return 0;
	}
	int32 i=FromString((AActor*)a,str,ver);

	TSoftObjectPtr<UFont> font=TSoftObjectPtr<UFont>(FSoftObjectPath(*ArdInstBaseActor::rdUnpackMaterialName(vals[i++])));
	c->Font=font.LoadSynchronous();
	c->HorizontalAlignment=(TEnumAsByte<EHorizTextAligment>)FCString::Atoi(*vals[i++]);
	c->HorizSpacingAdjust=FCString::Atof(*vals[i++]);
	c->InvDefaultSize=FCString::Atof(*vals[i++]);
	FText txt=FText::FromString(vals[i++]);
	TSoftObjectPtr<UMaterialInterface> mat=TSoftObjectPtr<UMaterialInterface>(FSoftObjectPath(*ArdInstBaseActor::rdUnpackMaterialName(vals[i++])));
	c->TextMaterial=mat.LoadSynchronous();
	c->TextRenderColor.R=FCString::Atoi(*vals[i++]);
	c->TextRenderColor.G=FCString::Atoi(*vals[i++]);
	c->TextRenderColor.B=FCString::Atoi(*vals[i++]);
	c->TextRenderColor.A=FCString::Atoi(*vals[i++]);
	c->VerticalAlignment=(TEnumAsByte<EVerticalTextAligment>)FCString::Atoi(*vals[i++]);
	c->VertSpacingAdjust=FCString::Atof(*vals[i++]);
	c->WorldSize=FCString::Atof(*vals[i++]);
	c->XScale=FCString::Atof(*vals[i++]);
	c->YScale=FCString::Atof(*vals[i++]);
	c->SetText(txt);

	return i;
}

//.............................................................................
// AActor ToString
//.............................................................................
FString FrdString::ToString(const AActor* a) {

	USceneComponent* comp=a->GetRootComponent();
	//int32 mob=comp?comp->Mobility:0;

	return FString::Printf(TEXT("%d|%d"),a->IsHidden(),a->GetIsReplicated());
//	return FString::Printf(TEXT("%d|%d|%d"),a->IsHidden(),a->GetIsReplicated(),mob);
}

//.............................................................................
// AActor FromString
//.............................................................................
int32 FrdString::FromString(AActor* a,const FString& str,int32 ver) {
	TArray<FString> vals;
	int32 num=str.ParseIntoArray(vals,TEXT("|"),false);
	if(num<2) {
		return 0;
	}
	int32 i=0;

	a->SetActorHiddenInGame(vals[i++].ToBool());
	a->SetReplicates(vals[i++].ToBool());
	//EComponentMobility::Type mob=(EComponentMobility::Type)FCString::Atoi(*vals[i++]);
	USceneComponent* comp=a->GetRootComponent();
	if(comp) {
		comp->SetMobility(EComponentMobility::Movable);//mob); (they get moved around by the spawn actor etc) 
	}
	return i;
}

//.............................................................................
// rdApplyStringProps
//.............................................................................
void ArdInstBaseActor::rdApplyStringProps(AActor* actor,const FString& str,int32 ver) {
	
	UClass* actorClass=actor->GetClass();
	int32 i=0;
	if(actorClass==ADecalActor::StaticClass()) { i=FrdString::FromString((ADecalActor*)actor,str,ver); }
#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>3
	else if(actorClass==ALocalFogVolume::StaticClass()) { i=FrdString::FromString((ALocalFogVolume*)actor,str,ver); }
#endif
	else if(actorClass==ASpotLight::StaticClass()) { i=FrdString::FromString((ASpotLight*)actor,str,ver); }
	else if(actorClass==ARectLight::StaticClass()) { i=FrdString::FromString((ARectLight*)actor,str,ver); } 
	else if(actorClass==APointLight::StaticClass()) { i=FrdString::FromString((APointLight*)actor,str,ver); } 
	else if(actorClass==APostProcessVolume::StaticClass()) { i=FrdString::FromString((APostProcessVolume*)actor,str,ver); } 
	else if(actorClass==ANiagaraActor::StaticClass()) { i=FrdString::FromString((ANiagaraActor*)actor,str,ver); } 
	else if(actorClass==ATextRenderActor::StaticClass()) { i=FrdString::FromString((ATextRenderActor*)actor,str,ver); } 
	else { 
		i=FrdString::FromString(actor,str,ver); 

		TArray<FString> vals;
		int32 num=str.ParseIntoArray(vals,TEXT("|"),false);
		if(num>i) {
			UFunction* func=actor->FindFunction(FName(TEXT("rdFromString")));
			if(func) {
				struct cnvParms { FString str; };
				cnvParms parms;
				for(;i<num;i++) {
					parms.str+=vals[i]+TEXT("|");
				}
				actor->ProcessEvent(func,(void*)&parms);
			}
		}
	}
}

//.............................................................................
