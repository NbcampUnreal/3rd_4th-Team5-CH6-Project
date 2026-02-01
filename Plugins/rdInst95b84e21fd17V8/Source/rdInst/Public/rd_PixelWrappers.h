// rd_PixelWrappers.h - Copyright (c) 2021 Recourse Design ltd.
#pragma once
#include "Math/Color.h"
#include "Math/Float16Color.h"

enum rdInterpolationType {
	RD_INTERP_NEAREST=0,
	RD_INTERP_LINEAR=1,
	RD_INTERP_BILINEAR=2,
	RD_INTERP_BICUBIC=3,
	RD_INTERP_AVERAGE=4
};

#define TSF_Float TSF_MAX+1
typedef signed short int16;

class rdPixelWrapper {
public:
	virtual ~rdPixelWrapper() {}

	void				setData(int16 w,int16 h,void* dat,uint8 rot,bool flipV,bool flipH);

	void				translatePos(double& x,double& y);
	void				translatePosI(double& x,double& y);

	virtual FLinearColor readLinearPixel(double x,double y)=0;
	virtual FColor		 readPixel(double x,double y)=0;

	FLinearColor		interpolateLinearPixel_Linear(double x,double y,const FLinearColor& c1,const FLinearColor& c2);
	FLinearColor		interpolateLinearPixel_Bilinear(double x,double y,const FLinearColor& c1,const FLinearColor& c2,const FLinearColor& c3,const FLinearColor& c4);
	FLinearColor		interpolateLinearPixel_Bicubic(double x,double y,const FLinearColor& c11,const FLinearColor& c12,const FLinearColor& c13,const FLinearColor& c14,const FLinearColor& c21,const FLinearColor& c22,const FLinearColor& c23,const FLinearColor& c24,const FLinearColor& c31,const FLinearColor& c32,const FLinearColor& c33,const FLinearColor& c34,const FLinearColor& c41,const FLinearColor& c42,const FLinearColor& c43,const FLinearColor& c44);
	FLinearColor		interpolateLinearPixel_Average(double x,double y,const FLinearColor& c1,const FLinearColor& c2,const FLinearColor& c3,const FLinearColor& c4);

	FLinearColor		interpolateFloat_Linear(double x,double y,float c1,float c2);
	FLinearColor		interpolateFloat_Bilinear(double x,double y,float c1,float c2,float c3,float c4);
	FLinearColor		interpolateFloat_Bicubic(double x,double y,float c11,float c12,float c13,float c14,float c21,float c22,float c23,float c24,float c31,float c32,float c33,float c34,float c41,float c42,float c43,float c44);
	FLinearColor		interpolateFloat_Average(double x,double y,float c1,float c2,float c3,float c4);

	virtual void		writeLinearPixel(int16 x,int16 y,const FLinearColor& scol)=0;
	virtual	void		writePixel(int16 x,int16 y,const FColor& scol)=0;

//	inline double		clampFloat(const double v) {	return v>=0.0f?(v>1.0f?1.0f:v):1.0f-(fabs(v)-(uint32)fabs(v)); }
	inline double		clampFloat(const double v) {	return v>=0.0f?(v>1.0f?1.0f:v):0.0f; }
	inline void			clampLinear(const FLinearColor& scol) { 
																lcol.R=clampFloat(scol.R); 
																lcol.G=clampFloat(scol.G); 
																lcol.B=clampFloat(scol.B); 
																lcol.A=clampFloat(scol.A); 
															  }
	inline uint32		getWidth() { return width; }
	inline uint32		getHeight() { return height; }
protected:
	uint32		 width,height;
	void*		 data;
	int16		 rot;
	bool		 flipV;
	bool		 flipH;
	bool		 translate;
	FLinearColor lcol;
	FColor		 col;

	float		cubicInterpolate(double x,float c1,float c2,float c3,float c4);
	float		bicubicInterpolate(double x,double y,float c11,float c12,float c13,float c14,float c21,float c22,float c23,float c24,float c31,float c32,float c33,float c34,float c41,float c42,float c43,float c44);
	float		averageInterpolate(double x,float c1,float c2,float c3,float c4);
};

//-------------------------------------------------------------------------------------------------
class rdPixelWrapper_Float : public rdPixelWrapper {
public:
	virtual ~rdPixelWrapper_Float() {}
	//  define the read method as inline so their derived interpolation classes can inline them...
	inline FLinearColor  read(double x,double y) { lcol.R=lcol.G=lcol.B=((double*)data)[(uint32)(((uint32)y*width)+x)]; lcol.A=1.0; return lcol; }
	virtual FLinearColor readLinearPixel(double x,double y) { if(translate) translatePos(x,y); return read(x,y); }
	virtual FColor		 readPixel(double x,double y) { if(translate) translatePos(x,y); col.R=col.G=col.B=((float*)data)[(uint32)(((uint32)y*width)+x)]; col.A=255; return col; }
	virtual void		 writeLinearPixel(int16 x,int16 y,const FLinearColor& scol) { ((float*)data)[(y*width)+x]=lcol.R; }
	virtual void		 writePixel(int16 x,int16 y,const FColor& scol) { ((float*)data)[(y*width)+x]=scol.R/256.0f; }
};

