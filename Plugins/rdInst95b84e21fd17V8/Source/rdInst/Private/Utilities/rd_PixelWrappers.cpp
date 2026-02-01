//
// rd_PixelHelpers.cpp
//
// Copyright (c) 2021 Recourse Design ltd. All rights reserved.
//
// Version 1.10
//
// Creation Date: 22nd August 2021
// Last Modified: 3rd February 2024
//
// More information on the rdInst SourceCode can be found at https://recourse.nz/index.php/rdinst-source-code/
//
#include "rd_PixelWrappers.h"
#include "Runtime/Engine/Classes/Engine/Texture.h"
#include "Runtime/CoreUObject/Public/UObject/Object.h"
#include "rdInstBaseActor.h"

#define LOCTEXT_NAMESPACE "FrdInstModule"

//----------------------------------------------------------------------------------------------------------------
//	setData
//----------------------------------------------------------------------------------------------------------------
void rdPixelWrapper::setData(int16 w,int16 h,void* dat,uint8 r,bool fV,bool fH) {

	width=w;
	height=h;
	data=dat;
	rot=r;
	flipV=fV;
	flipH=fH;

	translate=(rot||flipV||flipH);
}

//----------------------------------------------------------------------------------------------------------------
//	createPixelWrapper
//----------------------------------------------------------------------------------------------------------------
rdPixelWrapper* ArdInstBaseActor::createPixelWrapper(uint8* dat,const int16 w,int16 h,uint16 fmt,int8 interpType,int16 rot,bool flipV,bool flipH) {
	
	rdPixelWrapper* wrapper=nullptr;

	switch((int32)fmt) {
		case TSF_Float:
			switch(interpType) {
				case RD_INTERP_NEAREST: wrapper=new rdPixelWrapper_Float(); break;
				case RD_INTERP_LINEAR: wrapper=new rdPixelWrapper_Float_Linear(); break;
				case RD_INTERP_BILINEAR: wrapper=new rdPixelWrapper_Float_Bilinear(); break;
				case RD_INTERP_BICUBIC: wrapper=new rdPixelWrapper_Float_Bicubic(); break;
				case RD_INTERP_AVERAGE: wrapper=new rdPixelWrapper_Float_Average(); break;
			}
			break;
		case TSF_G8:
			switch(interpType) {
				case RD_INTERP_NEAREST: wrapper=new rdPixelWrapper_G8(); break;
				case RD_INTERP_LINEAR: wrapper=new rdPixelWrapper_G8_Linear(); break;
				case RD_INTERP_BILINEAR: wrapper=new rdPixelWrapper_G8_Bilinear(); break;
				case RD_INTERP_BICUBIC: wrapper=new rdPixelWrapper_G8_Bicubic(); break;
				case RD_INTERP_AVERAGE: wrapper=new rdPixelWrapper_G8_Average(); break;
			}
			break;
		case TSF_G16:
			switch(interpType) {
				case RD_INTERP_NEAREST: wrapper=new rdPixelWrapper_G16(); break;
				case RD_INTERP_LINEAR: wrapper=new rdPixelWrapper_G16_Linear(); break;
				case RD_INTERP_BILINEAR: wrapper=new rdPixelWrapper_G16_Bilinear(); break;
				case RD_INTERP_BICUBIC: wrapper=new rdPixelWrapper_G16_Bicubic(); break;
				case RD_INTERP_AVERAGE: wrapper=new rdPixelWrapper_G16_Average(); break;
			}
			break;
		case TSF_BGRA8:
			switch(interpType) {
				case RD_INTERP_NEAREST: wrapper=new rdPixelWrapper_BGRA8(); break;
				case RD_INTERP_LINEAR: wrapper=new rdPixelWrapper_BGRA8_Linear(); break;
				case RD_INTERP_BILINEAR: wrapper=new rdPixelWrapper_BGRA8_Bilinear(); break;
				case RD_INTERP_BICUBIC: wrapper=new rdPixelWrapper_BGRA8_Bicubic(); break;
				case RD_INTERP_AVERAGE: wrapper=new rdPixelWrapper_BGRA8_Average(); break;
			}
			break;
		case TSF_BGRE8: // HDR/CubeMap
			switch(interpType) {
				case RD_INTERP_NEAREST: wrapper=new rdPixelWrapper_BGRE8(); break;
				case RD_INTERP_LINEAR: wrapper=new rdPixelWrapper_BGRE8_Linear(); break;
				case RD_INTERP_BILINEAR: wrapper=new rdPixelWrapper_BGRE8_Bilinear(); break;
				case RD_INTERP_BICUBIC: wrapper=new rdPixelWrapper_BGRE8_Bicubic(); break;
				case RD_INTERP_AVERAGE: wrapper=new rdPixelWrapper_BGRE8_Average(); break;
			}
			break;
		case TSF_RGBA16:
			switch(interpType) {
				case RD_INTERP_NEAREST: wrapper=new rdPixelWrapper_RGBA16(); break;
				case RD_INTERP_LINEAR: wrapper=new rdPixelWrapper_RGBA16_Linear(); break;
				case RD_INTERP_BILINEAR: wrapper=new rdPixelWrapper_RGBA16_Bilinear(); break;
				case RD_INTERP_BICUBIC: wrapper=new rdPixelWrapper_RGBA16_Bicubic(); break;
				case RD_INTERP_AVERAGE: wrapper=new rdPixelWrapper_RGBA16_Average(); break;
			}
			break;
		case TSF_RGBA16F:
			switch(interpType) {
				case RD_INTERP_NEAREST: wrapper=new rdPixelWrapper_RGBA16F(); break;
				case RD_INTERP_LINEAR: wrapper=new rdPixelWrapper_RGBA16F_Linear(); break;
				case RD_INTERP_BILINEAR: wrapper=new rdPixelWrapper_RGBA16F_Bilinear(); break;
				case RD_INTERP_BICUBIC: wrapper=new rdPixelWrapper_RGBA16F_Bicubic(); break;
				case RD_INTERP_AVERAGE: wrapper=new rdPixelWrapper_RGBA16F_Average(); break;
			}
			break;
#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>0
		case TSF_RGBA32F:
			switch(interpType) {
				case RD_INTERP_NEAREST: wrapper=new rdPixelWrapper_RGBA32F(); break;
				case RD_INTERP_LINEAR: wrapper=new rdPixelWrapper_RGBA32F_Linear(); break;
				case RD_INTERP_BILINEAR: wrapper=new rdPixelWrapper_RGBA32F_Bilinear(); break;
				case RD_INTERP_BICUBIC: wrapper=new rdPixelWrapper_RGBA32F_Bicubic(); break;
				case RD_INTERP_AVERAGE: wrapper=new rdPixelWrapper_RGBA32F_Average(); break;
			}
			break;
#endif
		default:
			return nullptr;
	}

	wrapper->setData(w,h,dat,rot,flipV,flipH);

	return wrapper;
}

