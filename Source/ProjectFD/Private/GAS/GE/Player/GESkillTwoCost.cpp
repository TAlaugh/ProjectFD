// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GE/Player/GESkillTwoCost.h"

#include "GAS/Player/PlayerAbilitySystemComponent.h"
#include "GAS/Player/PlayerAttributeSet.h"


UGESkillTwoCost::UGESkillTwoCost()
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;
	// 주기적 실행
	Period = FScalableFloat(0.1f);

	// 적용 시 첫 Tick 실행 여부
	bExecutePeriodicEffectOnApplication = true;

	// 주기적 Tick 누락 처리 정책
	PeriodicInhibitionPolicy = EGameplayEffectPeriodInhibitionRemovedPolicy::NeverReset;

	FGameplayModifierInfo ManaModifier;
	ManaModifier.Attribute = UPlayerAttributeSet::GetManaAttribute();
	ManaModifier.ModifierOp = EGameplayModOp::Additive;

	FSetByCallerFloat SBC;
	SBC.DataTag = FGameplayTag::RequestGameplayTag(FName("Data.Skill.Cost"));
	ManaModifier.ModifierMagnitude = FGameplayEffectModifierMagnitude(SBC);
	Modifiers.Add(ManaModifier);
}