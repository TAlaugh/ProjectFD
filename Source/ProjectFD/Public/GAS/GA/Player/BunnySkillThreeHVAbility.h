// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Player/PlayerGameplayAbility.h"
#include "BunnySkillThreeHVAbility.generated.h"

class UNiagaraSystem;
/**
 * 
 */
UCLASS()
class PROJECTFD_API UBunnySkillThreeHVAbility : public UPlayerGameplayAbility
{
	GENERATED_BODY()
	
public:
	UBunnySkillThreeHVAbility();

	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr,
		const FGameplayTagContainer* TargetTags = nullptr, FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
private:
	FTimerHandle DistanceTimerHandle;
	FTimerHandle DurationTimerHandle;
	FTimerHandle CooldownTimerHandle;

	
	// 이동 추적
	FVector LastLocation = FVector::ZeroVector;
	float AccumulatedDistance = 0.f;
	

	// 이동 기반 주기적 체크
	void StartTimer();
	void StopTimer();
	
	UFUNCTION()
	void OnDistanceCheck();

	// 이동 트리거 공격 수행
	void PerformMovementTriggeredAttack();
	
	
	// 지속 시간 종료
	UFUNCTION()
	void OnAbilityDurationEnd();


protected:
	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	TSubclassOf<UGameplayEffect> CostGEClass;

	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	TSubclassOf<UGameplayEffect> CooldownGEClass;

	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	TSubclassOf<UGameplayEffect> DurationGEClass;

	// 이 어빌리티만 사용하는 전용 쿨다운 태그
	UPROPERTY(EditDefaultsOnly, Category="Skill")
	FGameplayTag CooldownTag;
	
	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	float TargetCapsuleSweepRadius = 1000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	float TargetCapsuleHalfHeight = 100.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	float CostAmount = 20.f;

	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	float SkillDamageMultiplier = 1.f;

	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	float TargetAmount = 10.f;

	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	float SkillCooldown = 5.f;

	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	float SkillDuration = 5.f;

	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	float IntervalSec = 1.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	float MovementDistanceInterval = 200.f;

	UPROPERTY(EditDefaultsOnly, Category="Skill")
	UNiagaraSystem* SkillThreeNiagaraSystem;

	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	USoundBase* SkillThreeSound;
};
