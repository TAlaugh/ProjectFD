// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GE/Player/GESkillOneCost.h"

#include "GAS/Player/PlayerAttributeSet.h"


UGESkillOneCost::UGESkillOneCost()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;

	FGameplayModifierInfo ElectricModifier;
	ElectricModifier.Attribute = UPlayerAttributeSet::GetSpecialResourceAttribute();
	ElectricModifier.ModifierOp = EGameplayModOp::Additive;

	FSetByCallerFloat SBC;
	SBC.DataTag = FGameplayTag::RequestGameplayTag(FName("Data.Skill.Cost"));
	ElectricModifier.ModifierMagnitude = FGameplayEffectModifierMagnitude(SBC);

	Modifiers.Add(ElectricModifier);
}
