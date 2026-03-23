// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "TSGA_BaseAbility.generated.h"

/**
 * 
 */
UCLASS()
class TINYSURVIVOR_API UTSGA_BaseAbility : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UTSGA_BaseAbility();
};

/**
 * [GAS Gameplay Ability Configuration Full Guide]
 * * 1. Tag Interactions & Conditions (태그 상호작용 및 활성화 조건)
 * ----------------------------------------------------------------------------------
 * - AbilityTags             : 이 어빌리티를 식별하는 고유 태그 (예: Ability.Skill.Fireball)
 * - CancelAbilitiesWithTag  : 실행 시, 해당 태그를 가진 '활성 상태'의 다른 능력들을 강제 종료
 * - BlockAbilitiesWithTag   : 실행 중인 동안, 해당 태그를 가진 다른 능력의 '발동'을 차단
 * - ActivationOwnedTags     : 실행 중인 동안 시전자(Owner)에게 일시적으로 부여되는 태그
 * - ActivationRequiredTags  : 시전자가 이 태그들을 '모두' 소유해야만 활성화 가능
 * - ActivationBlockedTags   : 시전자가 이 태그 중 '하나라도' 소유하면 활성화 불가
 * - SourceRequiredTags      : 시전자(나)가 특정 상태(태그)여야 함 (예: 은신 중 등)
 * - SourceBlockedTags       : 시전자(나)가 특정 상태(태그)면 발동 불가 (예: 침묵 등)
 * - TargetRequiredTags      : 대상(상대)이 특정 상태(태그)여야 함 (예: 기절한 적 등)
 * - TargetBlockedTags       : 대상(상대)이 특정 상태(태그)면 발동 불가 (예: 무적 등)
 *
 * 2. Instancing Policy (인스턴스 생성 규칙)
 * ----------------------------------------------------------------------------------
 * - NonInstanced            : 인스턴스 생성 안 함 (가장 가벼움, 상태 저장 불가, CDO에서 직접 실행)
 * - InstancedPerActor       : 액터당 하나만 생성 (성능과 상태 저장 사이의 균형, 재사용)
 * - InstancedPerExecution   : 실행 시마다 새로 생성 (가장 무겁지만 각 실행이 독립적임)
 *
 * 3. Net Execution Policy (네트워크 실행 우선순위)
 * ----------------------------------------------------------------------------------
 * - LocalPredicted          : 클라에서 즉시 예측 실행 후 서버 승인 (가장 부드러운 반응성)
 * - LocalOnly               : 로컬 클라이언트에서만 실행
 * - ServerInitiated         : 서버가 먼저 실행하고 클라이언트에 전달 (보안 중심)
 * - ServerOnly              : 서버에서만 실행
 *
 * 4. Net Security Policy (네트워크 보안 규칙)
 * ----------------------------------------------------------------------------------
 * - ClientOrServer          : 클라이언트나 서버 어디서든 실행 요청 가능
 * - ServerOnlyExecution     : 클라이언트의 실행 요청을 무시하고 서버에서만 실행 제어
 * - ServerOnlyTermination   : 클라이언트의 종료 요청을 무시하고 서버에서만 종료 제어
 *
 * 5. Replication Policy (네트워크 복제 규칙)
 * ----------------------------------------------------------------------------------
 * - DoNotReplicate          : 어떠한 네트워크 복제도 수행하지 않음
 * - ReplicateNo             : 태그/쿨다운 데이터는 동기화되나 어빌리티 인스턴스는 복제 안 함
 * - ReplicateYes            : 어빌리티의 실행 및 상태 변화를 클라이언트에 복제
 */