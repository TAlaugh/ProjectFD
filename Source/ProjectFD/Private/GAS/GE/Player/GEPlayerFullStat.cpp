// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GE/Player/GEPlayerFullStat.h"

#include "GAS/Player/PlayerAttributeSet.h"


UGEPlayerFullStat::UGEPlayerFullStat()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;
	
	// Health = MaxHealth
	{
		FGameplayModifierInfo HealthMod;
		HealthMod.Attribute = UPlayerAttributeSet::GetHealthAttribute();
		HealthMod.ModifierOp = EGameplayModOp::Override;

		FAttributeBasedFloat HealthFromMaxHealth;
		HealthFromMaxHealth.BackingAttribute = FGameplayEffectAttributeCaptureDefinition(
			UPlayerAttributeSet::GetMaxHealthAttribute(),
			EGameplayEffectAttributeCaptureSource::Source,
			false
		);
		HealthFromMaxHealth.AttributeCurve = FCurveTableRowHandle(); 
		HealthFromMaxHealth.Coefficient = 1.0f;
		HealthFromMaxHealth.PreMultiplyAdditiveValue = 0.0f;
		HealthFromMaxHealth.PostMultiplyAdditiveValue = 0.0f;

		HealthMod.ModifierMagnitude = FGameplayEffectModifierMagnitude(HealthFromMaxHealth);
		Modifiers.Add(HealthMod);
	}
	
	// Mana = MaxMana
	{
		FGameplayModifierInfo ManaMod;
		ManaMod.Attribute = UPlayerAttributeSet::GetManaAttribute();
		ManaMod.ModifierOp = EGameplayModOp::Override;

		FAttributeBasedFloat ManaFromMaxMana;
		ManaFromMaxMana.BackingAttribute = FGameplayEffectAttributeCaptureDefinition(
			UPlayerAttributeSet::GetMaxManaAttribute(),
			EGameplayEffectAttributeCaptureSource::Source,
			false
		);
		ManaFromMaxMana.AttributeCurve = FCurveTableRowHandle();
		ManaFromMaxMana.Coefficient = 1.0f;
		ManaFromMaxMana.PreMultiplyAdditiveValue = 0.0f;
		ManaFromMaxMana.PostMultiplyAdditiveValue = 0.0f;

		ManaMod.ModifierMagnitude = FGameplayEffectModifierMagnitude(ManaFromMaxMana);
		Modifiers.Add(ManaMod);
	}

	// Shield = MaxShield
	{
		FGameplayModifierInfo ShieldMod;
		ShieldMod.Attribute = UPlayerAttributeSet::GetShieldAttribute();
		ShieldMod.ModifierOp = EGameplayModOp::Override;

		FAttributeBasedFloat ShieldFromMaxShield;
		ShieldFromMaxShield.BackingAttribute = FGameplayEffectAttributeCaptureDefinition(
			UPlayerAttributeSet::GetMaxShieldAttribute(),
			EGameplayEffectAttributeCaptureSource::Source,
			false
		);
		ShieldFromMaxShield.AttributeCurve = FCurveTableRowHandle();
		ShieldFromMaxShield.Coefficient = 1.0f;
		ShieldFromMaxShield.PreMultiplyAdditiveValue = 0.0f;
		ShieldFromMaxShield.PostMultiplyAdditiveValue = 0.0f;

		ShieldMod.ModifierMagnitude = FGameplayEffectModifierMagnitude(ShieldFromMaxShield);
		Modifiers.Add(ShieldMod);
	}

	// Stamina = MaxStamina
	{
		FGameplayModifierInfo StaminaMod;
		StaminaMod.Attribute = UPlayerAttributeSet::GetStaminaAttribute();
		StaminaMod.ModifierOp = EGameplayModOp::Override;

		FAttributeBasedFloat StaminaFromMaxStamina;
		StaminaFromMaxStamina.BackingAttribute = FGameplayEffectAttributeCaptureDefinition(
			UPlayerAttributeSet::GetMaxStaminaAttribute(),
			EGameplayEffectAttributeCaptureSource::Source,
			false
		);
		StaminaFromMaxStamina.AttributeCurve = FCurveTableRowHandle();
		StaminaFromMaxStamina.Coefficient = 1.0f;
		StaminaFromMaxStamina.PreMultiplyAdditiveValue = 0.0f;
		StaminaFromMaxStamina.PostMultiplyAdditiveValue = 0.0f;

		StaminaMod.ModifierMagnitude = FGameplayEffectModifierMagnitude(StaminaFromMaxStamina);
		Modifiers.Add(StaminaMod);
	}

	// Ammo = MaxAmmo
	{
		FGameplayModifierInfo AmmoMod;
		AmmoMod.Attribute = UPlayerAttributeSet::GetAmmoAttribute();
		AmmoMod.ModifierOp = EGameplayModOp::Override;

		FAttributeBasedFloat AmmoFromMaxAmmo;
		AmmoFromMaxAmmo.BackingAttribute = FGameplayEffectAttributeCaptureDefinition(
			UPlayerAttributeSet::GetMaxAmmoAttribute(),
			EGameplayEffectAttributeCaptureSource::Source,
			false
		);
		AmmoFromMaxAmmo.AttributeCurve = FCurveTableRowHandle();
		AmmoFromMaxAmmo.Coefficient = 1.0f;
		AmmoFromMaxAmmo.PreMultiplyAdditiveValue = 0.0f;
		AmmoFromMaxAmmo.PostMultiplyAdditiveValue = 0.0f;

		AmmoMod.ModifierMagnitude = FGameplayEffectModifierMagnitude(AmmoFromMaxAmmo);
		Modifiers.Add(AmmoMod);
	}

	// GeneralAmmo = MaxGeneralAmmo
	{
		FGameplayModifierInfo GeneralAmmoMod;
		GeneralAmmoMod.Attribute = UPlayerAttributeSet::GetGeneralAmmoAttribute();
		GeneralAmmoMod.ModifierOp = EGameplayModOp::Override;

		FAttributeBasedFloat GeneralAmmoFromMaxGeneralAmmo;
		GeneralAmmoFromMaxGeneralAmmo.BackingAttribute = FGameplayEffectAttributeCaptureDefinition(
			UPlayerAttributeSet::GetMaxGeneralAmmoAttribute(),
			EGameplayEffectAttributeCaptureSource::Source,
			false
		);
		GeneralAmmoFromMaxGeneralAmmo.AttributeCurve = FCurveTableRowHandle();
		GeneralAmmoFromMaxGeneralAmmo.Coefficient = 1.0f;
		GeneralAmmoFromMaxGeneralAmmo.PreMultiplyAdditiveValue = 0.0f;
		GeneralAmmoFromMaxGeneralAmmo.PostMultiplyAdditiveValue = 0.0f;

		GeneralAmmoMod.ModifierMagnitude = FGameplayEffectModifierMagnitude(GeneralAmmoFromMaxGeneralAmmo);
		Modifiers.Add(GeneralAmmoMod);
	}

	// HighPowerAmmo = MaxHighPowerAmmo
	{
		FGameplayModifierInfo HighPowerAmmoMod;
		HighPowerAmmoMod.Attribute = UPlayerAttributeSet::GetHighPowerAmmoAttribute();
		HighPowerAmmoMod.ModifierOp = EGameplayModOp::Override;

		FAttributeBasedFloat HighPowerAmmoFromMaxHighPowerAmmo;
		HighPowerAmmoFromMaxHighPowerAmmo.BackingAttribute = FGameplayEffectAttributeCaptureDefinition(
			UPlayerAttributeSet::GetMaxHighPowerAmmoAttribute(),
			EGameplayEffectAttributeCaptureSource::Source,
			false
		);
		HighPowerAmmoFromMaxHighPowerAmmo.AttributeCurve = FCurveTableRowHandle();
		HighPowerAmmoFromMaxHighPowerAmmo.Coefficient = 1.0f;
		HighPowerAmmoFromMaxHighPowerAmmo.PreMultiplyAdditiveValue = 0.0f;
		HighPowerAmmoFromMaxHighPowerAmmo.PostMultiplyAdditiveValue = 0.0f;

		HighPowerAmmoMod.ModifierMagnitude = FGameplayEffectModifierMagnitude(HighPowerAmmoFromMaxHighPowerAmmo);
		Modifiers.Add(HighPowerAmmoMod);
	}
}
