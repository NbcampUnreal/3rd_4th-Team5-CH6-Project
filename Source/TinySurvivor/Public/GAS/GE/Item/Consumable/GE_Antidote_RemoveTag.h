// GE_Antidote_RemoveTag.h
#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "GE_Antidote_RemoveTag.generated.h"

/*
	해독제 전용 RemoveTag 용도 GameplayEffect
	
	사용 방법:
	1. 이 클래스를 부모로 하는 블루프린트 생성
	2. GEC는 C++에서 자동 연동 (GEC에서 Tag제거)
	3. GA에 매핑
	3. DT_Item의 해당 아이템에 AbilityBP 매핑 확인
	
	특징:
	- Instant 정책
	- ItemID 자동 매핑 불가 → 아이템별 블루프린트 생성 필요
	
	네트워크:
	- Attribute 변경은 자동으로 복제됨
*/
UCLASS(Abstract, Blueprintable)
class TINYSURVIVOR_API UGE_Antidote_RemoveTag : public UGameplayEffect
{
	GENERATED_BODY()
	
public:
	UGE_Antidote_RemoveTag();
};
