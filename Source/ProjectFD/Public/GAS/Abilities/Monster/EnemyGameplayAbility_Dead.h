// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/Monster/EnemyGameplayAbility.h"
#include "EnemyGameplayAbility_Dead.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTFD_API UEnemyGameplayAbility_Dead : public UEnemyGameplayAbility
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Play Montage")
	UAnimMontage* PlayToMontage;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UFUNCTION()
	void MonsterDestroy();
};
