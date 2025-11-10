//
// rdGridArray.cpp
//
// Copyright (c) 2022 Recourse Design ltd. All rights reserved.
//
// Creation Date: 6th April 2025 (moved from rdInstBaseActor_Spawn.cpp and refactored into it's own class)
// Last Modified: 13th April 2025
//
// More information on the rdInst SourceCode can be found at https://recourse.nz/index.php/rdinst-source-code/
//
//
// The rdGridArray is a simple 2D spatial based container optimized for fast spatial searching.
//  It takes an X and Y location, divides it by the Grid X/Y size, and packs the integer into a 64bit int, 32 bits per dimension.
//  That's used as the Key in a map of GridItems which have an array of items within.
//
// There are routines to search all, by proximity radius, by difference between two radii, and by difference between two frustums.
//  Check the "ForEach" routines
//
// There are 2 flavors of GridArray here, the first is the FrdGridArray - this is used by the SpawnActor BakedData Assets - they
//  contain all the entities in the spawn actors, collated by type, along with arrays of their transforms in these GridArrays.
//  These are scanned with the player location as reference, only scanning grid cells within the scan radius.
//
// The second one, the FrdProxyGridArray is used by the LongDistance Proxy system. It contains all long-distance proxies in the
//  level, spatially collated into the GridArray to make scanning for long distance proxies quick.
//
// For the type of level layout these are used for currently, the GridArray is more optimal than using an Octree - this will change
//  though as the mass physics system is implemented - it will be able to traverse the octree around the object being calculated in
//  a more optimal way.
// So these classes are based on a generic container from which an Octree will be able to be added using the same methods.
//
//TODO: add moving entities around when their location changes
//
#include "rdGridArray.h"
#include "rdInstBaseActor.h"
#include "rdSpawnActor.h"
#include "Kismet/KismetMathLibrary.h"

//.............................................................................
// SetGridSize
//.............................................................................
void FrdGridArray::SetGridSize(int32 w,int32 h,bool recreate) {

	if(recreate) {
		gridTilesX=w;
		gridTilesY=h;
	} else {
		map.Empty();
		gridTilesX=w;
		gridTilesY=h;
	}
}

//.............................................................................
// Add
//.............................................................................
void FrdGridArray::Add(const TArray<FTransform>& transforms) {

	for(auto& t:transforms) {
		Add(t);
	}
}

//.............................................................................
// Add
//.............................................................................
void FrdGridArray::Add(const TArray<FTransform>& transforms,const TArray<int32>& indices) {

	if(transforms.Num()!=indices.Num()) {
		return;
	}

	int32 i=0;
	for(auto& t:transforms) {
		FrdGridArray::Add(t,indices[i++]);
	}
}

//.............................................................................
// Add
//.............................................................................
void FrdGridArray::Add(const FTransform& transform,const int32 index) {

	FVector point=transform.GetTranslation();
	int32 row=point.X/gridTilesX,col=point.Y/gridTilesY;
	int64 key=(((int64)row)<<32)|col;
	map.FindOrAdd(key).items.Add(FrdInstanceTransformItem(transform,index));
}

//.............................................................................
// Precalc
//.............................................................................
void FrdGridArray::Precalc() {

	float minX=0.0f,minY=0.0f,maxX=0.0f,maxY=0.0f;

	ForEach([&minX,&minY,&maxX,&maxY](FrdInstanceTransformItem& t) { 
		FVector loc=t.transform.GetTranslation();
		if(loc.X<minX) minX=loc.X;
		if(loc.Y<minY) minY=loc.Y;
		if(loc.X>maxX) maxX=loc.X;
		if(loc.Y>maxY) maxY=loc.Y;
	});

	float width=fabs(maxX-minX);
	float height=fabs(maxY-minY);

	gridHalfWidth=width/(float)gridTilesX;
	gridHalfHeight=height/(float)gridTilesY;

	gridMaxRadius=sqrt((gridHalfWidth*gridHalfWidth)+(gridHalfHeight*gridHalfHeight));
	gridMaxDiameter=gridMaxRadius*3.0f; // 1.5* to ensure all encompassed
}

//.............................................................................
// Num
//.............................................................................
int32 FrdGridArray::Num() {

	int32 cnt=0;
	for(auto& it:map) {
		cnt+=it.Value.items.Num();
	}
	return cnt;
}

