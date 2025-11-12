// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "Abilities/GameplayAbility.h"
#include "FDGameplayAbility.generated.h"

class UPawnCombatComponent;
class UFDAbilitySystemComponent;
/**
 * 
 */
UENUM(BlueprintType)
enum class EFDAbilityActivationPolicy : uint8
{
	OnTriggered,
	OnGiven
};

UCLASS()
class PROJECTFD_API UFDGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
	
protected:
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
	UPROPERTY(EditDefaultsOnly)
	EFDAbilityActivationPolicy AbilityActivationPolicy = EFDAbilityActivationPolicy::OnTriggered;

public:
	// Combat
	UFUNCTION(BlueprintPure, Category = "GA")
	UPawnCombatComponent* GetPawnCombatComponentFromActorInfo() const;

	// ASC
	UFUNCTION(BlueprintPure, Category = "GA")
	UAbilitySystemComponent* GetFDAbilitySystemComponentFromActorInfo() const;

	UFUNCTION(BlueprintPure, Category = "GA")
	UAnimInstance* GetAnimInstance() const;
};
