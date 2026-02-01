// GE_RestoreAttribute_Base.h
#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "GE_RestoreAttribute_Base.generated.h"

/*
	소모품 아이템의 스탯 회복을 위한 베이스 GameplayEffect
	
	사용 방법:
	1. 이 클래스를 부모로 하는 블루프린트 생성
	2. GA_ConsumeItem_Base의 RestoreEffectClass에 블루프린트 클래스 설정
	
	동작 흐름:
	1. GA에서 SetByCaller로 ItemID 수신
	2. UGEC_RestoreAttribute에서 ItemID로 아이템 정보 조회
	3. EffectTag 기반으로 적용할 Attribute 자동 결정
		- HUNGER_RESTORE → Hunger 증가
		- THIRST_RESTORE → Thirst 증가
		- HP_HEAL → Health 증가
		- SANITY_RESTORE → Sanity 증가
		- TEMP_ADJUST → Temperature 증가
		- HP_DAMAGE → Health 감소
	4. EffectValue만큼 Attribute 변경
	
	Duration:
	- EffectDuration == 0 → Instant
	- 버프형은 별도 GE로 처리
	
	네트워크:
	- Execution은 서버에서만 실행
	- Attribute 변경은 자동으로 복제됨
*/
UCLASS(Abstract, Blueprintable)
class TINYSURVIVOR_API UGE_RestoreAttribute_Base : public UGameplayEffect
{
	GENERATED_BODY()

public:
	UGE_RestoreAttribute_Base();
};