class rdPixelWrapper_Float_Linear : public rdPixelWrapper_Float {
public:
	virtual ~rdPixelWrapper_Float_Linear() {}
	virtual FLinearColor readLinearPixel(double x,double y) { if(translate) translatePos(x,y); return (x<width-1 && y<height-1)?interpolateFloat_Linear(x,y,rdPixelWrapper_Float::read(x,y).R,rdPixelWrapper_Float::read(x+1,y).R):rdPixelWrapper_Float::read(x,y); }
};

class rdPixelWrapper_Float_Bilinear : public rdPixelWrapper_Float {
public:
	virtual ~rdPixelWrapper_Float_Bilinear() {}
	virtual FLinearColor readLinearPixel(double x,double y) { if(translate) translatePos(x,y); return (x<width-1 && y<height-1)?interpolateFloat_Bilinear(x,y,rdPixelWrapper_Float::read(x,y).R,rdPixelWrapper_Float::read(x+1.0,y).R,rdPixelWrapper_Float::read(x,y+1.0).R,rdPixelWrapper_Float::read(x+1.0,y+1.0).R):rdPixelWrapper_Float::read(x,y); }
};

class rdPixelWrapper_Float_Bicubic : public rdPixelWrapper_Float {
public:
	virtual ~rdPixelWrapper_Float_Bicubic() {}
#define rdpx rdPixelWrapper_Float::read
#define rdpxs rdpx(x-1.0,y-1.0).R,rdpx(x,y-1.0).R,rdpx(x+1.0,y-1.0).R,rdpx(x+2.0,y-1.0).R,rdpx(x-1.0,y).R,rdpx(x,y).R,rdpx(x+1.0,y).R,rdpx(x+2.0,y).R,rdpx(x-1.0,y+1.0).R,rdpx(x,y+1.0).R,rdpx(x+1.0,y+1.0).R,rdpx(x+2.0,y+1.0).R,rdpx(x-1.0,y+2.0).R,rdpx(x,y+2.0).R,rdpx(x+1.0,y+2.0).R,rdpx(x+2.0,y+2.0).R
	virtual FLinearColor readLinearPixel(double x,double y) { if(translate) translatePos(x,y); return (x>0 && y>0 && x<width-2 && y<height-2)?interpolateFloat_Bicubic(x,y,rdpxs):rdPixelWrapper_Float::read(x,y); }
#undef rdpxs
#undef rdpx
};

class rdPixelWrapper_Float_Average : public rdPixelWrapper_Float {
public:
	virtual ~rdPixelWrapper_Float_Average() {}
	virtual FLinearColor readLinearPixel(double x,double y) { if(translate) translatePos(x,y); return (x<width-1 && y<height-1)?interpolateFloat_Average(x,y,rdPixelWrapper_Float::read(x,y).R,rdPixelWrapper_Float::read(x+1.0,y).R,rdPixelWrapper_Float::read(x,y+1.0).R,rdPixelWrapper_Float::read(x+1.0,y+1.0).R):rdPixelWrapper_Float::read(x,y); }
};

//-------------------------------------------------------------------------------------------------
class rdPixelWrapper_G8 : public rdPixelWrapper {
public:
	virtual ~rdPixelWrapper_G8() {}
	//  define the read method as inline so their derived interpolation classes can inline them...
	inline FLinearColor  read(double x,double y) { lcol.R=lcol.G=lcol.B=((uint8*)data)[(uint32)(((uint32)y*width)+x)]/256.0; lcol.A=1.0; return lcol; }
	virtual FLinearColor readLinearPixel(double x,double y) { if(translate) translatePos(x,y); return read(x,y); }
	virtual FColor		 readPixel(double x,double y) { if(translate) translatePos(x,y); col.R=col.G=col.B=((uint8*)data)[(uint32)(((uint32)y*width)+x)]; col.A=255; return col; }
	virtual void		 writeLinearPixel(int16 x,int16 y,const FLinearColor& scol) { clampLinear(scol); ((uint8*)data)[(y*width)+x]=(uint8)(((lcol.R+lcol.G+lcol.B)/3.0)*255.0); }
	virtual void		 writePixel(int16 x,int16 y,const FColor& scol) { ((uint8*)data)[(y*width)+x]=(uint8)((scol.R+scol.G+scol.B)/3); }
	virtual void		 setAlpha(int16 x,int16 y,float alpha) {}
};

class rdPixelWrapper_G8_Linear : public rdPixelWrapper_G8 {
public:
	virtual ~rdPixelWrapper_G8_Linear() {}
	virtual FLinearColor readLinearPixel(double x,double y) { if(translate) translatePos(x,y); return (x<width-1 && y<height-1)?interpolateLinearPixel_Linear(x,y,rdPixelWrapper_G8::read(x,y),rdPixelWrapper_G8::read(x+1,y)):rdPixelWrapper_G8::read(x,y); }
};

