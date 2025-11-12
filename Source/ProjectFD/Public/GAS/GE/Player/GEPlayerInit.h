// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "GEPlayerInit.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTFD_API UGEPlayerInit : public UGameplayEffect
{
	GENERATED_BODY()
	
public:
	UGEPlayerInit();

private:
	float MaxHealth = 200.f;  //200
	float MaxMana = 100.f;
	float MaxShield = 200.f;  //200
	float MaxStamina = 100.f;
	float MaxSpecialResource = 300.f;
	float Ammo = 0.f;
	float MaxAmmo = 0.f;
	float GeneralAmmo = 200.f;
	float MaxGeneralAmmo = 350.f;
	float SpecialAmmo = 150.f;
	float MaxSpecialAmmo = 230.f;
	float ImpactAmmo = 120.f;
	float MaxImpactAmmo = 200.f;
	float HighPowerAmmo = 10.f;
	float MaxHighPowerAmmo = 20.f;
	float WeaponDamage = 0.f;
	float WeakDamage = 2.f;
	float FireRate = 1.f;
	float WeaponCriticalDamage = 1.5f;
	float WeaponCriticalChance = 0.05f;
	float SkillCriticalDamage = 1.5f;
	float SkillCriticalChance = 0.05f;
	float SkillDamage = 100.f;
	float SkillCost = 1.f;
	float SkillRadius = 1.f;
	float SkillCooldown = 1.f;
	float SkillDuration = 1.f;
	float ModuleCapacity = 0.f;
	float MaxModuleCapacity = 48.f;
	float SkillCost1 = 50.f;
	float SkillCost2 = 12.f;
	float SkillCost3 = 100.f;
	float SkillCost4 = 50.f;
};