//----------------------------------------------------------------------------------------------------------------
// translatePos
//----------------------------------------------------------------------------------------------------------------
inline void rdPixelWrapper::translatePos(double& x,double& y) {

	switch(rot) {
		case 90: {
			double x1=x;
			x=(y*width)/height;
			y=height-((x1*height)/width);
			break; }
		case 180:
			x=width-x;
			y=height-y;
			break;
		case 270: {
			double x1=x;
			x=width-((y*width)/height);
			y=(x1*height)/width;
			break; }
	}
	if(flipV) y=height-y;
	if(flipH) x=width-x;
}

//----------------------------------------------------------------------------------------------------------------
// translatePos
//----------------------------------------------------------------------------------------------------------------
inline void rdPixelWrapper::translatePosI(double& x,double& y) {
	translatePos(x,y);
	translate=false;
}

//----------------------------------------------------------------------------------------------------------------
// interpolatePixel
//----------------------------------------------------------------------------------------------------------------
inline FLinearColor rdPixelWrapper::interpolateLinearPixel_Linear(double x,double y,const FLinearColor& c1,const FLinearColor& c2) {

	double fx=1.0f-(x-(uint32)x);

	lcol.R=fx*(c1.R-c2.R)+c2.R;
	lcol.G=fx*(c1.G-c2.G)+c2.G;
	lcol.B=fx*(c1.B-c2.B)+c2.B;
	lcol.A=fx*(c1.A-c2.A)+c2.A;

	return lcol;
}

inline FLinearColor rdPixelWrapper::interpolateFloat_Linear(double x,double y,float c1,float c2) {

	double fx=1.0f-(x-(uint32)x);

	lcol.R=lcol.G=lcol.B=fx*(c1-c2)+c2;

	return lcol;
}

inline FLinearColor rdPixelWrapper::interpolateLinearPixel_Bilinear(double x,double y,const FLinearColor& c1,const FLinearColor& c2,const FLinearColor& c3,const FLinearColor& c4) {
	
	double fx=x-(uint32)x,fy=y-(uint32)y,fx1=1.0f-fx,fy1=1.0f-fy;
	double w1=fx1*fy1,w2=fx*fy1,w3=fx1*fy,w4=fx*fy;
 
	lcol.R=c1.R*w1+c2.R*w2+c3.R*w3+c4.R*w4;
	lcol.G=c1.G*w1+c2.G*w2+c3.G*w3+c4.G*w4;
	lcol.B=c1.B*w1+c2.B*w2+c3.B*w3+c4.B*w4;
	lcol.A=c1.A*w1+c2.A*w2+c3.A*w3+c4.A*w4;
 
	return lcol;
}

