// rdPools.h - Copyright (c) 2022 Recourse Design ltd.
//
// See rdInstBPLibrary.cpp for main Documentation
//
#pragma once

#include "rdInstBPLibrary.h"

#define RDINST_PLUGIN_API DLLEXPORT

// This is a simple class with an array of actors for pools
class rdPoolItem {
public:
	rdPoolItem() {}
	rdPoolItem(const rdPoolItem& other) : actor(other.actor),comp(other.comp),inUse(other.inUse),instCnt(other.instCnt),hasTick(other.hasTick),startHidden(other.startHidden),hasCollision(other.hasCollision),hasPhysics(other.hasPhysics) {}
	AActor*			 actor=nullptr;
	UActorComponent* comp=nullptr;
	bool			 inUse=false;
	uint64			 instCnt=0;
	bool			 hasTick=false;
	bool			 startHidden=false;
	bool			 hasCollision=false;
	TArray<bool>	 hasPhysics;
};

class rdPoolData {
public:
	rdPoolData(int32 num,bool rl,bool s,bool pm,bool dt,bool sh,int32 gb) : numPooled(num),reuseLast(rl),simple(s),premake(pm),doTick(dt),startHidden(sh),growBy(gb) {}
	rdPoolData(const rdPoolData& o) : numPooled(o.numPooled),pool(o.pool),reuseLast(o.reuseLast),simple(o.simple),premake(o.premake),doTick(o.doTick),startHidden(o.startHidden),growBy(o.growBy) {}

	int32				numPooled=0;
	uint64				currInstCnt=0;
	TArray<rdPoolItem>	pool;
	bool				reuseLast=true;
	bool				simple=false;
	bool				premake=true;
	bool				doTick=false;
	bool				startHidden=false;
	int32				growBy=0;
};
