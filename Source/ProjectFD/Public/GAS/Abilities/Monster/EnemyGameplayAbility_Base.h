// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/Monster/EnemyGameplayAbility.h"
#include "EnemyGameplayAbility_Base.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTFD_API UEnemyGameplayAbility_Base : public UEnemyGameplayAbility
{
	GENERATED_BODY()
	
public:
	UEnemyGameplayAbility_Base();
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Cooldown")
	TSubclassOf<UGameplayEffect> CooldownGameplayEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Play Montage")
	UAnimMontage* PlayToMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Play Montage")
	UAnimMontage* DestroyAttachPartMontage;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UFUNCTION()
	void OnMontageEndWait();

	UFUNCTION()
	void OnWaitFinished();
};