class rdPixelWrapper_G8_Bilinear : public rdPixelWrapper_G8 {
public:
	virtual ~rdPixelWrapper_G8_Bilinear() {}
	virtual FLinearColor readLinearPixel(double x,double y) { if(translate) translatePos(x,y); return (x<width-1 && y<height-1)?interpolateLinearPixel_Bilinear(x,y,rdPixelWrapper_G8::read(x,y),rdPixelWrapper_G8::read(x+1.0,y),rdPixelWrapper_G8::read(x,y+1.0),rdPixelWrapper_G8::read(x+1.0,y+1.0)):rdPixelWrapper_G8::read(x,y); }
};

class rdPixelWrapper_G8_Bicubic : public rdPixelWrapper_G8 {
public:
	virtual ~rdPixelWrapper_G8_Bicubic() {}
#define rdpx rdPixelWrapper_G8::read
#define rdpxs rdpx(x-1.0,y-1.0),rdpx(x,y-1.0),rdpx(x+1.0,y-1.0),rdpx(x+2.0,y-1.0),rdpx(x-1.0,y),rdpx(x,y),rdpx(x+1.0,y),rdpx(x+2.0,y),rdpx(x-1.0,y+1.0),rdpx(x,y+1.0),rdpx(x+1.0,y+1.0),rdpx(x+2.0,y+1.0),rdpx(x-1.0,y+2.0),rdpx(x,y+2.0),rdpx(x+1.0,y+2.0),rdpx(x+2.0,y+2.0)
	virtual FLinearColor readLinearPixel(double x,double y) { if(translate) translatePos(x,y); return (x>0 && y>0 && x<width-2 && y<height-2)?interpolateLinearPixel_Bicubic(x,y,rdpxs):rdpx(x,y); }
#undef rdpxs
#undef rdpx
};

class rdPixelWrapper_G8_Average : public rdPixelWrapper_G8 {
public:
	virtual ~rdPixelWrapper_G8_Average() {}
	virtual FLinearColor readLinearPixel(double x,double y) { if(translate) translatePos(x,y); return (x<width-1 && y<height-1)?interpolateLinearPixel_Average(x,y,rdPixelWrapper_G8::read(x,y),rdPixelWrapper_G8::read(x+1.0,y),rdPixelWrapper_G8::read(x,y+1.0),rdPixelWrapper_G8::read(x+1.0,y+1.0)):rdPixelWrapper_G8::read(x,y); }
};

//-------------------------------------------------------------------------------------------------
class rdPixelWrapper_G16 : public rdPixelWrapper {
public:
	virtual ~rdPixelWrapper_G16() {}
	inline FLinearColor  read(double x,double y) { lcol.R=lcol.G=lcol.B=((uint16*)data)[(uint32)(((uint32)y*width)+x)]/65535.0; lcol.A=1.0; return lcol; }
	virtual FLinearColor readLinearPixel(double x,double y) { if(translate) translatePos(x,y); return read(x,y); }
	virtual FColor		 readPixel(double x,double y) { if(translate) translatePos(x,y); col.R=col.G=col.B=((uint16*)data)[(uint32)(((uint32)y*width)+x)]/256; col.A=255; return col; }
	virtual void		 writeLinearPixel(int16 x,int16 y,const FLinearColor& scol) { clampLinear(scol); ((uint16*)data)[(y*width)+x]=(uint16)(((lcol.R+lcol.G+lcol.B)/3.0)*65535.0); }
	virtual void		 writePixel(int16 x,int16 y,const FColor& scol) { ((uint16*)data)[(y*width)+x]=(uint16)((scol.R+scol.G+scol.B)/3)*256; }
	virtual void		 setAlpha(int16 x,int16 y,float alpha) {}
};

class rdPixelWrapper_G16_Linear : public rdPixelWrapper_G16 {
public:
	virtual ~rdPixelWrapper_G16_Linear() {}
	virtual FLinearColor readLinearPixel(double x,double y) { if(translate) translatePos(x,y); return (x<width-1 && y<height-1)?interpolateLinearPixel_Linear(x,y,rdPixelWrapper_G16::read(x,y),rdPixelWrapper_G16::read(x+1.0,y)):rdPixelWrapper_G16::read(x,y); }
};

class rdPixelWrapper_G16_Bilinear : public rdPixelWrapper_G16 {
public:
	virtual ~rdPixelWrapper_G16_Bilinear() {}
	virtual FLinearColor readLinearPixel(double x,double y) { if(translate) translatePos(x,y); return (x<width-1 && y<height-1)?interpolateLinearPixel_Bilinear(x,y,rdPixelWrapper_G16::read(x,y),rdPixelWrapper_G16::read(x+1.0,y),rdPixelWrapper_G16::read(x,y+1.0),rdPixelWrapper_G16::read(x+1.0,y+1.0)):rdPixelWrapper_G16::read(x,y); }
};

