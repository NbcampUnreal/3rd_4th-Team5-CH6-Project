// rdSplines.h - Copyright (c) 2022 Recourse Design ltd.
//
// See rdInstBPLibrary.cpp for main Documentation
//
#pragma once

#include "rdInstBPLibrary.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
//#include "ProceduralMeshComponent.h"
#include "rdSplines.generated.h"

#define RDINST_PLUGIN_API DLLEXPORT

USTRUCT(BlueprintType,meta=(HasNativeBreak="/Script/rdInst.rdActor.BreakSplinePopulateData",HasNativeMake="/Script/rdInst.rdActor.MakeSplinePopulateData"))
struct FrdSplinePopulateData {
	GENERATED_BODY()
public:
	FrdSplinePopulateData() {}
	FrdSplinePopulateData(const FrdSplinePopulateData& o): mesh(o.mesh),mat(o.mat),offset(o.offset),gap(o.gap),axis(o.axis),customData(o.customData) {}
	FrdSplinePopulateData(UStaticMesh* m,UMaterialInterface* mt,const FTransform& ofs,float g,const TEnumAsByte<ESplineMeshAxis::Type> ax,const TArray<float>& cd) : mesh(m),mat(mt),offset(ofs),gap(g),axis(ax),customData(cd) {}

	UPROPERTY(Category=rdSplinePopulateData,EditAnywhere)
	UStaticMesh*						mesh=nullptr;

	UPROPERTY(Category=rdSplinePopulateData,EditAnywhere)
	UMaterialInterface*					mat=nullptr;

	UPROPERTY(Category=rdSplinePopulateData,EditAnywhere)
	FTransform							offset=FTransform::Identity;

	UPROPERTY(Category=rdSplinePopulateData,EditAnywhere)
	float								gap=0.0f;

	UPROPERTY(Category=rdSplinePopulateData,EditAnywhere)
	TEnumAsByte<ESplineMeshAxis::Type>	axis=ESplineMeshAxis::X;

	UPROPERTY(Category=rdSplinePopulateData,EditAnywhere)
	TArray<float>						customData;
};

USTRUCT(BlueprintType,meta=(HasNativeBreak="/Script/rdInst.rdActor.BreakSplineInstanceData",HasNativeMake="/Script/rdInst.rdActor.MakeSplineInstanceData"))
struct FrdSplineInstanceData {
	GENERATED_BODY()
public:
	FrdSplineInstanceData() {}
//	FrdSplineInstanceData(const FrdSplineInstanceData& o): startLocation(o.startLocation),startTangent(o.startTangent),startScale(o.startScale),startRoll(o.startRoll),endLocation(o.endLocation),endTangent(o.endTangent),endScale(o.endScale),endRoll(o.endRoll),smc(o.smc),pmc(o.pmc) {}
//	FrdSplineInstanceData(const FVector& sl,const FVector& st,const FVector& ss,float sr,const FVector& el,const FVector& et,const FVector& es,float er,USplineMeshComponent* sm,UProceduralMeshComponent* pm) : startLocation(sl),startTangent(st),startScale(ss),startRoll(sr),endLocation(el),endTangent(et),endScale(es),endRoll(er),smc(sm),pmc(pm) {}
	FrdSplineInstanceData(const FrdSplineInstanceData& o): startLocation(o.startLocation),startTangent(o.startTangent),startScale(o.startScale),startRoll(o.startRoll),endLocation(o.endLocation),endTangent(o.endTangent),endScale(o.endScale),endRoll(o.endRoll),smc(o.smc) {}
	FrdSplineInstanceData(const FVector& sl,const FVector& st,const FVector& ss,float sr,const FVector& el,const FVector& et,const FVector& es,float er,USplineMeshComponent* sm) : startLocation(sl),startTangent(st),startScale(ss),startRoll(sr),endLocation(el),endTangent(et),endScale(es),endRoll(er),smc(sm) {}

	FORCEINLINE bool operator ==(const FrdSplineInstanceData& o) const { return startLocation==o.startLocation && startTangent==o.startTangent && startScale==o.startScale && startRoll==o.startRoll && endLocation==o.endLocation && endTangent==o.endTangent && endScale==o.endScale && endRoll==o.endRoll; }
	FORCEINLINE bool operator !=(const FrdSplineInstanceData& o) const { return startLocation!=o.startLocation || startTangent!=o.startTangent || startScale!=o.startScale || startRoll!=o.startRoll || endLocation!=o.endLocation || endTangent!=o.endTangent || endScale!=o.endScale || endRoll!=o.endRoll; }

	UPROPERTY(Category=FrdSplineInstanceData,VisibleAnywhere,BlueprintReadOnly)
	FVector startLocation=FVector::ZeroVector;

	UPROPERTY(Category=FrdSplineInstanceData,VisibleAnywhere,BlueprintReadOnly)
	FVector startTangent=FVector::ZeroVector;

	UPROPERTY(Category=FrdSplineInstanceData,VisibleAnywhere,BlueprintReadOnly)
	FVector startScale=FVector(1,1,1);

	UPROPERTY(Category=FrdSplineInstanceData,VisibleAnywhere,BlueprintReadOnly)
	float startRoll=0.0f;

	UPROPERTY(Category=FrdSplineInstanceData,VisibleAnywhere,BlueprintReadOnly)
	FVector endLocation=FVector::ZeroVector;

	UPROPERTY(Category=FrdSplineInstanceData,VisibleAnywhere,BlueprintReadOnly)
	FVector endTangent=FVector::ZeroVector;

	UPROPERTY(Category=FrdSplineInstanceData,VisibleAnywhere,BlueprintReadOnly)
	FVector endScale=FVector(1,1,1);

	UPROPERTY(Category=FrdSplineInstanceData,VisibleAnywhere,BlueprintReadOnly)
	float endRoll=0.0f;

	UPROPERTY(Category=FrdSplineInstanceData,VisibleAnywhere,BlueprintReadOnly)
	USplineMeshComponent*				smc=nullptr;

//	UPROPERTY(Category=FrdSplineInstanceData,VisibleAnywhere,BlueprintReadOnly)
//	UProceduralMeshComponent*			pmc=nullptr;
};
