// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GE/Player/GEPlayerInit.h"

#include "GAS/Player/PlayerAttributeSet.h"


UGEPlayerInit::UGEPlayerInit()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;

	//MaxHealth설정
	{
		FGameplayModifierInfo MaxHealthModifier;
		MaxHealthModifier.Attribute = UPlayerAttributeSet::GetMaxHealthAttribute();
		MaxHealthModifier.ModifierOp = EGameplayModOp::Override;
		MaxHealthModifier.ModifierMagnitude = FScalableFloat(MaxHealth);
		Modifiers.Add(MaxHealthModifier);
	}

	//MaxMana설정
	{
		FGameplayModifierInfo MaxManaModifier;
		MaxManaModifier.Attribute = UPlayerAttributeSet::GetMaxManaAttribute();
		MaxManaModifier.ModifierOp = EGameplayModOp::Override;
		MaxManaModifier.ModifierMagnitude = FScalableFloat(MaxMana);
		Modifiers.Add(MaxManaModifier);
	}

	//MaxShield설정
	{
		FGameplayModifierInfo MaxShieldModifier;
		MaxShieldModifier.Attribute = UPlayerAttributeSet::GetMaxShieldAttribute();
		MaxShieldModifier.ModifierOp = EGameplayModOp::Override;
		MaxShieldModifier.ModifierMagnitude = FScalableFloat(MaxShield);
		Modifiers.Add(MaxShieldModifier);
	}

	//MaxStamina설정
	{
		FGameplayModifierInfo MaxStaminaModifier;
		MaxStaminaModifier.Attribute = UPlayerAttributeSet::GetMaxStaminaAttribute();
		MaxStaminaModifier.ModifierOp = EGameplayModOp::Override;
		MaxStaminaModifier.ModifierMagnitude = FScalableFloat(MaxStamina);
		Modifiers.Add(MaxStaminaModifier);
	}

	//MaxSpecialResource설정
	{
		FGameplayModifierInfo MaxSpecialResourceModifier;
		MaxSpecialResourceModifier.Attribute = UPlayerAttributeSet::GetMaxSpecialResourceAttribute();
		MaxSpecialResourceModifier.ModifierOp = EGameplayModOp::Override;
		MaxSpecialResourceModifier.ModifierMagnitude = FScalableFloat(MaxSpecialResource);
		Modifiers.Add(MaxSpecialResourceModifier);
	}

	//MaxAmmo설정
	{
		FGameplayModifierInfo MaxAmmoModifier;
		MaxAmmoModifier.Attribute = UPlayerAttributeSet::GetMaxAmmoAttribute();
		MaxAmmoModifier.ModifierOp = EGameplayModOp::Override;
		MaxAmmoModifier.ModifierMagnitude = FScalableFloat(MaxAmmo);
		Modifiers.Add(MaxAmmoModifier);
	}

	//Ammo설정
	{
		FGameplayModifierInfo AmmoModifier;
		AmmoModifier.Attribute = UPlayerAttributeSet::GetAmmoAttribute();
		AmmoModifier.ModifierOp = EGameplayModOp::Override;
		AmmoModifier.ModifierMagnitude = FScalableFloat(Ammo);
		Modifiers.Add(AmmoModifier);
	}

	//MaxGeneralAmmo설정
	{
		FGameplayModifierInfo MaxGeneralAmmoModifier;
		MaxGeneralAmmoModifier.Attribute = UPlayerAttributeSet::GetMaxGeneralAmmoAttribute();
		MaxGeneralAmmoModifier.ModifierOp = EGameplayModOp::Override;
		MaxGeneralAmmoModifier.ModifierMagnitude = FScalableFloat(MaxGeneralAmmo);
		Modifiers.Add(MaxGeneralAmmoModifier);
	}

	//GeneralAmmo설정
	{
		FGameplayModifierInfo GeneralAmmoModifier;
		GeneralAmmoModifier.Attribute = UPlayerAttributeSet::GetGeneralAmmoAttribute();
		GeneralAmmoModifier.ModifierOp = EGameplayModOp::Override;
		GeneralAmmoModifier.ModifierMagnitude = FScalableFloat(GeneralAmmo);
		Modifiers.Add(GeneralAmmoModifier);
	}

	//MaxSpecialAmmo설정
	{
		FGameplayModifierInfo MaxSpecialModifier;
		MaxSpecialModifier.Attribute = UPlayerAttributeSet::GetMaxSpecialAmmoAttribute();
		MaxSpecialModifier.ModifierOp = EGameplayModOp::Override;
		MaxSpecialModifier.ModifierMagnitude = FScalableFloat(MaxSpecialAmmo);
		Modifiers.Add(MaxSpecialModifier);
	}

	//SpecialAmmo설정
	{
		FGameplayModifierInfo SpecialAmmoModifier;
		SpecialAmmoModifier.Attribute = UPlayerAttributeSet::GetSpecialAmmoAttribute();
		SpecialAmmoModifier.ModifierOp = EGameplayModOp::Override;
		SpecialAmmoModifier.ModifierMagnitude = FScalableFloat(SpecialAmmo);
		Modifiers.Add(SpecialAmmoModifier);
	}

	//MaxImpactAmmo설정
	{
		FGameplayModifierInfo MaxImpactAmmoModifier;
		MaxImpactAmmoModifier.Attribute = UPlayerAttributeSet::GetMaxImpactAmmoAttribute();
		MaxImpactAmmoModifier.ModifierOp = EGameplayModOp::Override;
		MaxImpactAmmoModifier.ModifierMagnitude = FScalableFloat(MaxImpactAmmo);
		Modifiers.Add(MaxImpactAmmoModifier);
	}
	
	//ImpactAmmo설정
	{
		FGameplayModifierInfo ImpactAmmoModifier;
		ImpactAmmoModifier.Attribute = UPlayerAttributeSet::GetImpactAmmoAttribute();
		ImpactAmmoModifier.ModifierOp = EGameplayModOp::Override;
		ImpactAmmoModifier.ModifierMagnitude = FScalableFloat(ImpactAmmo);
		Modifiers.Add(ImpactAmmoModifier);
	}

	//MaxHighPowerAmmo설정
	{
		FGameplayModifierInfo MaxHighPowerAmmoModifier;
		MaxHighPowerAmmoModifier.Attribute = UPlayerAttributeSet::GetMaxHighPowerAmmoAttribute();
		MaxHighPowerAmmoModifier.ModifierOp = EGameplayModOp::Override;
		MaxHighPowerAmmoModifier.ModifierMagnitude = FScalableFloat(MaxHighPowerAmmo);
		Modifiers.Add(MaxHighPowerAmmoModifier);
	}

	//HighPowerAmmo설정
	{
		FGameplayModifierInfo HighPowerAmmoModifier;
		HighPowerAmmoModifier.Attribute = UPlayerAttributeSet::GetHighPowerAmmoAttribute();
		HighPowerAmmoModifier.ModifierOp = EGameplayModOp::Override;
		HighPowerAmmoModifier.ModifierMagnitude = FScalableFloat(HighPowerAmmo);
		Modifiers.Add(HighPowerAmmoModifier);
	}

	{
		FGameplayModifierInfo WeaponDamageModifier;
		WeaponDamageModifier.Attribute = UPlayerAttributeSet::GetWeaponDamageAttribute();
		WeaponDamageModifier.ModifierOp = EGameplayModOp::Override;
		WeaponDamageModifier.ModifierMagnitude = FScalableFloat(WeaponDamage);
		Modifiers.Add(WeaponDamageModifier);
	}

	{
		FGameplayModifierInfo SkillDamageModifier;
		SkillDamageModifier.Attribute = UPlayerAttributeSet::GetSkillDamageAttribute();
		SkillDamageModifier.ModifierOp = EGameplayModOp::Override;
		SkillDamageModifier.ModifierMagnitude = FScalableFloat(SkillDamage);
		Modifiers.Add(SkillDamageModifier);
	}
	
	{
		FGameplayModifierInfo WeakDamageModifier;
		WeakDamageModifier.Attribute = UPlayerAttributeSet::GetWeakDamageAttribute();
		WeakDamageModifier.ModifierOp = EGameplayModOp::Override;
		WeakDamageModifier.ModifierMagnitude = FScalableFloat(WeakDamage);
		Modifiers.Add(WeakDamageModifier);
	}
	
	{
		FGameplayModifierInfo FireRateModifier;
		FireRateModifier.Attribute = UPlayerAttributeSet::GetFireRateAttribute();
		FireRateModifier.ModifierOp = EGameplayModOp::Override;
		FireRateModifier.ModifierMagnitude = FScalableFloat(FireRate);
		Modifiers.Add(FireRateModifier);
	}
	
	{
		FGameplayModifierInfo WeaponCriticalChanceModifier;
		WeaponCriticalChanceModifier.Attribute = UPlayerAttributeSet::GetWeaponCriticalChanceAttribute();
		WeaponCriticalChanceModifier.ModifierOp = EGameplayModOp::Override;
		WeaponCriticalChanceModifier.ModifierMagnitude = FScalableFloat(WeaponCriticalChance);
		Modifiers.Add(WeaponCriticalChanceModifier);
	}
	
	{
		FGameplayModifierInfo WeaponCriticalDamageModifier;
		WeaponCriticalDamageModifier.Attribute = UPlayerAttributeSet::GetWeaponCriticalDamageAttribute();
		WeaponCriticalDamageModifier.ModifierOp = EGameplayModOp::Override;
		WeaponCriticalDamageModifier.ModifierMagnitude = FScalableFloat(WeaponCriticalDamage);
		Modifiers.Add(WeaponCriticalDamageModifier);
	}
	
	{
		FGameplayModifierInfo SkillCriticalChanceModifier;
		SkillCriticalChanceModifier.Attribute = UPlayerAttributeSet::GetSkillCriticalChanceAttribute();
		SkillCriticalChanceModifier.ModifierOp = EGameplayModOp::Override;
		SkillCriticalChanceModifier.ModifierMagnitude = FScalableFloat(SkillCriticalChance);
		Modifiers.Add(SkillCriticalChanceModifier);
	}
	
	{
		FGameplayModifierInfo SkillCriticalDamageModifier;
		SkillCriticalDamageModifier.Attribute = UPlayerAttributeSet::GetSkillCriticalDamageAttribute();
		SkillCriticalDamageModifier.ModifierOp = EGameplayModOp::Override;
		SkillCriticalDamageModifier.ModifierMagnitude = FScalableFloat(SkillCriticalDamage);
		Modifiers.Add(SkillCriticalDamageModifier);
	}

	{
		FGameplayModifierInfo SkillCostModifier;
		SkillCostModifier.Attribute = UPlayerAttributeSet::GetSkillCostAttribute();
		SkillCostModifier.ModifierOp = EGameplayModOp::Override;
		SkillCostModifier.ModifierMagnitude = FScalableFloat(SkillCost);
		Modifiers.Add(SkillCostModifier);
	}

	{
		FGameplayModifierInfo SkillCost1Modifier;
		SkillCost1Modifier.Attribute = UPlayerAttributeSet::GetSkillCost1Attribute();
		SkillCost1Modifier.ModifierOp = EGameplayModOp::Override;
		SkillCost1Modifier.ModifierMagnitude = FScalableFloat(SkillCost1);
		Modifiers.Add(SkillCost1Modifier);
	}

	{
		FGameplayModifierInfo SkillCost2Modifier;
		SkillCost2Modifier.Attribute = UPlayerAttributeSet::GetSkillCost2Attribute();
		SkillCost2Modifier.ModifierOp = EGameplayModOp::Override;
		SkillCost2Modifier.ModifierMagnitude = FScalableFloat(SkillCost2);
		Modifiers.Add(SkillCost2Modifier);
	}

	{
		FGameplayModifierInfo SkillCost3Modifier;
		SkillCost3Modifier.Attribute = UPlayerAttributeSet::GetSkillCost3Attribute();
		SkillCost3Modifier.ModifierOp = EGameplayModOp::Override;
		SkillCost3Modifier.ModifierMagnitude = FScalableFloat(SkillCost3);
		Modifiers.Add(SkillCost3Modifier);
	}

	{
		FGameplayModifierInfo SkillCost4Modifier;
		SkillCost4Modifier.Attribute = UPlayerAttributeSet::GetSkillCost4Attribute();
		SkillCost4Modifier.ModifierOp = EGameplayModOp::Override;
		SkillCost4Modifier.ModifierMagnitude = FScalableFloat(SkillCost4);
		Modifiers.Add(SkillCost4Modifier);
	}
	
	{
		FGameplayModifierInfo SkillRadiusModifier;
		SkillRadiusModifier.Attribute = UPlayerAttributeSet::GetSkillRadiusAttribute();
		SkillRadiusModifier.ModifierOp = EGameplayModOp::Override;
		SkillRadiusModifier.ModifierMagnitude = FScalableFloat(SkillRadius);
		Modifiers.Add(SkillRadiusModifier);
	}
	
	{
		FGameplayModifierInfo SkillCooldownModifier;
		SkillCooldownModifier.Attribute = UPlayerAttributeSet::GetSkillCooldownAttribute();
		SkillCooldownModifier.ModifierOp = EGameplayModOp::Override;
		SkillCooldownModifier.ModifierMagnitude = FScalableFloat(SkillCooldown);
		Modifiers.Add(SkillCooldownModifier);
	}
	
	{
		FGameplayModifierInfo SkillDurationModifier;
		SkillDurationModifier.Attribute = UPlayerAttributeSet::GetSkillDurationAttribute();
		SkillDurationModifier.ModifierOp = EGameplayModOp::Override;
		SkillDurationModifier.ModifierMagnitude = FScalableFloat(SkillDuration);
		Modifiers.Add(SkillDurationModifier);
	}
	
	{
		FGameplayModifierInfo MaxModuleCapacityModifier;
		MaxModuleCapacityModifier.Attribute = UPlayerAttributeSet::GetMaxModuleCapacityAttribute();
		MaxModuleCapacityModifier.ModifierOp = EGameplayModOp::Override;
		MaxModuleCapacityModifier.ModifierMagnitude = FScalableFloat(MaxModuleCapacity);
		Modifiers.Add(MaxModuleCapacityModifier);
	}
	
	{
		FGameplayModifierInfo ModuleCapacityModifier;
		ModuleCapacityModifier.Attribute = UPlayerAttributeSet::GetModuleCapacityAttribute();
		ModuleCapacityModifier.ModifierOp = EGameplayModOp::Override;
		ModuleCapacityModifier.ModifierMagnitude = FScalableFloat(ModuleCapacity);
		Modifiers.Add(ModuleCapacityModifier);
	}
}