class rdPixelWrapper_G16_Bicubic : public rdPixelWrapper_G16 {
public:
	virtual ~rdPixelWrapper_G16_Bicubic() {}
#define rdpx rdPixelWrapper_G16::read
#define rdpxs rdpx(x-1.0,y-1.0),rdpx(x,y-1.0),rdpx(x+1.0,y-1.0),rdpx(x+2.0,y-1.0),rdpx(x-1.0,y),rdpx(x,y),rdpx(x+1.0,y),rdpx(x+2.0,y),rdpx(x-1.0,y+1.0),rdpx(x,y+1.0),rdpx(x+1.0,y+1.0),rdpx(x+2.0,y+1.0),rdpx(x-1.0,y+2.0),rdpx(x,y+2.0),rdpx(x+1.0,y+2.0),rdpx(x+2.0,y+2.0)
	virtual FLinearColor readLinearPixel(double x,double y) { if(translate) translatePos(x,y); return (x>0 && y>0 && x<width-2 && y<height-2)?interpolateLinearPixel_Bicubic(x,y,rdpxs):rdpx(x,y); }
#undef rdpxs
#undef rdpx
};

class rdPixelWrapper_G16_Average : public rdPixelWrapper_G16 {
public:
	virtual ~rdPixelWrapper_G16_Average() {}
	virtual FLinearColor readLinearPixel(double x,double y) { if(translate) translatePos(x,y); return (x<width-1 && y<height-1)?interpolateLinearPixel_Average(x,y,rdPixelWrapper_G16::read(x,y),rdPixelWrapper_G16::read(x+1.0,y),rdPixelWrapper_G16::read(x,y+1.0),rdPixelWrapper_G16::read(x+1.0,y+1.0)):rdPixelWrapper_G16::read(x,y); }
};

//-------------------------------------------------------------------------------------------------
class rdPixelWrapper_BGRA8 : public rdPixelWrapper {
public:
	virtual ~rdPixelWrapper_BGRA8() {}
	inline FLinearColor  read(double x,double y) { uint32 raw=((uint32*)data)[(uint32)(((uint32)y*width)+x)]; lcol.A=(raw>>24)/255.0; lcol.R=((raw>>16)&0xff)/255.0; lcol.G=((raw>>8)&0xff)/255.0; lcol.B=(raw&0xff)/255.0; return lcol; }
	virtual FLinearColor readLinearPixel(double x,double y) { if(translate) translatePos(x,y); return read(x,y); }
	virtual FColor		 readPixel(double x,double y) { if(translate) translatePos(x,y); uint32 raw=((uint32*)data)[(uint32)(((uint32)y*width)+x)]; col.A=(raw>>24); col.R=((raw>>16)&0xff); col.G=((raw>>8)&0xff); col.B=(raw&0xff); return col; }
	virtual void		 writeLinearPixel(int16 x,int16 y,const FLinearColor& scol) { clampLinear(scol); ((uint32*)data)[(y*width)+x]=((uint32)(lcol.A*255.0)<<24)|((uint32)(lcol.R*255.0)<<16)|((uint32)(lcol.G*255.0)<<8)|(uint32)(lcol.B*255.0); }
	virtual void		 writePixel(int16 x,int16 y,const FColor& scol) { ((uint32*)data)[(y*width)+x]=*((uint32*)&scol); }
	virtual void		 setAlpha(int16 x,int16 y,float alpha) { ((uint8*)data)[(y*width*4)+(x*4)+3]=(alpha*255.0f); }
};

class rdPixelWrapper_BGRA8_Linear : public rdPixelWrapper_BGRA8 {
public:
	virtual ~rdPixelWrapper_BGRA8_Linear() {}
	virtual FLinearColor readLinearPixel(double x,double y) { if(translate) translatePos(x,y); return (x<width-1 && y<height-1)?interpolateLinearPixel_Linear(x,y,rdPixelWrapper_BGRA8::read(x,y),rdPixelWrapper_BGRA8::read(x+1.0,y)):rdPixelWrapper_BGRA8::read(x,y); }
};

class rdPixelWrapper_BGRA8_Bilinear : public rdPixelWrapper_BGRA8 {
public:
	virtual ~rdPixelWrapper_BGRA8_Bilinear() {}
	virtual FLinearColor readLinearPixel(double x,double y) { if(translate) translatePos(x,y); return (x>0 && y>0 && x<width-2 && y<height-2)?interpolateLinearPixel_Bilinear(x,y,rdPixelWrapper_BGRA8::read(x,y),rdPixelWrapper_BGRA8::read(x+1.0,y),rdPixelWrapper_BGRA8::read(x,y+1.0),rdPixelWrapper_BGRA8::read(x+1.0,y+1.0)):rdPixelWrapper_BGRA8::read(x,y); }
};

class rdPixelWrapper_BGRA8_Bicubic : public rdPixelWrapper_BGRA8 {
public:
	virtual ~rdPixelWrapper_BGRA8_Bicubic() {}
#define rdpx rdPixelWrapper_BGRA8::read
#define rdpxs rdpx(x-1.0,y-1.0),rdpx(x,y-1.0),rdpx(x+1.0,y-1.0),rdpx(x+2.0,y-1.0),rdpx(x-1.0,y),rdpx(x,y),rdpx(x+1.0,y),rdpx(x+2.0,y),rdpx(x-1.0,y+1.0),rdpx(x,y+1.0),rdpx(x+1.0,y+1.0),rdpx(x+2.0,y+1.0),rdpx(x-1.0,y+2.0),rdpx(x,y+2.0),rdpx(x+1.0,y+2.0),rdpx(x+2.0,y+2.0)
	virtual FLinearColor readLinearPixel(double x,double y) { if(translate) translatePos(x,y); return (x>0 && y>0 && x<width-2 && y<height-2)?interpolateLinearPixel_Bicubic(x,y,rdpxs):rdpx(x,y); }
#undef rdpxs
#undef rdpx
};

