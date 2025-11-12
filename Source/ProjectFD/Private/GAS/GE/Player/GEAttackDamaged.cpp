// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GE/Player/GEAttackDamaged.h"

#include "GAS/FDAttributeSet.h"


UGEAttackDamaged::UGEAttackDamaged()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;

	FGameplayModifierInfo HealthModifier;
	//HealthModifier.Attribute = FGameplayAttribute(FindFieldChecked<FProperty>(UFDAttributeSet::StaticClass(), GET_MEMBER_NAME_CHECKED(UFDAttributeSet, GetHealthAttribute())));
	HealthModifier.Attribute = UFDAttributeSet::GetHealthAttribute();
	HealthModifier.ModifierOp = EGameplayModOp::Additive;
	HealthModifier.ModifierMagnitude = FScalableFloat(-10.f);
	Modifiers.Add(HealthModifier);
}
