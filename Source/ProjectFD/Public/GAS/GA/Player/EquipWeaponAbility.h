// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Player/PlayerGameplayAbility.h"
#include "EquipWeaponAbility.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTFD_API UEquipWeaponAbility : public UPlayerGameplayAbility
{
	GENERATED_BODY()
	
public:
	UEquipWeaponAbility();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

private:
	UPROPERTY(EditDefaultsOnly, Category="Ability")
	UAnimMontage* EquipMontage;

	UFUNCTION()
	void ChangeWeapon(FGameplayEventData EventData);

	UPROPERTY()
	TWeakObjectPtr<AFDPlayerCharacter> PC;

	UPROPERTY(EditDefaultsOnly, Category="Ability")
	int EquipNumber = 0; 
};