//.............................................................................
// GetAll
//.............................................................................
TArray<FTransform> FrdGridArray::GetAll() {

	TArray<FTransform> transforms;
	ForEach([&transforms](FrdInstanceTransformItem& t) { transforms.Add(t.transform); });
	return transforms;
}

//.............................................................................
// GetAllRelative
//.............................................................................
TArray<FTransform> FrdGridArray::GetAllRelative(ArdSpawnActor* parent) {

	TArray<FTransform> transforms;
	ForEach([&transforms,&parent](FrdInstanceTransformItem& t) { transforms.Add(parent->rdInstToWorld(t.transform)); });
	return transforms;
}

//.............................................................................
// ForEach
//.............................................................................
void FrdGridArray::ForEach(FForEachFunction func) {

	for(auto& it:map) {
		for(auto& t:it.Value.items) {
			func(t);
		}
	}
}

//.............................................................................
// ForEachCell
//.............................................................................
void FrdGridArray::ForEachCell(FForEachCellFunction func) {

	for(auto& it:map) {
		func(it.Value.items,it.Key);
	}
}

//.............................................................................
// ForEachCellInSpheres
//.............................................................................
void FrdGridArray::ForEachCellInSpheres(const FVector& newPoint,const FVector& oldPoint,float radius,FForEachCellFunction func) {

	for(auto& it:map) {

		FVector gridPos((float)(it.Key>>16)+(gridHalfWidth),(float)(it.Key&0xFFFF)+(gridHalfHeight),0);
		if(FVector::Distance(newPoint,gridPos)<=(radius+gridMaxDiameter) || FVector::Distance(oldPoint,gridPos)<=(radius+gridMaxDiameter)) {
			func(it.Value.items,it.Key);
		}
	}
}

//.............................................................................
// isInside2d
//  Returns true if 'point' is within the 2D triangle (Z ignored but may be used in the future)
//.............................................................................
bool rdTriangle::isInside2d(const FVector& point) {
/*
    int as_x = s.x - a.x;
    int as_y = s.y - a.y;
    bool s_ab = (b.x - a.x) * as_y - (b.y - a.y) * as_x > 0;
    if ((c.x - a.x) * as_y - (c.y - a.y) * as_x > 0 == s_ab) return false;
    if ((c.x - b.x) * (s.y - b.y) - (c.y - b.y)*(s.x - b.x) > 0 != s_ab) return false;
    return true;
*/
	float x=point.X-t1.X;
	float y=point.Y-t1.Y;
	bool ab=((t2.X-t1.X)*y-(t2.Y-t1.Y)*x)>0;

	if(((t3.X-t1.X)*y-(t3.Y-t1.Y)*x>0)==ab) return false;
	if(((t3.X-t2.X)*(point.Y-t2.Y)-(t3.Y-t2.Y)*(point.X-t2.X)>0)!=ab) return false;
	return true;
}

//.............................................................................
// ForEachCellInCones
//.............................................................................
void FrdGridArray::ForEachCellInCones(const FVector& newPoint,const FRotator& newRot,const FVector& oldPoint,const FRotator& oldRot,float radius,float range,FForEachCellFunction func) {

	FVector fwd1=UKismetMathLibrary::GetForwardVector(FRotator(0,newRot.Yaw,0))*radius;
	FVector fwd2=UKismetMathLibrary::GetForwardVector(FRotator(0,oldRot.Yaw,0))*radius;
	rdTriangle tri1(newPoint,FRotator(0,-range*0.5f,0).RotateVector(fwd1)+newPoint,FRotator(0,range*0.5f,0).RotateVector(fwd1)+newPoint);
	rdTriangle tri2(oldPoint,FRotator(0,-range*0.5f,0).RotateVector(fwd2)+oldPoint,FRotator(0,range*0.5f,0).RotateVector(fwd2)+oldPoint);

	for(auto& it:map) {
		const FVector gridPos((float)(it.Key>>16)+gridHalfWidth,(float)(it.Key&0xFFFF)+gridHalfHeight,0);
		//if(FVector::Distance(newPoint,gridPos)<=(radius+gridMaxDiameter) || FVector::Distance(oldPoint,gridPos)<=(radius+gridMaxDiameter)) {
			if(tri1.isInside2d(gridPos) || tri2.isInside2d(gridPos)) {
				func(it.Value.items,it.Key);
			}
		//}
	}
}

