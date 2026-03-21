#pragma once

/*

0. TODO : 에셋 폴더 정리 (50%)
0. TODO : CPP 폴더 정리 (50%)
0. TODO : 콘텐츠 폴더 정리 (50%)

1. TODO : 코드 컨벤션 정하기 -完-
2. TODO : 단일화 아키텍처 구조 설계 -完-
3. TODO : C++ 및 블루프린트 다이어트
4. TODO : 과도한 체인 구조 다이어트.        
5. TODO : rdinst 시스템 제거 
6. TODO : 5.7버전 업그레이드 - 完-
7. TODO: 스팀 플러그인 5.7버전으로 교체 - 完-

 */

// cpp 폴더 정리 하위 투트 리스트

/*
 
 
TODO : A_FOR_COMMON 파일 분석
TODO : A_FOR_COMMON 코드 룰, 네이밍 룰에 맞게 수정
TODO : A_FOR_COMMON 아키텍처 룰에 맞게 수정

- GameInstance 파일 진행 현황 :
			-- TSGameInstance.h : 
    
- GameplayMessage 파일 진행 현황 :
			-- Actor 파일 진행 상황 :
				-- Test 파일 진행 상황 : 
					-- GameplayMessageTestReceiveActor.h : 
					-- GameplayMessageTestSendActor.h : 
			-- Data 파일 진행 상황 :
				-- Struct 파일 진행 상황 :
					-- TSGameplayMessageData.h :
			

- Tag 파일 진행 현황 :
			-- Test 파일 진행 상황 :
				-- GameplayMessageTestTag.h :  
	-- AbilityGameplayTags.h :
	-- ErosionTag.h : 
	-- GameplayCueTags.h : 
	-- GameplayTagDisplayData.h : 
	-- GiantResoucreAreaIDTag.h : 
	-- InteractTag.h : 
	-- ItemGameplayTags.h :
	-- MonsterTag.h :
	-- NofiticationTags.h : 
	-- TSGiantStateTreeSendTag.h :
	-- TSReourceUniqueTag.h : 
	-- TSResourceSectorTag.h :

- Voice 파일 진행 현황 : 
	-- VOIPLIB.h :
*/

//----------------------------------------------------
 
/*
TODO : A_FOR_INGAME 파일 분석
TODO : A_FOR_INGAME 코드 룰, 네이밍 룰에 맞게 수정
TODO : A_FOR_INGAME 아키텍처 룰에 맞게 수정

- Framework 파일 진행 현황 :
		-- GameMode 파일 진행 상황 :
		-- GameState 파일 진행 상황 :

- Input 파일 진행 현황 :
	-- TSPlayerInputDataAsset.h

- SECTION_AI 파일 진행 현황 :
		-- Gaint 파일 진행 상황 : 
		-- Monster 파일 진행 상황 : 

- SECTION_GAS 파일 진행 현황 : 
		-- AbilityManager 파일 진행 상황 : 
		-- AttributeSet 파일 진행 상황 : 
		-- BaseAbility 파일 진행 상황 : 
		-- DataAsset 파일 진행 상황 : 
		-- GA 파일 진행 상황 : 
		-- GC 파일 진행 상황 : 
				GC_ElectricShock_Material.h : -完-
		-- GE 파일 진행 상황 : 

- SECTION_GIMMICK 파일 진행 현황 :
	-- GimmickZoneBase.h :

- SECTION_ITEM 파일 진행 현황 :
		-- Building 파일 진행 상황 : 
		-- Crafting 파일 진행 상황 : 
		-- Inventory 파일 진행 상황 : 
				TSInventoryMasterComponent.h : 
		-- Item 파일 진행 상황 : 

- SECTION_PLAYER 파일 진행 현황 :
		-- Character 파일 진행 상황 : 
		-- Controller 파일 진행 상황 : 
		-- EmoteSystem 파일 진행 상황 : 
		-- PingSystem 파일 진행 상황 : 
		-- PlayerState 파일 진행 상황 : 

- SECTION_UI 파일 진행 현황 : 
		-- Interface 파일 진행 상황 : 
		-- StatDisplay 파일 진행 상황 : 
		-- TagDisplay 파일 진행 상황 : 
	--	TSPlayerUIDataControllerSystem.h : 

- SECTION_WORLD 파일 진행 현황 :
		-- Erosion 파일 진행 상황 : 
		-- ResourceControl 파일 진행 상황 : 
		-- Time 파일 진행 상황 : 

- Sound 파일 진행 현황 :
		-- FootSetp 파일 진행 상황 : 
			-- AN_Climbing.h :
			-- AN_Footstep.h : 
			-- FootstepComponent.h	
		-- Hit 파일 진행 상황 : 
			-- HitComponent.h : 
	-- SoundComponent.h : 

*/

//----------------------------------------------------

/*
TODO : Z_NoUse 파일 분석
TODO : Z_NoUse 코드 룰, 네이밍 룰에 맞게 수정
TODO : Z_NoUse 아키텍처 룰에 맞게 수정

- Spawner 파일 진행 현황 :
	-- Data 파일 진행 상황 :
		-- SpawnerData.h :
		-- SpawnerTableAsset.h :
	-- System 파일 진행 상황 :  
		-- SpawnerDataSubsystem.h :
		-- SpawnerSystemSettings.h : 
		
*/