class rdPixelWrapper_BGRA8_Average : public rdPixelWrapper_BGRA8 {
public:
	virtual ~rdPixelWrapper_BGRA8_Average() {}
	virtual FLinearColor readLinearPixel(double x,double y) { if(translate) translatePos(x,y); return (x>0 && y>0 && x<width-2 && y<height-2)?interpolateLinearPixel_Average(x,y,rdPixelWrapper_BGRA8::read(x,y),rdPixelWrapper_BGRA8::read(x+1.0,y),rdPixelWrapper_BGRA8::read(x,y+1.0),rdPixelWrapper_BGRA8::read(x+1.0,y+1.0)):rdPixelWrapper_BGRA8::read(x,y); }
};

//-------------------------------------------------------------------------------------------------
class rdPixelWrapper_BGRE8 : public rdPixelWrapper { // 8bit RGB with shared 8bit exponent in A (author is Gregory Ward for his Radiance routines)
public:
	virtual ~rdPixelWrapper_BGRE8() {}
	inline FLinearColor  read(double x,double y) { uint32 raw=((uint32*)data)[(uint32)(((uint32)y*width)+x)]; col.A=(raw>>24); col.R=((raw>>16)&0xff); col.G=((raw>>8)&0xff); col.B=(raw&0xff); lcol=col.FromRGBE(); return lcol; }
	virtual FLinearColor readLinearPixel(double x,double y) { if(translate) translatePos(x,y); return read(x,y); }
	virtual FColor		 readPixel(double x,double y) { if(translate) translatePos(x,y); uint32 raw=((uint32*)data)[(uint32)(((uint32)y*width)+x)]; col.A=(raw>>24); col.R=((raw>>16)&0xff); col.G=((raw>>8)&0xff); col.B=(raw&0xff); return col; }
	virtual void		 writeLinearPixel(int16 x,int16 y,const FLinearColor& scol) { col=scol.ToRGBE(); ((uint32*)data)[(y*width)+x]=*((uint32*)&col); }
	virtual void		 writePixel(int16 x,int16 y,const FColor& scol) { ((uint32*)data)[(y*width)+x]=*((uint32*)&scol); 	}
	virtual void		 setAlpha(int16 x,int16 y,float alpha) { ((uint8*)data)[(y*width*4)+(x*4)+3]=(alpha*255.0f);	}
};

class rdPixelWrapper_BGRE8_Linear : public rdPixelWrapper_BGRE8 {
public:
	virtual ~rdPixelWrapper_BGRE8_Linear() {}
	virtual FLinearColor readLinearPixel(double x,double y) { if(translate) translatePos(x,y); return (x<width-1 && y<height-1)?interpolateLinearPixel_Linear(x,y,rdPixelWrapper_BGRE8::read(x,y),rdPixelWrapper_BGRE8::read(x+1.0,y)):rdPixelWrapper_BGRE8::read(x,y); }
};

class rdPixelWrapper_BGRE8_Bilinear : public rdPixelWrapper_BGRE8 {
public:
	virtual ~rdPixelWrapper_BGRE8_Bilinear() {}
	virtual FLinearColor readLinearPixel(double x,double y) { if(translate) translatePos(x,y); return (x<width-1 && y<height-1)?interpolateLinearPixel_Bilinear(x,y,rdPixelWrapper_BGRE8::read(x,y),rdPixelWrapper_BGRE8::read(x+1.0,y),rdPixelWrapper_BGRE8::read(x,y+1.0),rdPixelWrapper_BGRE8::read(x+1.0,y+1.0)):rdPixelWrapper_BGRE8::read(x,y); }
};

class rdPixelWrapper_BGRE8_Bicubic : public rdPixelWrapper_BGRE8 {
public:
	virtual ~rdPixelWrapper_BGRE8_Bicubic() {}
#define rdpx rdPixelWrapper_BGRE8::read
#define rdpxs rdpx(x-1.0,y-1.0),rdpx(x,y-1.0),rdpx(x+1.0,y-1.0),rdpx(x+2.0,y-1.0),rdpx(x-1.0,y),rdpx(x,y),rdpx(x+1.0,y),rdpx(x+2.0,y),rdpx(x-1.0,y+1.0),rdpx(x,y+1.0),rdpx(x+1.0,y+1.0),rdpx(x+2.0,y+1.0),rdpx(x-1.0,y+2.0),rdpx(x,y+2.0),rdpx(x+1.0,y+2.0),rdpx(x+2.0,y+2.0)
	virtual FLinearColor readLinearPixel(double x,double y) { if(translate) translatePos(x,y); return (x>0 && y>0 && x<width-2 && y<height-2)?interpolateLinearPixel_Bicubic(x,y,rdpxs):rdpx(x,y); }
#undef rdpxs
#undef rdpx
};

