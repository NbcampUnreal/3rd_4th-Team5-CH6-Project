// rdGridArray.h - Copyright (c) 2022 Recourse Design ltd.
//
// See rdInstBPLibrary.cpp for main Documentation
//
#pragma once

#include "rdInstBPLibrary.h"
#include "rdGridArray.generated.h"

#define RDINST_PLUGIN_API DLLEXPORT

class ArdSpawnActor;

#define rdGridFlag_Hidden 1
#define rdGridFlag_Destroyed 2

USTRUCT(BlueprintType)
struct RDINST_PLUGIN_API FrdInstanceTransformItem {
	GENERATED_BODY()
public:
	FrdInstanceTransformItem() {}
	FrdInstanceTransformItem(const FTransform& t) : transform(t) {}
	FrdInstanceTransformItem(const FTransform& t,int32 i) : index(i),transform(t) {}

	UPROPERTY(EditDefaultsOnly,Category=rdBakedSpawnObjects)
	int32 index=-1;
		
	UPROPERTY(EditDefaultsOnly,Category=rdBakedSpawnObjects)
	uint32 flags=0; // To avoid any potential structure invalidation we just check the bits manually (see above)
		
	UPROPERTY(EditDefaultsOnly,Category=rdBakedSpawnObjects)
	FTransform transform;
};

USTRUCT(BlueprintType)
struct RDINST_PLUGIN_API FrdProxyInstanceGridItem {
	GENERATED_BODY()
public:
	FrdProxyInstanceGridItem() {}
	FrdProxyInstanceGridItem(const FTransform& t,int32 i,const FrdProxyItem& p) : index(i),proxy(p),transform(t) {}
	FrdProxyInstanceGridItem(const FTransform& t,AActor* pf,const FrdProxyItem& px) : prefab(pf),proxy(px),transform(t) {}

	UPROPERTY(EditDefaultsOnly,Category=rdProxyInstanceGridItem)
	int32 index=-1;

	UPROPERTY(EditDefaultsOnly,Category=rdProxyInstanceGridItem)
	AActor* prefab=nullptr;

	UPROPERTY(EditDefaultsOnly,Category=rdProxyInstanceGridItem)
	FrdProxyItem proxy;
		
	UPROPERTY(EditDefaultsOnly,Category=rdProxyInstanceGridItem)
	FTransform transform;
};

USTRUCT(BlueprintType)
struct RDINST_PLUGIN_API FrdGridArrayItem {
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly,Category=rdGridArrayItem)
	TArray<FrdInstanceTransformItem> items;
};

USTRUCT(BlueprintType)
struct RDINST_PLUGIN_API FrdProxyGridArrayItem {
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly,Category=rdGridArrayItem)
	TArray<FrdProxyInstanceGridItem> items;
};

struct rdTriangle {
	rdTriangle(const FVector& p1,const FVector& p2,const FVector& p3) : t1(p1),t2(p2),t3(p3) {}
	bool isInside2d(const FVector& point);
	FVector t1,t2,t3;
};

typedef TFunction<void(FrdInstanceTransformItem& t)> FForEachFunction;
typedef TFunction<void(FrdProxyInstanceGridItem& p)> FForEachProxyFunction;
typedef TFunction<void(TArray<FrdInstanceTransformItem>& t,int32 cell)> FForEachCellFunction;
typedef TFunction<void(FrdInstanceTransformItem& t,int32 state)> FForEachOverlappingFunction;

// Base Struct for Spatial Containers

USTRUCT(BlueprintType)
struct RDINST_PLUGIN_API FrdSpatialContainerBase {
	GENERATED_BODY()
	virtual ~FrdSpatialContainerBase() {}
public:

	virtual void Add(const TArray<FTransform>& t) {}
	//void Add(const TArray<FTransform>& t,const TArray<int32>& i);
	//void Add(const FTransform& t,const int32 i=-1);

	virtual void Precalc() {};

	virtual int32 Num() { return 0; };

	virtual void ForEach(FForEachFunction func) {};
};

