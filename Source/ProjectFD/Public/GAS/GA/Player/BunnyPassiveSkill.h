// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Player/PlayerGameplayAbility.h"
#include "BunnyPassiveSkill.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTFD_API UBunnyPassiveSkill : public UPlayerGameplayAbility
{
	GENERATED_BODY()
	
public:
	UBunnyPassiveSkill();

	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

private:
	FTimerHandle DistanceTimerHandle;
	FTimerHandle ElectricDecreaseTimerHandle;
	FVector LastLocation = FVector::ZeroVector;
	double AccumulatedDistance = 0.0;
	double AccumulatedDistance2 = 0.0;
	double LastElectricIncreaseTime = 0.0;

	UFUNCTION()
	void StartTimer();
	void StopTimer();
	void OnDistanceCheck();
	void StartElectricDecreaseTimer();
	void OnElectricDecreaseCheck();
	void ElectricDecrease();
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	TSubclassOf<UGameplayEffect> ElectricIncreaseGEClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	TSubclassOf<UGameplayEffect> ElectricDecreaseGEClass;

	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	bool bActivateOnGiveAbility = false;

	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	float IntervalSec = 0.1f;

	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	float IntervalDistance = 100.f;

	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	float ElectricPerIncrease = 5.f;

	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	float ElectricDecreaseDelay = 3.f;

	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	float ElectricPerDecrease = 3.f;

	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	USoundBase* RunningSound;
	
};