class rdPixelWrapper_BGRE8_Average : public rdPixelWrapper_BGRE8 {
public:
	virtual ~rdPixelWrapper_BGRE8_Average() {}
	virtual FLinearColor readLinearPixel(double x,double y) { if(translate) translatePos(x,y); return (x<width-1 && y<height-1)?interpolateLinearPixel_Average(x,y,rdPixelWrapper_BGRE8::read(x,y),rdPixelWrapper_BGRE8::read(x+1.0,y),rdPixelWrapper_BGRE8::read(x,y+1.0),rdPixelWrapper_BGRE8::read(x+1.0,y+1.0)):rdPixelWrapper_BGRE8::read(x,y); }
};

//-------------------------------------------------------------------------------------------------
class rdPixelWrapper_RGBA16 : public rdPixelWrapper { // Note: actually stored as ABGR
public:
	virtual ~rdPixelWrapper_RGBA16() {}
	inline FLinearColor  read(double x,double y) { uint64 raw=((uint64*)data)[(uint32)(((uint32)y*width)+x)]; lcol.A=(raw>>48)/65535.0; lcol.B=((raw>>32)&0xffff)/65535.0; lcol.G=((raw>>16)&0xffff)/65535.0; lcol.R=(raw&0xffff)/65535.0; return lcol; }
	virtual FLinearColor readLinearPixel(double x,double y) { if(translate) translatePos(x,y); return read(x,y); }
	virtual FColor		 readPixel(double x,double y) { if(translate) translatePos(x,y); uint64 raw=((uint64*)data)[(uint32)(((uint32)y*width)+x)]; col.A=(raw>>48)/255; col.R=((raw>>32)&0xffff)/255; col.G=((raw>>16)&0xffff)/255; col.B=(raw&0xffff)/255; return col; }
	virtual void		 writeLinearPixel(int16 x,int16 y,const FLinearColor& scol) { clampLinear(scol); uint64 icol=(uint64)(lcol.A*65535.0)<<48|(uint64)(lcol.B*65535.0)<<32|(uint64)(lcol.G*65535.0)<<16|(uint64)(lcol.R*65535.0); ((uint64*)data)[(y*width)+x]=icol; }
	virtual void		 writePixel(int16 x,int16 y,const FColor& scol) { uint64 icol=(uint64)(scol.A*256.0)<<48|(uint64)(scol.B*255.0)<<32|(uint64)(scol.G*255.0)<<16|(uint64)(scol.R*255.0);((uint64*)data)[(y*width)+x]=icol; }
	virtual void		 setAlpha(int16 x,int16 y,float alpha) { uint64 icol=((uint64*)data)[(uint32)(((uint32)y*width)+x)]&0xFFFFFFFFFFFF0000;icol|=((uint64)(alpha*65535.0f));((uint64*)data)[(y*width)+x]=icol; }
};

class rdPixelWrapper_RGBA16_Linear : public rdPixelWrapper_RGBA16 {
public:
	virtual ~rdPixelWrapper_RGBA16_Linear() {}
	virtual FLinearColor readLinearPixel(double x,double y) { if(translate) translatePos(x,y); return (x<width-1 && y<height-1)?interpolateLinearPixel_Linear(x,y,rdPixelWrapper_RGBA16::read(x,y),rdPixelWrapper_RGBA16::read(x+1.0,y)):rdPixelWrapper_RGBA16::read(x,y); }
};

class rdPixelWrapper_RGBA16_Bilinear : public rdPixelWrapper_RGBA16 {
public:
	virtual ~rdPixelWrapper_RGBA16_Bilinear() {}
	virtual FLinearColor readLinearPixel(double x,double y) { if(translate) translatePos(x,y); return (x<width-1 && y<height-1)?interpolateLinearPixel_Bilinear(x,y,rdPixelWrapper_RGBA16::read(x,y),rdPixelWrapper_RGBA16::read(x+1.0,y),rdPixelWrapper_RGBA16::read(x,y+1.0),rdPixelWrapper_RGBA16::read(x+1.0,y+1.0)):rdPixelWrapper_RGBA16::read(x,y); }
};

class rdPixelWrapper_RGBA16_Bicubic : public rdPixelWrapper_RGBA16 {
public:
	virtual ~rdPixelWrapper_RGBA16_Bicubic() {}
#define rdpx rdPixelWrapper_RGBA16::read
#define rdpxs rdpx(x-1.0,y-1.0),rdpx(x,y-1.0),rdpx(x+1.0,y-1.0),rdpx(x+2.0,y-1.0),rdpx(x-1.0,y),rdpx(x,y),rdpx(x+1.0,y),rdpx(x+2.0,y),rdpx(x-1.0,y+1.0),rdpx(x,y+1.0),rdpx(x+1.0,y+1.0),rdpx(x+2.0,y+1.0),rdpx(x-1.0,y+2.0),rdpx(x,y+2.0),rdpx(x+1.0,y+2.0),rdpx(x+2.0,y+2.0)
	virtual FLinearColor readLinearPixel(double x,double y) { if(translate) translatePos(x,y); return (x>0 && y>0 && x<width-2 && y<height-2)?interpolateLinearPixel_Bicubic(x,y,rdpxs):rdpx(x,y); }
#undef rdpxs
#undef rdpx
};

