// rdSpawn.h - Copyright (c) 2022 Recourse Design ltd.
//
// See rdInstBPLibrary.cpp for main Documentation
//
#pragma once

UENUM()
enum rdSpawnMode {
	// use the default SpawnMode for this mesh/actor
	RDSPAWNMODE_DEFAULT			UMETA(DisplayName="Default"),		
	// doesn't spawn anything
	RDSPAWNMODE_NONE			UMETA(DisplayName="None"),		
	// Spawns all objects in the SpawnActor
	RDSPAWNMODE_ALL				UMETA(DisplayName="All"),	
	// Spawns objects within a specified sphere surrounding the player, populating areas revealed from the last movement
	RDSPAWNMODE_SDP	UMETA(DisplayName="Spherical Difference Population"),	
	// Spawns objects within the View Frustum, populating areas revealed from the last movement
	RDSPAWNMODE_FDP		UMETA(DisplayName="Frustum Difference Population"),	
};
