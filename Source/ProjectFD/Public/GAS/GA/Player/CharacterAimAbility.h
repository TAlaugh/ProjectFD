// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "GAS/Player/PlayerGameplayAbility.h"
#include "CharacterAimAbility.generated.h"

class UAbilityTask_PlayMontageAndWait;
/**
 * 
 */
UCLASS()
class PROJECTFD_API UCharacterAimAbility : public UPlayerGameplayAbility
{
	GENERATED_BODY()

public:
	UCharacterAimAbility();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
protected:
	// 보간 시간 (초)
	UPROPERTY(EditDefaultsOnly, Category = "Aim")
	float AimInterpDuration = 0.2f;
	
private:
	UPROPERTY(EditDefaultsOnly, Category="Animation")
	UAnimMontage* AimInMontage;
	
	FTimerHandle AimInterpTimerHandle;
	
	float AimInterpElapsed = 0.f;
	float AimStartArmLength = 0.f;
	float AimTargetArmLength = 0.f;
	float OriginalArmLength = 0.f;
	bool bIsAiming = false;
	bool bEndOnInterpComplete = false;

	void StartAimInterp(float NewTarget, float Duration);
	void TickAimInterp();
	void StopAimInterp();
};
