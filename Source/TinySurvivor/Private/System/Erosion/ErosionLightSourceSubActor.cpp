// Fill out your copyright notice in the Description page of Project Settings.

#include "System/Erosion/ErosionLightSourceSubActor.h"
#include "System/Erosion/ErosionLightSourceComponent.h"

AErosionLightSourceSubActor::AErosionLightSourceSubActor()
{
	PrimaryActorTick.bCanEverTick = false;
	ErosionLightSourceComponent = CreateDefaultSubobject<UErosionLightSourceComponent>(TEXT("ErosionLightSourceComponent"));
}

