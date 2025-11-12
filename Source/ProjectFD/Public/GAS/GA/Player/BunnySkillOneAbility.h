// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Player/PlayerGameplayAbility.h"
#include "BunnySkillOneAbility.generated.h"

class UNiagaraSystem;
/**
 * 
 */
UCLASS()
class PROJECTFD_API UBunnySkillOneAbility : public UPlayerGameplayAbility
{
	GENERATED_BODY()
	
public:
	UBunnySkillOneAbility();

	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr,
		const FGameplayTagContainer* TargetTags = nullptr, FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	UAnimMontage* SkillMontage = nullptr;
	
	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	TSubclassOf<UGameplayEffect> CostGEClass;

	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	TSubclassOf<UGameplayEffect> CooldownGEClass;

	// 이 어빌리티만 사용하는 전용 쿨다운 태그
	UPROPERTY(EditDefaultsOnly, Category="Skill")
	FGameplayTag CooldownTag;
	
	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	float TargetSweepSphereRadius = 1000.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	float CostAmount = 50.f;

	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	float SkillDamageMultiplier = 1.f;

	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	float TargetAmount = 1.f;

	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	float SkillCooldown = 5.f;

	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	FName SocketName = FName("Bip001-L-Hand");

	FTimerHandle CooldownTimerHandle;
	
	UFUNCTION()
	void StartTargetSweep(FGameplayEventData EventData);

	UPROPERTY(EditDefaultsOnly, Category="Skill")
	UNiagaraSystem* SkillOneNiagaraSystem;

	UPROPERTY(EditDefaultsOnly, Category="Skill")
	UNiagaraSystem* SkillOneSpawnSphereNiagaraSystem;
	
	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	USoundBase* SkillOneSound;
};
