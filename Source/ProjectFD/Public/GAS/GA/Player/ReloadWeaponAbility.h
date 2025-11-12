// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/FDGameplayAbility.h"
#include "ReloadWeaponAbility.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTFD_API UReloadWeaponAbility : public UFDGameplayAbility
{
	GENERATED_BODY()
	
public:
	UReloadWeaponAbility();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

private:
	UPROPERTY()
	UFDAbilitySystemComponent* ASC;

	UPROPERTY(EditDefaultsOnly, Category = "Reload")
	UAnimMontage* ReloadMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Reload")
	USoundBase* MGReloadSound;
	UPROPERTY(EditDefaultsOnly, Category = "Reload")
	USoundBase* MGReloadEndSound;

	UPROPERTY(EditDefaultsOnly, Category = "Reload")
	USoundBase* LNCReloadSound;
	UPROPERTY(EditDefaultsOnly, Category = "Reload")
	USoundBase* LNCReloadEndSound;

	UFUNCTION()
	void OnMontageEnded(FGameplayEventData Payload);
};
