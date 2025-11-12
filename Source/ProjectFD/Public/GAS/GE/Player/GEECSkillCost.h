// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "GEECSkillCost.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTFD_API UGEECSkillCost : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
	
public:
	UGEECSkillCost();

protected:
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
		FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
	
};
