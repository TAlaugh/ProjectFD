// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Player/PlayerGameplayAbility.h"
#include "HangOnGrabAbility.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTFD_API UHangOnGrabAbility : public UPlayerGameplayAbility
{
	GENERATED_BODY()
	
public:
	UHangOnGrabAbility();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	UAnimMontage* HangOnGrabMontage = nullptr;
};
