// All CopyRight From YulRyongGameStudio //


#include "A_FOR_COMMON/Library/Debug/Net/TSDebugNetLibrary.h"

void UTSDebugNetLibrary::GetActorNetworkRoles(const AActor* TargetActor)
{
	if (!IsValid(TargetActor)) return;
	
	const UEnum* RoleEnum = StaticEnum<ENetRole>();
    
	ENetRole LocalRole = TargetActor->GetLocalRole();
	ENetRole RemoteRole = TargetActor->GetRemoteRole();

	FString LocalRoleStr = RoleEnum->GetValueAsString(LocalRole);
	FString RemoteRoleStr = RoleEnum->GetValueAsString(RemoteRole);

	UE_LOG(LogTemp, Log, TEXT("[TSDebug] Actor: %s | LocalRole: %s | RemoteRole: %s"), *TargetActor->GetName(), *LocalRoleStr, *RemoteRoleStr);
}

void UTSDebugNetLibrary::CheckCurrentNetContext(const UObject* WorldContextObject)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!IsValid(World)) return;

	FString NetModeStr;
	ENetMode NetMode = World->GetNetMode();

	switch (NetMode)
	{
	case NM_Standalone:      NetModeStr = TEXT("Standalone"); break;
	case NM_DedicatedServer: NetModeStr = TEXT("Dedicated Server"); break;
	case NM_ListenServer:    NetModeStr = TEXT("Listen Server"); break;
	case NM_Client:          NetModeStr = TEXT("Client"); break;
	default:                 NetModeStr = TEXT("Unknown"); break;
	}

	UE_LOG(LogTemp, Log, TEXT("[TSDebug] Current NetMode: %s "), *NetModeStr);
}
