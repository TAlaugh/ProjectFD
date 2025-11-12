// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Player/PlayerGameplayAbility.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "MeleeAttackAbility.generated.h"

class UNiagaraSystem;
/**
 * 
 */
UCLASS()
class PROJECTFD_API UMeleeAttackAbility : public UPlayerGameplayAbility
{
	GENERATED_BODY()
	
public:
	UMeleeAttackAbility();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	
	UPROPERTY(EditDefaultsOnly, Category="Punch", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* PunchActionMontage;

	UPROPERTY(EditDefaultsOnly, Category="Punch", meta = (AllowPrivateAccess = "true"))
	UNiagaraSystem* MeleeAttackEffect;

	UPROPERTY(EditDefaultsOnly, Category="Punch", meta = (AllowPrivateAccess = "true"))
	float MeleeAttackRange = 50.f;
	
	UFUNCTION()
	void OnMeleeAttackEvent(FGameplayEventData EventData);
};
