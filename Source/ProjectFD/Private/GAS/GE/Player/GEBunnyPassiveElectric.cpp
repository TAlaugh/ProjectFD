// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GE/Player/GEBunnyPassiveElectric.h"

#include "GAS/Player/PlayerAttributeSet.h"


UGEBunnyPassiveElectric::UGEBunnyPassiveElectric()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;

	// Electric 증가
	FGameplayModifierInfo ElectricModifier;
	ElectricModifier.Attribute = UPlayerAttributeSet::GetSpecialResourceAttribute();
	ElectricModifier.ModifierOp = EGameplayModOp::Additive;

	FSetByCallerFloat SBC;
	SBC.DataTag = FGameplayTag::RequestGameplayTag(FName("Data.Bunny.Passive.Electric"));
	ElectricModifier.ModifierMagnitude = FGameplayEffectModifierMagnitude(SBC);
	Modifiers.Add(ElectricModifier);
}
