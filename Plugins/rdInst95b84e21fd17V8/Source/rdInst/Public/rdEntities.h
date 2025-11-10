// rdActor.h - Copyright (c) 2025 Recourse Design ltd.
//
// See rdInstBPLibrary.cpp for main Documentation
//
#pragma once

#include "rdInstBaseActor.h"
#include "rdEntities.generated.h"

#define RDINST_PLUGIN_API DLLEXPORT

struct rdGridInfo {
	TArray<TTuple<int32,FTransform>>* moveArray;
	TArray<FTransform>* addArray;
	TArray<AActor*>* destroyArray;
	FrdProxyInstanceItems* proxyItems=nullptr;
	bool hasProxy;
	bool hasMovement;
	bool isISM;
	FVector loc;
	FrdProxyInstanceItems* pim=nullptr;
	float proxySwapDistance;
};

UCLASS()
class UrdEntityMovementBase : public UObject {
	GENERATED_BODY()
public:
	virtual bool hasMovement() { return false; }
	virtual void move() {}
};

UCLASS()
class UrdEntityMovementAnimalBase : public UrdEntityMovementBase {
	GENERATED_BODY()
public:
	virtual void move() override {}
};


UCLASS()
class UrdEntityMovementVehicleBase : public UrdEntityMovementBase {
	GENERATED_BODY()
public:
	virtual void move() override {}
};


UCLASS()
class UrdEntityMovementNPCBase : public UrdEntityMovementBase {
	GENERATED_BODY()
public:
	virtual void move() override {}
};
