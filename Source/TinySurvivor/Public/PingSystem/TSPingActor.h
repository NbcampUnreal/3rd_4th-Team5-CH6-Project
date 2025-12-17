#pragma once

#include "CoreMinimal.h"
#include "TSPingTypes.h"
#include "GameFramework/Actor.h"
#include "Character/TSCharacter.h"
#include "TSPingActor.generated.h"

UCLASS()
class TINYSURVIVOR_API ATSPingActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ATSPingActor();
	
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Meta = (ExposeOnSpawn = "true"))
	ETSPingType PingType;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
