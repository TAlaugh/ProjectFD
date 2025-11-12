// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Player/PlayerGameplayAbility.h"
#include "BunnySkillTwoAbility.generated.h"

class UNiagaraSystem;
/**
 * 
 */
UCLASS()
class PROJECTFD_API UBunnySkillTwoAbility : public UPlayerGameplayAbility
{
	GENERATED_BODY()

public:
	UBunnySkillTwoAbility();
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	TSubclassOf<UGameplayEffect> GrantTagGEClass;

	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	TSubclassOf<UGameplayEffect> FirstCostGEClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	TSubclassOf<UGameplayEffect> CostGEClass;

	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	TSubclassOf<UGameplayEffect> CooldownGEClass;

	UPROPERTY(EditDefaultsOnly, Category="Skill")
	FGameplayTag CooldownTag;

	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	float FirstCostAmount = 12.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	float CostAmount = 1.f;

	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	float SkillCooldown = 2.f;

	FTimerHandle CooldownTimerHandle;

	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	UNiagaraSystem* SkillTwoNiagaraSystem;

	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	USoundBase* SkillTwoSoundStart;

	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	USoundBase* SkillTwoSoundEnd;
	
protected:
	// 마나가 부족할 떄 호출될 함수
	UFUNCTION()
	void OnEndAbility();
};