USTRUCT(BlueprintType)
struct RDINST_PLUGIN_API FrdGridArray : public FrdSpatialContainerBase {
	GENERATED_BODY()
	~FrdGridArray() {}
public:
	UPROPERTY(EditDefaultsOnly,Category=rdGridArrayItem)
	TMap<int64,FrdGridArrayItem> map;

	void SetGridSize(int32 w,int32 h,bool recreate=false);

	virtual void Add(const TArray<FTransform>& t);
	void Add(const TArray<FTransform>& t,const TArray<int32>& i);
	void Add(const FTransform& t,const int32 i=-1);

	virtual void Precalc() override;

	int32 Num();
	TArray<FTransform> GetAll();
	TArray<FTransform> GetAllRelative(ArdSpawnActor* parent);

	virtual void ForEach(FForEachFunction func) override;
	void ForEachCell(FForEachCellFunction func);
	void ForEachCellInSpheres(const FVector& newPoint,const FVector& oldPoint,float radius,FForEachCellFunction func);
	void ForEachCellInCones(const FVector& newPoint,const FRotator& newRot,const FVector& oldPoint,const FRotator& oldRot,float radius,float range,FForEachCellFunction func);
	void ForEachWithinRadius(const FVector& point,float radius,FForEachFunction func);
	void ForEachOverlappingInSpheres(const FVector& newPoint,const FVector& oldPoint,float radius,FForEachOverlappingFunction func);
	void ForEachOverlappingInTriangles(const FVector& newPoint,const FRotator& newRot,const FVector& oldPoint,const FRotator& oldRot,float radius,float range,FForEachOverlappingFunction func);

	int32 GetItemsWithinRadius(const FVector& point,float radius,TArray<int32>& indices);

	// Number of tiles to split the grid into in the X dimension
	UPROPERTY(Category=rdGridArray,EditDefaultsOnly)
	int32			gridTilesX=16;

	// Number of tiles to split the grid into in the Y dimension
	UPROPERTY(Category=rdGridArray,EditDefaultsOnly)
	int32			gridTilesY=16;

	// used for distance scanning the grids
	float			gridHalfWidth=0.0; // half width of the tiles in pixels
	float			gridHalfHeight=0.0; // half height of the tiles in pixels
	float			gridMaxRadius=0.0; // radius from center of tile to corner
	float			gridMaxDiameter=0.0; // diameter from corner of tile to corner
};

USTRUCT(BlueprintType)
struct RDINST_PLUGIN_API FrdProxyGridArray : public FrdSpatialContainerBase {
	GENERATED_BODY()
	~FrdProxyGridArray() {}
public:
	UPROPERTY(EditDefaultsOnly,Category=rdGridArrayItem)
	TMap<int64,FrdProxyGridArrayItem> map;

	void SetGridSize(int32 w,int32 h,bool recreate=false);

	virtual void Add(const TArray<FTransform>& t) {}
	FrdProxyInstanceGridItem* Add(const FTransform& t,int32 i,const FrdProxyItem& p);
	//void Remove(int32 i);
	virtual int32 Num() override;

	virtual void Precalc() override;

	//virtual void ForEach(FForEachProxyFunction func) override;
	virtual void ForEach(FForEachFunction func) override {};
	void ForEach(FForEachProxyFunction func);
	void ForEachWithinRadius(const FVector& point,float radius,FForEachProxyFunction func);

	FrdProxyInstanceGridItem* FindByInstanceIndex(int32 i);
	float GetLargestLongDistance();

	// Number of tiles to split the grid into in the X dimension
	UPROPERTY(Category=rdGridArray,EditDefaultsOnly)
	int32			gridTilesX=16;

	// Number of tiles to split the grid into in the Y dimension
	UPROPERTY(Category=rdGridArray,EditDefaultsOnly)
	int32			gridTilesY=16;

	// used for distance scanning the grids
	float			gridHalfWidth=0.0; // half width of the tiles in pixels
	float			gridHalfHeight=0.0; // half height of the tiles in pixels
	float			gridMaxRadius=0.0; // radius from center of tile to corner
	float			gridMaxDiameter=0.0; // diameter from corner of tile to corner
};
