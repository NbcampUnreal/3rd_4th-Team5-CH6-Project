// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_Hotkey.generated.h"

/**
 * 
 */
UCLASS()
class TINYSURVIVOR_API UGA_Hotkey : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_Hotkey();
	void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
};
