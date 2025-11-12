// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GAS/FDAttributeSet.h"
#include "PlayerAttributeSet.generated.h"
class UPlayerAttributeSet;
class AFDPlayerCharacter;

/**
 * 
 */
UCLASS()
class PROJECTFD_API UPlayerAttributeSet : public UFDAttributeSet
{
	GENERATED_BODY()
	
public:
	UPROPERTY(ReplicatedUsing= OnRep_Mana)
	FGameplayAttributeData Mana;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, Mana);
	UPROPERTY(ReplicatedUsing= OnRep_MaxMana)
	FGameplayAttributeData MaxMana;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, MaxMana);
	UPROPERTY(ReplicatedUsing= OnRep_Stamina)
	FGameplayAttributeData Stamina;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, Stamina);
	UPROPERTY(ReplicatedUsing= OnRep_MaxStamina)
	FGameplayAttributeData MaxStamina;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, MaxStamina);
	UPROPERTY(ReplicatedUsing= OnRep_SpecialResource)
	FGameplayAttributeData SpecialResource;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, SpecialResource);
	UPROPERTY(ReplicatedUsing= OnRep_MaxSpecialResource)
	FGameplayAttributeData MaxSpecialResource;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, MaxSpecialResource);
	
	// Apply Damage
	UPROPERTY(ReplicatedUsing = OnRep_WeaponDamage)
	FGameplayAttributeData WeaponDamage;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, WeaponDamage);

	UPROPERTY(ReplicatedUsing= OnRep_WeakDamage)
	FGameplayAttributeData WeakDamage;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, WeakDamage);
	
	UPROPERTY(ReplicatedUsing= OnRep_FireRate)
	FGameplayAttributeData FireRate;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, FireRate);
	
	UPROPERTY(ReplicatedUsing= OnRep_WeaponCriticalChance)
	FGameplayAttributeData WeaponCriticalChance;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, WeaponCriticalChance);
	
	UPROPERTY(ReplicatedUsing= OnRep_WeaponCriticalDamage)
	FGameplayAttributeData WeaponCriticalDamage;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, WeaponCriticalDamage);
	
	UPROPERTY(ReplicatedUsing= OnRep_SkillCriticalChance)
	FGameplayAttributeData SkillCriticalChance;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, SkillCriticalChance);
	
	UPROPERTY(ReplicatedUsing= OnRep_SkillCriticalDamage)
	FGameplayAttributeData SkillCriticalDamage;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, SkillCriticalDamage);
	//

	// AMMO
	UPROPERTY(ReplicatedUsing= OnRep_Ammo)
	FGameplayAttributeData Ammo;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, Ammo);
	UPROPERTY(ReplicatedUsing= OnRep_MaxAmmo)
	FGameplayAttributeData MaxAmmo;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, MaxAmmo);
	UPROPERTY(ReplicatedUsing= OnRep_GeneralAmmo)
	FGameplayAttributeData GeneralAmmo;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, GeneralAmmo);
	UPROPERTY(ReplicatedUsing= OnRep_MaxGeneralAmmo)
	FGameplayAttributeData MaxGeneralAmmo;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, MaxGeneralAmmo);
	UPROPERTY(ReplicatedUsing= OnRep_SpecialAmmo)
	FGameplayAttributeData SpecialAmmo;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, SpecialAmmo);
	UPROPERTY(ReplicatedUsing= OnRep_MaxSpecialAmmo)
	FGameplayAttributeData MaxSpecialAmmo;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, MaxSpecialAmmo);
	UPROPERTY(ReplicatedUsing= OnRep_ImpactAmmo)
	FGameplayAttributeData ImpactAmmo;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, ImpactAmmo);
	UPROPERTY(ReplicatedUsing= OnRep_MaxImpactAmmo)
	FGameplayAttributeData MaxImpactAmmo;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, MaxImpactAmmo);
	UPROPERTY(ReplicatedUsing= OnRep_HighPowerAmmo)
	FGameplayAttributeData HighPowerAmmo;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, HighPowerAmmo);
	UPROPERTY(ReplicatedUsing= OnRep_MaxHighPowerAmmo)
	FGameplayAttributeData MaxHighPowerAmmo;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, MaxHighPowerAmmo);
	
	// Skill
	UPROPERTY(ReplicatedUsing = OnRep_SkillDamage)
	FGameplayAttributeData SkillDamage;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, SkillDamage);

	UPROPERTY(ReplicatedUsing = OnRep_SkillRadius)
	FGameplayAttributeData SkillRadius;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, SkillRadius);

	UPROPERTY(ReplicatedUsing = OnRep_SkillCooldown)
	FGameplayAttributeData SkillCooldown;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, SkillCooldown);

	UPROPERTY(ReplicatedUsing = OnRep_SkillDuration)
	FGameplayAttributeData SkillDuration;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, SkillDuration);

	UPROPERTY(ReplicatedUsing = OnRep_SkillCost)
	FGameplayAttributeData SkillCost;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, SkillCost);
	
	UPROPERTY(ReplicatedUsing = OnRep_SkillCost1)
	FGameplayAttributeData SkillCost1;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, SkillCost1);
	
	UPROPERTY(ReplicatedUsing = OnRep_SkillCost2)
	FGameplayAttributeData SkillCost2;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, SkillCost2);
	
	UPROPERTY(ReplicatedUsing = OnRep_SkillCost3)
	FGameplayAttributeData SkillCost3;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, SkillCost3);
	
	UPROPERTY(ReplicatedUsing = OnRep_SkillCost4)
	FGameplayAttributeData SkillCost4;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, SkillCost4);

	// Module
	UPROPERTY(ReplicatedUsing = OnRep_ModuleCapacity)
	FGameplayAttributeData ModuleCapacity;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, ModuleCapacity);

	UPROPERTY(ReplicatedUsing = OnRep_MaxModuleCapacity)
	FGameplayAttributeData MaxModuleCapacity;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, MaxModuleCapacity);

	virtual void OnRep_Health(const FGameplayAttributeData& OldValue) override;
	virtual void OnRep_MaxHealth(const FGameplayAttributeData& OldValue) override;
	virtual void OnRep_Shield(const FGameplayAttributeData& OldValue) override;
	virtual void OnRep_MaxShield(const FGameplayAttributeData& OldValue) override;
	
	UFUNCTION()
	void OnRep_Mana(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_MaxMana(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_Stamina(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_MaxStamina(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_SpecialResource(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_MaxSpecialResource(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_Ammo(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_MaxAmmo(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_GeneralAmmo(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_MaxGeneralAmmo(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_SpecialAmmo(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_MaxSpecialAmmo(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_ImpactAmmo(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_MaxImpactAmmo(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_HighPowerAmmo(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_MaxHighPowerAmmo(const FGameplayAttributeData& OldValue);

	// Apply Damage
	UFUNCTION()
	void OnRep_WeaponDamage(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_SkillDamage(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_WeakDamage(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_WeaponCriticalChance(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_WeaponCriticalDamage(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_SkillCriticalChance(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_SkillCriticalDamage(const FGameplayAttributeData& OldValue);
	
	// Skill
	UFUNCTION()
	void OnRep_SkillRadius(const FGameplayAttributeData& OldValue);
	
	UFUNCTION()
	void OnRep_SkillCooldown(const FGameplayAttributeData& OldValue);
	
	UFUNCTION()
	void OnRep_SkillDuration(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_SkillCost(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_SkillCost1(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_SkillCost2(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_SkillCost3(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_SkillCost4(const FGameplayAttributeData& OldValue);
	
	// Weapon
	UFUNCTION()
	void OnRep_FireRate(const FGameplayAttributeData& OldValue);

	// Module	
	UFUNCTION()
	void OnRep_ModuleCapacity(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_MaxModuleCapacity(const FGameplayAttributeData& OldValue);
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;

	AFDPlayerCharacter* GetPlayerCharacter();

	// 마나가 0 이하로 떨어졌을 때 호출될 델리게이트
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnManaDepleted);
	FOnManaDepleted OnManaDepleted;
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHealthDepleted);
	FOnHealthDepleted OnHealthDepleted;
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSpecialResourceDepleted);
	FOnSpecialResourceDepleted OnSpecialResourceDepleted;
};