class rdPixelWrapper_RGBA16_Average : public rdPixelWrapper_RGBA16 {
public:
	virtual ~rdPixelWrapper_RGBA16_Average() {}
	virtual FLinearColor readLinearPixel(double x,double y) { if(translate) translatePos(x,y); return (x<width-1 && y<height-1)?interpolateLinearPixel_Average(x,y,rdPixelWrapper_RGBA16::read(x,y),rdPixelWrapper_RGBA16::read(x+1.0,y),rdPixelWrapper_RGBA16::read(x,y+1.0),rdPixelWrapper_RGBA16::read(x+1.0,y+1.0)):rdPixelWrapper_RGBA16::read(x,y); }
};

//-------------------------------------------------------------------------------------------------
class rdPixelWrapper_RGBA16F : public rdPixelWrapper { // HDR (16bit floats from formats such as EXR)
public:
	virtual ~rdPixelWrapper_RGBA16F() {}
	inline FLinearColor  read(double x,double y) { FFloat16Color& rgb=((FFloat16Color*)data)[(uint32)(((uint32)y*width)+x)]; FLinearColor lc(rgb); lcol=lc; return lcol; }
	virtual FLinearColor readLinearPixel(double x,double y) { if(translate) translatePos(x,y); return read(x,y); }
	virtual FColor		 readPixel(double x,double y) { if(translate) translatePos(x,y); FFloat16Color& rgb=((FFloat16Color*)data)[(uint32)(((uint32)y*width)+x)]; FLinearColor lc(rgb); col.R=lc.R*255.0; col.G=lc.G*255.0; col.B=lc.B*255.0; col.A=lc.A*255.0; return col; }
	virtual void		 writeLinearPixel(int16 x,int16 y,const FLinearColor& scol) { FFloat16Color& rgb=((FFloat16Color*)data)[(y*width)+x]; rgb=FFloat16Color(scol); }
	virtual void		 writePixel(int16 x,int16 y,const FColor& scol) { FFloat16Color& rgb=((FFloat16Color*)data)[(y*width)+x]; rgb=FFloat16Color(scol); }
	virtual void		 setAlpha(int16 x,int16 y,float alpha) { FFloat16Color& rgb=((FFloat16Color*)data)[(y*width)+x]; rgb.A=alpha; }
};

class rdPixelWrapper_RGBA16F_Linear : public rdPixelWrapper_RGBA16F {
public:
	virtual ~rdPixelWrapper_RGBA16F_Linear() {}
	virtual FLinearColor readLinearPixel(double x,double y) { if(translate) translatePos(x,y); return (x<width-1.0f && y<height-1.0f)?interpolateLinearPixel_Linear(x,y,rdPixelWrapper_RGBA16F::read(x,y),rdPixelWrapper_RGBA16F::read(x+1.0f,y)):rdPixelWrapper_RGBA16F::read(x,y); }
};

class rdPixelWrapper_RGBA16F_Bilinear : public rdPixelWrapper_RGBA16F {
public:
	virtual ~rdPixelWrapper_RGBA16F_Bilinear() {}
	virtual FLinearColor readLinearPixel(double x,double y) { if(translate) translatePos(x,y); return (x<width-1 && y<height-1)?interpolateLinearPixel_Bilinear(x,y,rdPixelWrapper_RGBA16F::read(x,y),rdPixelWrapper_RGBA16F::read(x+1.0f,y),rdPixelWrapper_RGBA16F::read(x,y+1.0f),rdPixelWrapper_RGBA16F::read(x+1.0f,y+1.0f)):rdPixelWrapper_RGBA16F::read(x,y); }
};

class rdPixelWrapper_RGBA16F_Bicubic : public rdPixelWrapper_RGBA16F {
public:
	virtual ~rdPixelWrapper_RGBA16F_Bicubic() {}
#define rdpx rdPixelWrapper_RGBA16F::read
#define rdpxs rdpx(x-1.0,y-1.0),rdpx(x,y-1.0),rdpx(x+1.0,y-1.0),rdpx(x+2.0,y-1.0),rdpx(x-1.0,y),rdpx(x,y),rdpx(x+1.0f,y),rdpx(x+2.0f,y),rdpx(x-1.0f,y+1.0f),rdpx(x,y+1.0f),rdpx(x+1.0f,y+1.0f),rdpx(x+2.0f,y+1.0f),rdpx(x-1.0f,y+2.0f),rdpx(x,y+2.0f),rdpx(x+1.0f,y+2.0f),rdpx(x+2.0f,y+2.0f)
	virtual FLinearColor readLinearPixel(double x,double y) { if(translate) translatePos(x,y); return (x>0.0f && y>0.0f && x<width-2.0f && y<height-2.0f)?interpolateLinearPixel_Bicubic(x,y,rdpxs):rdpx(x,y); }
#undef rdpxs
#undef rdpx
};

