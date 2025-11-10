//
// rdInstBaseActor_LandscapeModify.cpp
//
// Copyright (c) 2022 Recourse Design ltd. All rights reserved.
//
// Creation Date: 3rd February 2024
// Last Modified: 11th October 2024
//
// More information on the rdInst SourceCode can be found at https://recourse.nz/index.php/rdinst-source-code/
//
#include "rdInstBaseActor.h"
#include "Engine/World.h"
#include "Landscape.h"
#include "LandscapeProxy.h"
#include "LandscapeDataAccess.h"
#if WITH_EDITOR
#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>0
#include "LandscapeEdit.h"
#endif
#endif
#include "Misc/FileHelper.h"
#include "Kismet/GameplayStatics.h"

//.............................................................................
// rdPaintLandscape
//.............................................................................
double ArdInstBaseActor::rdGetLandscapeZ(double x,double y) {

	return 0.0f;
}

//.............................................................................
// rdPaintLandscape
//.............................................................................
void ArdInstBaseActor::rdPaintLandscape(ULandscapeLayerInfoObject* paintLayer,TEnumAsByte<rdLandscapeShape> shape,float x,float y,float width,float height,float angle,float strength,float falloff,USplineComponent* spline,const FString& layer) {

	UTexture2D* stamp=rdCreateTextureForShape(shape,x,y,width,height,angle,strength,falloff,spline);
}

//.............................................................................
// rdFlattenLandscapeArea
//.............................................................................
void ArdInstBaseActor::rdFlattenLandscapeArea(TEnumAsByte<rdLandscapeShape> shape,float x,float y,float width,float height,float angle,float strength,float falloff,USplineComponent* spline,const FString& layer) {

	UTexture2D* stamp=rdCreateTextureForShape(shape,x,y,width,height,angle,strength,falloff,spline);
	rdStampLandscapeArea(stamp,x,y,width,height,strength,layer);
}

//.............................................................................
// rdStampLandscapeArea
//.............................................................................
void ArdInstBaseActor::rdStampLandscapeArea(UTexture2D* stamp,float x,float y,float width,float height,float strength,const FString& layer) {

#if WITH_EDITOR
#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>0

	TArray<AActor*> landscapes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(),ALandscape::StaticClass(),landscapes);
	for(auto a:landscapes) {
		ALandscape* ls=(ALandscape*)a;
		FIntRect rct=ls->GetBoundingRect()+ls->LandscapeSectionOffset;
		int32 wmSize=8129;
		if(x>=rct.Min.X && x<=rct.Max.X && y>=rct.Min.Y && y<=rct.Max.Y) {

			TArray<ULandscapeComponent*> lsComps;
			a->GetComponents(ULandscapeComponent::StaticClass(),(TArray<UActorComponent*>&)lsComps,false);
			for(auto& c:lsComps) {

				UTexture2D* tex=c->GetLandscapeProxy()->CreateLandscapeTexture(wmSize,wmSize,TEXTUREGROUP_Terrain_Weightmap,TSF_BGRA8);

				FColor* data=(FColor*)tex->Source.LockMip(0);
				FFileHelper::CreateBitmap(*(FPaths::ScreenShotDir()/"LandscapeMap"),wmSize,wmSize,data);//@@
				tex->Source.UnlockMip(0);

			}
			/*
			FHeightmapAccessor<false> dat(ls->GetLandscapeInfo());
			FLandscapeTextureDataInterface* tdi=
			accessor.SetData(rct.Min.X,rct.Min.Y,rct.Max.X,rct.Max.Y,dat.GetData());
			FColor* rawdata=LandscapeDataAccess::GetRawHeightData();
			*/
		}
	}
#endif
#else
#endif
}

//.............................................................................
// rdCreateTextureForShape
//.............................................................................
UTexture2D* ArdInstBaseActor::rdCreateTextureForShape(TEnumAsByte<rdLandscapeShape> shape,float x,float y,float width,float height,float angle,float strength,float falloff,USplineComponent* spline) {

	UTexture2D* tex=nullptr;

	switch(shape) {
		case RDLANDSCAPESHAPE_RECTANGLE:
			break;
		case RDLANDSCAPESHAPE_CIRCLE:
			break;
		case RDLANDSCAPESHAPE_SPLINE:
			break;
	}

	return tex;
}

//.............................................................................
