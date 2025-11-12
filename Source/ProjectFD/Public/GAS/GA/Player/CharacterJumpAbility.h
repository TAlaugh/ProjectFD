// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Player/PlayerGameplayAbility.h"
#include "CharacterJumpAbility.generated.h"

class UNiagaraSystem;
/**
 * 
 */
UCLASS()
class PROJECTFD_API UCharacterJumpAbility : public UPlayerGameplayAbility
{
	GENERATED_BODY()
	
public:
	UCharacterJumpAbility();

	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	UFUNCTION()
	void OnLandedCallback(const FHitResult& Hit);


	UPROPERTY(EditDefaultsOnly, Category = "Jump")
	int32 MaxJumpCount = 3;

	UPROPERTY(EditDefaultsOnly, Category = "Jump")
	int32 CurrentJumpCount = 0;

	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	UAnimMontage* DoubleJumpMontage;

	UPROPERTY(EditDefaultsOnly, Category="Jump|Landing")
	float LandingDamage = 1.f;

	UPROPERTY(EditDefaultsOnly, Category="Jump|Landing")
	float LandingDamageRadius = 1.f;

	UPROPERTY(EditDefaultsOnly, Category="Jump|Landing")
	float TargetAmount = 4.f;

	UPROPERTY(EditDefaultsOnly, Category="Niagara")
	UNiagaraSystem* LandingNiagaraSystem;

	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	USoundBase* JumpSound;
	
	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	USoundBase* LandingSound;
	
	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	USoundBase* SkillSound;

};
