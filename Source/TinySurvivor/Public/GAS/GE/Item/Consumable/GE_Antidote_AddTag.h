// GE_Antidote_AddTag.h
#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "GE_Antidote_AddTag.generated.h"

/*
	해독제 전용 AddTag 용도 GameplayEffect
	
	사용 방법:
	1. 이 클래스를 부모로 하는 블루프린트 생성
	2. 블루프린트에서 Gameplay Effect 설정:
		- Gamplay Effect/Components/TargetTags(GrantedtoActor)/AddTags/
		- Add to Inherited: State.Status.Immune.Poison
		- Remove from Inherited: State.Status.Poison
	3. Duration Magnitude: Scalable Float (60.0)
	4. GA에 매핑
	4. DT_Item의 해당 아이템에 AbilityBP 매핑 확인
	
	동작 흐름:
	1. Duration 동안 버프 적용
	3. Duration 종료 시 자동으로 버프 해제 (Attribute 원상복구)
	
	특징:
	- Duration 정책: HasDuration
	- ItemID 자동 매핑 불가 → 아이템별 블루프린트 생성 필요
	
	네트워크:
	- Duration 효과는 서버에서 관리
	- Attribute 변경은 자동으로 복제됨
*/
UCLASS()
class TINYSURVIVOR_API UGE_Antidote_AddTag : public UGameplayEffect
{
	GENERATED_BODY()
	
public:
	UGE_Antidote_AddTag();
};
