// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/FDGameplayAbility.h"
#include "CharacterRollAbility.generated.h"

class AFDPlayerCharacter;
/**
 * 
 */
UCLASS()
class PROJECTFD_API UCharacterRollAbility : public UFDGameplayAbility
{
	GENERATED_BODY()
	
public:
	virtual void ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* RollMontage;
	
	FTimerHandle RollTimerHandle;
	float TotalRollDistance;
	float RollElapsedTime;
	float RollDuration;
	AFDPlayerCharacter* RollCharacter;
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	USoundBase* RollSound;

	void TickRollMove();
};
