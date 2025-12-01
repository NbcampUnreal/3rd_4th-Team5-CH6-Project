// GE_WeaponStats_Base.h
#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "GE_WeaponStats_Base.generated.h"

/*
	무기/도구 장착 시 스탯 적용을 위한 베이스 GameplayEffect
	
	사용 방법:
	1. 이 클래스를 부모로 하는 블루프린트 생성 (BP_GE_WeaponStats)
	2. 블루프린트에서 Modifiers 설정:
		- DamageBonus = SetByCaller[Data.AttackDamage]
		- AttackSpeedBonus = SetByCaller[Data.AttackSpeed]
		- AttackRangeBonus = SetByCaller[Data.AttackRange]
	3. UTSInventoryMasterComponent의 WeaponStatEffectClass에 블루프린트 할당
	
	동작 흐름:
	1. 무기 장착 → ApplyWeaponStats 호출
	2. ItemInfo에서 무기 스탯 값 추출
	3. SetByCaller로 3가지 값 전달
	4. GE 적용 → Bonus 속성에 더해짐
	5. 무기 해제 → GE 제거 → Bonus 자동 복구
	
	Duration:
	- Infinite (장착 중 계속 유지)
	
	네트워크:
	- 서버에서만 적용
	- Attribute 변경은 자동 복제
*/
UCLASS(Abstract, Blueprintable)
class TINYSURVIVOR_API UGE_WeaponStats_Base : public UGameplayEffect
{
	GENERATED_BODY()
	
public:
	UGE_WeaponStats_Base();
};
