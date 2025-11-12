// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/FDGameplayAbility.h"
#include "AttackHitCheckAbility.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTFD_API UAttackHitCheckAbility : public UFDGameplayAbility
{
	GENERATED_BODY()
	
public:
	UAttackHitCheckAbility();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
protected:
	UFUNCTION()
	void OnTraceResultCallback(const FGameplayAbilityTargetDataHandle& TargetDataHandle);
};