//.............................................................................
// ForEachWithinRadius
//.............................................................................
void FrdGridArray::ForEachWithinRadius(const FVector& point,float radius,FForEachFunction func) {

	for(auto& it:map) {
		FVector gridPos((float)(it.Key>>16),(float)(it.Key&0xFFFF),0);
		if(FVector::Distance(point,gridPos)<=(radius+gridMaxDiameter)) {
			for(auto& t:it.Value.items) {
				if(FVector::Distance(t.transform.GetTranslation(),point)<=radius) {
					func(t);
				}
			}
		}
	}
}

//.............................................................................
// ForEachOverlappingInSpheres
//   Enumerates the entities, calling the func for each entity transform that lies
//    within the 2 circles - the state passed to the function is:
//                                   0 - transform is within both circles (still gets enumerated so proxies can be swapped from same loop)
//                                   1 - transform is within the new circle, but not the old
//                                   2 - transform is within the old circle, but not the new
//.............................................................................
void FrdGridArray::ForEachOverlappingInSpheres(const FVector& newPoint,const FVector& oldPoint,float radius,FForEachOverlappingFunction func) {

	for(auto& it:map) {
		const FVector gridPos((float)((int32)(it.Key>>32)),(float)((int32)(it.Key&0xFFFFFFFF)),0);
		bool inS1=FVector::Distance(gridPos,newPoint)<=(radius+gridMaxDiameter);
		bool inS2=FVector::Distance(gridPos,oldPoint)<=(radius+gridMaxDiameter);
			
		if(!inS1&&!inS2) continue;
		for(FrdInstanceTransformItem& t:it.Value.items) {

			inS1=FVector::Distance(t.transform.GetTranslation(),newPoint)<=radius;
			inS2=FVector::Distance(t.transform.GetTranslation(),oldPoint)<=radius;
			if(!inS1&&!inS2) continue;
			int32 state=(inS1&&inS2)?0:((inS1&&!inS2)?1:2);
			if(inS1 || inS2) {
				func(t,state);
			}
		}
	}
}

//.............................................................................
// ForEachOverlappingInTriangles
//.............................................................................
void FrdGridArray::ForEachOverlappingInTriangles(const FVector& newPoint,const FRotator& newRot,const FVector& oldPoint,const FRotator& oldRot,float radius,float range,FForEachOverlappingFunction func) {

	FVector fwd1=UKismetMathLibrary::GetForwardVector(FRotator(0,newRot.Yaw,0))*radius;
	FVector fwd2=UKismetMathLibrary::GetForwardVector(FRotator(0,oldRot.Yaw,0))*radius;
	rdTriangle tri1(newPoint,FRotator(0,-range*0.5f,0).RotateVector(fwd1)+newPoint,FRotator(0,range*0.5f,0).RotateVector(fwd1)+newPoint);
	rdTriangle tri2(oldPoint,FRotator(0,-range*0.5f,0).RotateVector(fwd2)+oldPoint,FRotator(0,range*0.5f,0).RotateVector(fwd2)+oldPoint);

	for(auto& it:map) {
		const FVector gridPos((float)((int32)(it.Key>>32))+gridHalfWidth,(float)((int32)(it.Key&0xFFFFFFFF))+gridHalfHeight,0);
		bool inT1=tri1.isInside2d(gridPos);
		bool inT2=tri2.isInside2d(gridPos);
			
		if(!inT1&&!inT2) continue;

		for(FrdInstanceTransformItem& t:it.Value.items) {

			inT1=tri1.isInside2d(t.transform.GetTranslation());
			inT2=tri2.isInside2d(t.transform.GetTranslation());
			if(inT1||inT2) {
				int32 state=(inT1&&inT2)?0:((inT1&&!inT2)?1:2);
				func(t,state);
			}
		}
	}
}

