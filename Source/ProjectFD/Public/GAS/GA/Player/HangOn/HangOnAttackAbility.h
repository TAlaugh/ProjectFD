// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Player/PlayerGameplayAbility.h"
#include "HangOnAttackAbility.generated.h"

class UNiagaraSystem;
/**
 * 
 */
UCLASS()
class PROJECTFD_API UHangOnAttackAbility : public UPlayerGameplayAbility
{
	GENERATED_BODY()
	
public:
	UHangOnAttackAbility();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;



private:
	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	UAnimMontage* HangOnAttackMontage = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	float HangOnAttackDamageMultiplier = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	int32 MaxHitCount = 0;
	
	UFUNCTION()
	void HangOnAttack(FGameplayEventData EventData);

	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	UNiagaraSystem* HangOnAttackNiagaraEffect;
	
	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	USoundBase* HangOnAttackSound;
};
