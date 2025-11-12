// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Player/PlayerGameplayAbility.h"
#include "BunnySkillFourAbility.generated.h"

class UNiagaraSystem;
/**
 * 
 */
UCLASS()
class PROJECTFD_API UBunnySkillFourAbility : public UPlayerGameplayAbility
{
	GENERATED_BODY()

public:
	UBunnySkillFourAbility();

	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr,
		const FGameplayTagContainer* TargetTags = nullptr, FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	UAnimMontage* SkillMontage = nullptr;
	
	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	TSubclassOf<UGameplayEffect> CostGEClass;

	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	TSubclassOf<UGameplayEffect> CooldownGEClass;

	UPROPERTY(EditDefaultsOnly, Category="Skill")
	FGameplayTag CooldownTag;

	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	float CostAmount = 50.f;

	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	float SkillCooldown = 5.f;

	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	float SkillDamageMultiplier = 1.f;

	// 매초 소모 에너지
	UPROPERTY(EditDefaultsOnly, Category="Skill")
	float EnergyCostPerSecond = 10.f;

	// 초당 한 번 처리
	UPROPERTY(EditDefaultsOnly, Category="Skill")
	float TickInterval = 1.f;

	// 2초 유지마다 배율 +0.2
	UPROPERTY(EditDefaultsOnly, Category="Skill")
	float StackInterval = 2.f;

	UPROPERTY(EditDefaultsOnly, Category="Skill")
	FName MuzzleSocketName = TEXT("Bn_Socket_L_Hand");

	UPROPERTY(EditDefaultsOnly, Category="Skill")
	float BunnySkillFourTraceDistance = 5000.f;
	
	// 내부 상태
	UPROPERTY()
	float DamageMultiplier = 1.f;

	UPROPERTY()
	float StackAccumulated = 0.f;
	
	FTimerHandle CooldownTimerHandle;

	
	UPROPERTY(EditDefaultsOnly, Category="Skill")
	UNiagaraSystem* SkillFourNiagaraSystem;

	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	USoundBase* SkillFourSoundStart;
	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	USoundBase* SkillFourSoundLoop;
	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	USoundBase* SkillFourSoundEnd;
	
	UFUNCTION()
	void OnTick();

	UFUNCTION()
	void StartTick(FGameplayEventData EventData);

	bool DoLineTrace(const FGameplayAbilityActorInfo* ActorInfo, FHitResult& OutHit,
					 FVector& OutStart, FVector& OutEnd) const;
	bool PayCostAndDealDamage();

protected:
	// 디버그 라인트레이스 표시 옵션
	UPROPERTY(EditDefaultsOnly, Category="BunnySkillFour|Debug")
	bool bDrawTraceDebug = true;

	UPROPERTY(EditDefaultsOnly, Category="BunnySkillFour|Debug", meta=(EditCondition="bDrawTraceDebug", ClampMin="0.0", UIMin="0.0"))
	float TraceDebugTime = 0.1f;

	UPROPERTY(EditDefaultsOnly, Category="BunnySkillFour|Debug", meta=(EditCondition="bDrawTraceDebug"))
	FColor TraceColorHit = FColor::Red;

	UPROPERTY(EditDefaultsOnly, Category="BunnySkillFour|Debug", meta=(EditCondition="bDrawTraceDebug"))
	FColor TraceColorMiss = FColor::Green;

	UPROPERTY(EditDefaultsOnly, Category="BunnySkillFour|Debug", meta=(EditCondition="bDrawTraceDebug", ClampMin="0.1", UIMin="0.1"))
	float TraceThickness = 30.0f;

	UPROPERTY(EditDefaultsOnly, Category="BunnySkillFour|Debug", meta=(EditCondition="bDrawTraceDebug", ClampMin="1.0", UIMin="1.0"))
	float ImpactPointSize = 8.0f;
};
