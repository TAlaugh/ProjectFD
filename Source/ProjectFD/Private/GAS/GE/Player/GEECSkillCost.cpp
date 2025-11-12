// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GE/Player/GEECSkillCost.h"

#include "AbilitySystemComponent.h"
#include "GAS/Player/PlayerAttributeSet.h"


UGEECSkillCost::UGEECSkillCost()
{
}

void UGEECSkillCost::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	Super::Execute_Implementation(ExecutionParams, OutExecutionOutput);

	UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	AActor* SourceActor = SourceASC->GetAvatarActor();
	UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();
	AActor* TargetActor = TargetASC->GetAvatarActor();

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	const FGameplayTagContainer* SpecTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SpecTags;
	EvaluationParameters.TargetTags = TargetTags;

	UPlayerAttributeSet* PlayerAttrSet = const_cast<UPlayerAttributeSet*>(SourceASC->GetSet<UPlayerAttributeSet>());
	float SkillCost = PlayerAttrSet->GetSkillCost();

	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(
		UPlayerAttributeSet::GetManaAttribute(),
		EGameplayModOp::Additive, -SkillCost));
 
	
}