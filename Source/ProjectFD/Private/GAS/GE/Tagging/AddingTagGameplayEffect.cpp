// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GE/Tagging/AddingTagGameplayEffect.h"


UAddingTagGameplayEffect::UAddingTagGameplayEffect()
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;
	// DurationMagnitude = FScalableFloat(5.0f);

	StackLimitCount = 1;
	// StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	// StackExpirationPolicy = EGameplayEffectStackingExpirationPolicy::RemoveSingleStackAndRefreshDuration;

	// FGameplayModifierInfo Modifier;
	//
	// Modifier.Attribute = FGameplayAttribute(UProperty* /* Health Attribute Ref */);
	// Modifier.ModifierOp = EGameplayModOp::Additive;
	// Modifier.ModifierMagnitude = FScalableFloat(-5.f);
	//
	// Modifiers.Add(Modifier);
}
