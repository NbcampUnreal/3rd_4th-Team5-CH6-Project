// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectCustomApplicationRequirement.h"
#include "TSGECR_MovementRequire.generated.h"

/**
 * 적용 요건(Application Requirements): 적용 요건은 여기에 포함된 게임플레이 태그 세트가 있어야 (또는 없어야) 게임플레이 이펙트를 적용하도록 하거나, 랜덤으로 게임플레이 이펙트를 적용하지 않도록 할 수 있습니다. 
 * 이러한 요건이 게임의 니즈를 충족하지 않으면 UGameplayEffectCustomApplicationRequirement 베이스 클래스에서 데이터 오브젝트를 파생한 뒤, 거기에 복잡한 적용 규칙을 임의로 정의하는 네이티브 코드를 작성할 수 있습니다.
 */
UCLASS()
class TINYSURVIVOR_API UTSGECR_MovementRequire : public UGameplayEffectCustomApplicationRequirement
{
	GENERATED_BODY()
};