inline FLinearColor rdPixelWrapper::interpolateFloat_Bilinear(double x,double y,float c1,float c2,float c3,float c4) {
	
	double fx=x-(uint32)x,fy=y-(uint32)y,fx1=1.0f-fx,fy1=1.0f-fy;
	float w1=fx1*fy1,w2=fx*fy1,w3=fx1*fy,w4=fx*fy;
 
	lcol.R=lcol.G=lcol.B=c1*w1+c2*w2+c3*w3+c4*w4;

	return lcol;
}

inline FLinearColor rdPixelWrapper::interpolateLinearPixel_Bicubic(double x,double y,const FLinearColor& c11,const FLinearColor& c12,const FLinearColor& c13,const FLinearColor& c14,const FLinearColor& c21,const FLinearColor& c22,const FLinearColor& c23,const FLinearColor& c24,const FLinearColor& c31,const FLinearColor& c32,const FLinearColor& c33,const FLinearColor& c34,const FLinearColor& c41,const FLinearColor& c42,const FLinearColor& c43,const FLinearColor& c44) {

	lcol.R=bicubicInterpolate(x,y,c11.R,c12.R,c13.R,c14.R,c21.R,c22.R,c23.R,c24.R,c31.R,c32.R,c33.R,c34.R,c41.R,c42.R,c43.R,c44.R);
	lcol.G=bicubicInterpolate(x,y,c11.G,c12.G,c13.G,c14.G,c21.G,c22.G,c23.G,c24.G,c31.G,c32.G,c33.G,c34.G,c41.G,c42.G,c43.G,c44.G);
	lcol.B=bicubicInterpolate(x,y,c11.B,c12.B,c13.B,c14.B,c21.B,c22.B,c23.B,c24.B,c31.B,c32.B,c33.B,c34.B,c41.B,c42.B,c43.B,c44.B);
	lcol.A=bicubicInterpolate(x,y,c11.A,c12.A,c13.A,c14.A,c21.A,c22.A,c23.A,c24.A,c31.A,c32.A,c33.A,c34.A,c41.A,c42.A,c43.A,c44.A);
	return lcol;
}

inline FLinearColor rdPixelWrapper::interpolateFloat_Bicubic(double x,double y,float c11,float c12,float c13,float c14,float c21,float c22,float c23,float c24,float c31,float c32,float c33,float c34,float c41,float c42,float c43,float c44) {

	lcol.R=lcol.G=lcol.B=bicubicInterpolate(x,y,c11,c12,c13,c14,c21,c22,c23,c24,c31,c32,c33,c34,c41,c42,c43,c44);
	return lcol;
}


inline FLinearColor rdPixelWrapper::interpolateLinearPixel_Average(double x,double y,const FLinearColor& c1,const FLinearColor& c2,const FLinearColor& c3,const FLinearColor& c4) {
	
	lcol.R=(c1.R+c2.R+c3.R+c4.R)/4.0f; 
	lcol.G=(c1.G+c2.G+c3.G+c4.G)/4.0f; 
	lcol.B=(c1.B+c2.B+c3.B+c4.B)/4.0f; 
	lcol.A=(c1.A+c2.A+c3.A+c4.A)/4.0f; 
	return lcol;
}

inline FLinearColor rdPixelWrapper::interpolateFloat_Average(double x,double y,float c1,float c2,float c3,float c4) {
	
	lcol.R=lcol.G=lcol.B=(c1+c2+c3+c4)/4.0f;
	return lcol;
}

float rdPixelWrapper::cubicInterpolate(double x,float c1,float c2,float c3,float c4) {
	return c2+0.5f*x*(c3-c1+x*(2.0f*c1-5.0f*c2+4.0f*c3-c4+x*(3.0f*(c2-c3)+c4-c1)));
}

float rdPixelWrapper::bicubicInterpolate(double x,double y,float c11,float c12,float c13,float c14,float c21,float c22,float c23,float c24,float c31,float c32,float c33,float c34,float c41,float c42,float c43,float c44) {
	double fx=(x-(uint32)x),fy=(y-(uint32)y);
	float r1,r2,r3,r4;
	r1=cubicInterpolate(fx,c11,c12,c13,c14);
	r2=cubicInterpolate(fx,c21,c22,c23,c24);
	r3=cubicInterpolate(fx,c31,c32,c33,c34);
	r4=cubicInterpolate(fx,c41,c42,c43,c44);
	float ret=cubicInterpolate(fy,r1,r2,r3,r4);
	if(ret<0.0f) ret=0.0f;
	if(ret>1.0f) ret=1.0f;
	return ret;
}

//----------------------------------------------------------------------------------------------------------------
#undef LOCTEXT_NAMESPACE
