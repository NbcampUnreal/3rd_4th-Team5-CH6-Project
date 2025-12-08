// GE_ArmorCommonStats_Base.h
#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "GE_ArmorCommonStats_Base.generated.h"

/*
	방어구 장착 시 공통 스탯 적용을 위한 베이스 GameplayEffect
	
	■ 사용 목적:
	- 모든 방어구의 기본 스탯(체력 보너스)을 적용하기 위한 공통 GE
	- FItemData.ArmorData.HealthBonus 값을 MaxHealth Attribute에 가산 적용
	
	■ 사용 방법:
	1. 블루프린트에서 이 클래스를 상속받아 BP_GE_ArmorCommonStats 생성
	2. UTSInventoryMasterComponent의 ArmorCommonStatEffectClass에 BP 할당
	3. EquipArmor() 함수에서 자동으로 적용됨:
		- SetByCallerMagnitude로 HealthBonus 값 전달
		- ASC->ApplyGameplayEffectSpecToSelf()로 적용
		- FEquippedArmor.CommonEffectHandle에 핸들 저장
	
	■ 동작 흐름:
	[장착 시]
	1. EquipArmor() 호출
	2. GE Spec 생성 후 SetByCaller로 HealthBonus 값 설정
	3. ASC에 적용 → MaxHealth 증가
	4. CommonEffectHandle 저장
	
	[탈착 시]
	1. UnequipArmor() 호출
	2. CommonEffectHandle로 GE 제거
	3. MaxHealth 원상복구
	
	■ SetByCaller Tag:
	- "Data.HealthBonus": 방어구의 체력 보너스 값 (ItemInfo.ArmorData.HealthBonus에서 전달)
	
	■ Duration:
	- Infinite (장착 중 계속 유지)
	- 장비 교체 시 기존 GE 제거 후 새 GE 적용
	
	■ 네트워크:
	- 서버 권한에서만 적용 (GetOwner()->HasAuthority() 체크)
	- Attribute 변화는 GAS 시스템에 의해 자동 복제
	
	■ 연관 시스템:
	- UTSInventoryMasterComponent::EquipArmor()
	- UTSInventoryMasterComponent::UnequipArmor()
	- FEquippedArmor::CommonEffectHandle
	- UTSAttributeSet::MaxHealth
*/
UCLASS(Abstract, Blueprintable)
class TINYSURVIVOR_API UGE_ArmorCommonStats_Base : public UGameplayEffect
{
	GENERATED_BODY()
	
public:
	UGE_ArmorCommonStats_Base();
};