//.............................................................................
// GetItemsWithinRadius
//.............................................................................
int32 FrdGridArray::GetItemsWithinRadius(const FVector& point,float radius,TArray<int32>& indices) {

	int32 cnt=0;

	for(auto& it:map) {
		const FVector gridPos((float)((int32)(it.Key>>32)),(float)((int32)(it.Key&0xFFFFFFFF)),0);
		bool inS=FVector::Distance(gridPos,point)<=(radius+gridMaxDiameter);
		if(!inS) continue;
		for(FrdInstanceTransformItem& t:it.Value.items) {

			inS=FVector::Distance(t.transform.GetTranslation(),point)<=radius;
			if(inS) {
				indices.Add(t.index);
				cnt++;
			}
		}
	}

	return cnt;
}

//
// ProxyGridArray
//

//.............................................................................
// SetGridSize
//.............................................................................
void FrdProxyGridArray::SetGridSize(int32 w,int32 h,bool recreate) {

	if(recreate) {
		gridTilesX=w;
		gridTilesY=h;
	} else {
		map.Empty();
		gridTilesX=w;
		gridTilesY=h;
	}
}

//.............................................................................
// Add
//.............................................................................
FrdProxyInstanceGridItem* FrdProxyGridArray::Add(const FTransform& t,int32 i,const FrdProxyItem& p) {

	FVector point=t.GetTranslation();
	int32 row=point.X/gridTilesX,col=point.Y/gridTilesY;
	int64 key=(((int64)row)<<32)|col;
	FrdProxyGridArrayItem& pga=map.FindOrAdd(key);
	return &pga.items.Add_GetRef(FrdProxyInstanceGridItem(t,i,p));
}

//.............................................................................
// Num
//.............................................................................
int32 FrdProxyGridArray::Num() {
	int32 cnt=0;
	for(auto& it:map) {
		cnt+=it.Value.items.Num();
	}
	return cnt;
}

//.............................................................................
// Precalc
//.............................................................................
void FrdProxyGridArray::Precalc() {

	float minX=0.0f,minY=0.0f,maxX=0.0f,maxY=0.0f;

	ForEach([&minX,&minY,&maxX,&maxY](FrdProxyInstanceGridItem& t) { 
		FVector loc=t.transform.GetTranslation();
		if(loc.X<minX) minX=loc.X;
		if(loc.Y<minY) minY=loc.Y;
		if(loc.X>maxX) maxX=loc.X;
		if(loc.Y>maxY) maxY=loc.Y;
	});

	float width=fabs(maxX-minX);
	float height=fabs(maxY-minY);

	gridHalfWidth=width/(float)gridTilesX;
	gridHalfHeight=height/(float)gridTilesY;

	gridMaxRadius=sqrt((gridHalfWidth*gridHalfWidth)+(gridHalfHeight*gridHalfHeight));
	gridMaxDiameter=gridMaxRadius*3.0f; // 1.5* to ensure all encompassed
}

//.............................................................................
// FindByInstanceIndex
//.............................................................................
FrdProxyInstanceGridItem* FrdProxyGridArray::FindByInstanceIndex(int32 i) {
	for(auto& it:map) {
		for(auto& pgi:it.Value.items) {
			if(pgi.index==i) {
				return &pgi;
			}
		}
	}
	return nullptr;
}

//.............................................................................
// GetLargestLongDistance
//.............................................................................
float FrdProxyGridArray::GetLargestLongDistance() {
	float maxDist=0.0f;
	for(auto& it:map) {
		for(auto& pgi:it.Value.items) {
			if(pgi.proxy.distance>maxDist) {
				maxDist=pgi.proxy.distance;
			}
		}
	}
	return maxDist;
}

//.............................................................................
// ForEach
//.............................................................................
void FrdProxyGridArray::ForEach(FForEachProxyFunction func) {

	for(auto& it:map) {
		for(auto& t:it.Value.items) {
			func(t);
		}
	}
}

//.............................................................................
// ForEachWithinRadius
//.............................................................................
void FrdProxyGridArray::ForEachWithinRadius(const FVector& point,float radius,FForEachProxyFunction func) {

	for(auto& it:map) {
		FVector gridPos((float)((int32)(it.Key>>32)),(float)((int32)(it.Key&0xFFFFFFFF)),0);
		if(FVector::Distance(point,gridPos)<=(radius+gridMaxDiameter)) {
			for(auto& t:it.Value.items) {
				if(FVector::Distance(t.transform.GetTranslation(),point)<=radius) {
					func(t);
				}
			}
		}
	}
}

//.............................................................................

