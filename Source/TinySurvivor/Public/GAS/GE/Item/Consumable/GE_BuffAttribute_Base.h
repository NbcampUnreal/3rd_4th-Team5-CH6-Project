// GE_BuffAttribute_Base.h
#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "GE_BuffAttribute_Base.generated.h"

/*
	버프형 소모품 아이템을 위한 베이스 GameplayEffect
	
	사용 방법:
	1. 이 클래스를 부모로 하는 블루프린트 생성
	2. 블루프린트에서 Modifiers 설정:
		- Attribute: Temperature
		- ModifierOp: Add
		- Modifier Magnitude: Scalable Float (10.0)
	3. Duration Magnitude: Scalable Float (180.0)
	4. DT_Item의 해당 아이템에 AbilityBP 매핑
	
	동작 흐름:
	1. ConsumptionTime 동안 애니메이션 재생
	2. Duration 동안 Attribute에 버프 적용
	3. Duration 종료 시 자동으로 버프 해제 (Attribute 원상복구)
	
	특징:
	- Duration 정책: HasDuration
	- Modifier 사용: Attribute 직접 수정
	- ItemID 자동 매핑 불가 → 아이템별 블루프린트 생성 필요
	
	네트워크:
	- Duration 효과는 서버에서 관리
	- Attribute 변경은 자동으로 복제됨
	
	주의사항:
	- TEMP_ADJUST 같은 양방향 효과는 음수 값으로 처리
		예:	핫초코(+10) → Modifier: +10
			아이스크림(-10) → Modifier: -10
*/
UCLASS(Abstract, Blueprintable)
class TINYSURVIVOR_API UGE_BuffAttribute_Base : public UGameplayEffect
{
	GENERATED_BODY()
	
public:
	UGE_BuffAttribute_Base();
};
