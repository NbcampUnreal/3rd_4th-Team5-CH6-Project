#include "GAS/Ability/GA_Roll.h"
#include "GameplayTagContainer.h"

UGA_Roll::UGA_Roll()
{
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Move.Roll")));
	UE_LOG(LogTemp, Log, TEXT("GAS_Roll pressed"));
}
void UGA_Roll::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	UE_LOG(LogTemp, Warning, TEXT("[GA_Roll] ActivateAbility"));
	EndAbility(Handle, ActorInfo, ActivationInfo, /*bReplicateEndAbility*/true, /*bWasCancelled*/false);
}
void UGA_Roll::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled)
{
	UE_LOG(LogTemp, Warning, TEXT("[GA_Roll] EndAbility"));
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}