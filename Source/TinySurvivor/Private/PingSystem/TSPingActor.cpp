#include "PingSystem/TSPingActor.h"
#include "Net/UnrealNetwork.h"

ATSPingActor::ATSPingActor()
{
	bReplicates = true;
}

void ATSPingActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ATSPingActor, PingType);
}
