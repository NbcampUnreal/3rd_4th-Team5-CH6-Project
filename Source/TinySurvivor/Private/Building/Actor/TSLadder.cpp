// Fill out your copyright notice in the Description page of Project Settings.


#include "Building/Actor/TSLadder.h"


// Sets default values
ATSLadder::ATSLadder()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	MeshComponent->SetSimulatePhysics(true);
	MeshComponent->SetCollisionProfileName("PhysicsActor");
	// 무게 설정
	MeshComponent->SetMassOverrideInKg(NAME_None, 5000.0f, true);
	// 감쇠 높이기 - 움직임이 빨리 멈춤
	MeshComponent->SetLinearDamping(5.0f); // 기본 0.01, 높을수록 빨리 정지
	MeshComponent->SetAngularDamping(5.0f); 
	Tags.Add("Climbable");
}