class rdPixelWrapper_RGBA16F_Average : public rdPixelWrapper_RGBA16F {
public:
	virtual ~rdPixelWrapper_RGBA16F_Average() {}
	virtual FLinearColor readLinearPixel(double x,double y) { if(translate) translatePos(x,y); return (x<width-1 && y<height-1)?interpolateLinearPixel_Average(x,y,rdPixelWrapper_RGBA16F::read(x,y),rdPixelWrapper_RGBA16F::read(x+1.0f,y),rdPixelWrapper_RGBA16F::read(x,y+1.0f),rdPixelWrapper_RGBA16F::read(x+1.0f,y+1.0f)):rdPixelWrapper_RGBA16F::read(x,y); }
};

//-------------------------------------------------------------------------------------------------
class rdPixelWrapper_RGBA32F : public rdPixelWrapper {
public:
	virtual ~rdPixelWrapper_RGBA32F() {}
	//  define the read method as inline so their derived interpolation classes can inline them...
	inline FLinearColor  read(double x,double y) { lcol=((FLinearColor*)data)[(uint32)(((uint32)y*width)+x)]; return lcol; }
	virtual FLinearColor readLinearPixel(double x,double y) { if(translate) translatePos(x,y); return read(x,y); }
	virtual FColor		 readPixel(double x,double y) { if(translate) translatePos(x,y); lcol=((FLinearColor*)data)[(uint32)(((uint32)y*width)+x)]; return lcol.ToFColor(true); }
	virtual void		 writeLinearPixel(int16 x,int16 y,const FLinearColor& scol) { 
		((FLinearColor*)data)[(y*width)+x]=scol; 
	}
	virtual void		 writePixel(int16 x,int16 y,const FColor& scol) { lcol=scol; ((FLinearColor*)data)[(y*width)+x]=lcol; }
	virtual void		 setAlpha(int16 x,int16 y,float alpha) { FLinearColor& rgb=((FLinearColor*)data)[(y*width)+x]; rgb.A=alpha; }
};

class rdPixelWrapper_RGBA32F_Linear : public rdPixelWrapper_RGBA32F {
public:
	virtual ~rdPixelWrapper_RGBA32F_Linear() {}
	virtual FLinearColor readLinearPixel(double x,double y) { if(translate) translatePos(x,y); return (x<width-1 && y<height-1)?interpolateLinearPixel_Linear(x,y,rdPixelWrapper_RGBA32F::read(x,y),rdPixelWrapper_RGBA32F::read(x+1,y)):rdPixelWrapper_RGBA32F::read(x,y); }
};

class rdPixelWrapper_RGBA32F_Bilinear : public rdPixelWrapper_RGBA32F {
public:
	virtual ~rdPixelWrapper_RGBA32F_Bilinear() {}
	virtual FLinearColor readLinearPixel(double x,double y) { if(translate) translatePos(x,y); return (x<width-1 && y<height-1)?interpolateLinearPixel_Bilinear(x,y,rdPixelWrapper_RGBA32F::read(x,y),rdPixelWrapper_RGBA32F::read(x+1.0,y),rdPixelWrapper_RGBA32F::read(x,y+1.0),rdPixelWrapper_RGBA32F::read(x+1.0,y+1.0)):rdPixelWrapper_RGBA32F::read(x,y); }
};

class rdPixelWrapper_RGBA32F_Bicubic : public rdPixelWrapper_RGBA32F {
public:
	virtual ~rdPixelWrapper_RGBA32F_Bicubic() {}
#define rdpx rdPixelWrapper_RGBA32F::read
#define rdpxs rdpx(x-1.0,y-1.0),rdpx(x,y-1.0),rdpx(x+1.0,y-1.0),rdpx(x+2.0,y-1.0),rdpx(x-1.0,y),rdpx(x,y),rdpx(x+1.0,y),rdpx(x+2.0,y),rdpx(x-1.0,y+1.0),rdpx(x,y+1.0),rdpx(x+1.0,y+1.0),rdpx(x+2.0,y+1.0),rdpx(x-1.0,y+2.0),rdpx(x,y+2.0),rdpx(x+1.0,y+2.0),rdpx(x+2.0,y+2.0)
	virtual FLinearColor readLinearPixel(double x,double y) { if(translate) translatePos(x,y); return (x>0 && y>0 && x<width-2 && y<height-2)?interpolateLinearPixel_Bicubic(x,y,rdpxs):rdPixelWrapper_RGBA32F::read(x,y); }
#undef rdpxs
#undef rdpx
};

class rdPixelWrapper_RGBA32F_Average : public rdPixelWrapper_RGBA32F {
public:
	virtual ~rdPixelWrapper_RGBA32F_Average() {}
	virtual FLinearColor readLinearPixel(double x,double y) { if(translate) translatePos(x,y); return (x<width-1 && y<height-1)?interpolateLinearPixel_Average(x,y,rdPixelWrapper_RGBA32F::read(x,y),rdPixelWrapper_RGBA32F::read(x+1.0,y),rdPixelWrapper_RGBA32F::read(x,y+1.0),rdPixelWrapper_RGBA32F::read(x+1.0,y+1.0)):rdPixelWrapper_RGBA32F::read(x,y); }
};

//-------------------------------------------------------------------------------------------------
