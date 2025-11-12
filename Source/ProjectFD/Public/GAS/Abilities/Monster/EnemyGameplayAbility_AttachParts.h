// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/Monster/EnemyGameplayAbility.h"
#include "EnemyGameplayAbility_AttachParts.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTFD_API UEnemyGameplayAbility_AttachParts : public UEnemyGameplayAbility
{
	GENERATED_BODY()
	
public:
	UEnemyGameplayAbility_AttachParts();
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	
};
