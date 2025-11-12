// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Player/PlayerAttributeSet.h"

#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"
#include "Character/Player/FDPlayerCharacter.h"
#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"


void UPlayerAttributeSet::OnRep_Health(const FGameplayAttributeData& OldValue)
{
	Super::OnRep_Health(OldValue);
}

void UPlayerAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldValue)
{
	Super::OnRep_MaxHealth(OldValue);
}

void UPlayerAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, Mana, OldValue);
}

void UPlayerAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, MaxMana, OldValue);
}

void UPlayerAttributeSet::OnRep_Shield(const FGameplayAttributeData& OldValue)
{
	Super::OnRep_Shield(OldValue);
}

void UPlayerAttributeSet::OnRep_MaxShield(const FGameplayAttributeData& OldValue)
{
	Super::OnRep_MaxShield(OldValue);
}

void UPlayerAttributeSet::OnRep_Stamina(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, Stamina, OldValue);
}

void UPlayerAttributeSet::OnRep_MaxStamina(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, MaxStamina, OldValue);
}

void UPlayerAttributeSet::OnRep_SpecialResource(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, SpecialResource, OldValue);
}

void UPlayerAttributeSet::OnRep_MaxSpecialResource(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, MaxSpecialResource, OldValue);
}

void UPlayerAttributeSet::OnRep_Ammo(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, Ammo, OldValue);
}

void UPlayerAttributeSet::OnRep_MaxAmmo(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, MaxAmmo, OldValue);
}

void UPlayerAttributeSet::OnRep_GeneralAmmo(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, GeneralAmmo, OldValue);
}

void UPlayerAttributeSet::OnRep_MaxGeneralAmmo(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, MaxGeneralAmmo, OldValue);
}

void UPlayerAttributeSet::OnRep_SpecialAmmo(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, SpecialAmmo, OldValue);
}

void UPlayerAttributeSet::OnRep_MaxSpecialAmmo(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, MaxSpecialAmmo, OldValue);
}

void UPlayerAttributeSet::OnRep_ImpactAmmo(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, ImpactAmmo, OldValue);
}

void UPlayerAttributeSet::OnRep_MaxImpactAmmo(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, MaxImpactAmmo, OldValue);
}

void UPlayerAttributeSet::OnRep_HighPowerAmmo(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, HighPowerAmmo, OldValue);
}

void UPlayerAttributeSet::OnRep_MaxHighPowerAmmo(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, MaxHighPowerAmmo, OldValue);
}

void UPlayerAttributeSet::OnRep_WeaponDamage(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, WeaponDamage, OldValue);
}

void UPlayerAttributeSet::OnRep_WeakDamage(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, WeakDamage, OldValue);
}

void UPlayerAttributeSet::OnRep_FireRate(const FGameplayAttributeData& OldValue)
{	
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, FireRate, OldValue);
}

void UPlayerAttributeSet::OnRep_ModuleCapacity(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, ModuleCapacity, OldValue);
}

void UPlayerAttributeSet::OnRep_MaxModuleCapacity(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, MaxModuleCapacity, OldValue);
}

void UPlayerAttributeSet::OnRep_WeaponCriticalChance(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, WeaponCriticalChance, OldValue);
}

void UPlayerAttributeSet::OnRep_WeaponCriticalDamage(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, WeaponCriticalDamage, OldValue);
}

void UPlayerAttributeSet::OnRep_SkillCriticalChance(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, SkillCriticalChance, OldValue);
}

void UPlayerAttributeSet::OnRep_SkillCriticalDamage(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, SkillCriticalDamage, OldValue);
}

void UPlayerAttributeSet::OnRep_SkillDamage(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, SkillDamage, OldValue);
}

void UPlayerAttributeSet::OnRep_SkillRadius(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, SkillRadius, OldValue);
}

void UPlayerAttributeSet::OnRep_SkillCooldown(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, SkillCooldown, OldValue);
}

void UPlayerAttributeSet::OnRep_SkillDuration(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, SkillDuration, OldValue);
}

void UPlayerAttributeSet::OnRep_SkillCost(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, SkillCost, OldValue);
}

void UPlayerAttributeSet::OnRep_SkillCost1(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, SkillCost1, OldValue);
}

void UPlayerAttributeSet::OnRep_SkillCost2(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, SkillCost2, OldValue);
}

void UPlayerAttributeSet::OnRep_SkillCost3(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, SkillCost3, OldValue);
}

void UPlayerAttributeSet::OnRep_SkillCost4(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, SkillCost4, OldValue);
}

void UPlayerAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicated로 속성들 동기화
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, Mana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, Stamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, MaxStamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, SpecialResource, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, MaxSpecialResource, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, Ammo, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, MaxAmmo, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, GeneralAmmo, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, MaxGeneralAmmo, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, SpecialAmmo, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, MaxSpecialAmmo, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, ImpactAmmo, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, MaxImpactAmmo, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, HighPowerAmmo, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, MaxHighPowerAmmo, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, WeaponDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, SkillDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, WeakDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, FireRate, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, WeaponCriticalChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, WeaponCriticalDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, SkillCriticalChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, SkillCriticalDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, SkillRadius, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, SkillCooldown, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, SkillDuration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, SkillCost, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, SkillCost1, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, SkillCost2, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, SkillCost3, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, SkillCost4, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, ModuleCapacity, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, MaxModuleCapacity, COND_None, REPNOTIFY_Always);
}

void UPlayerAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
	}

	if (Attribute == GetManaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxMana());
	}
	
	if (Attribute == GetShieldAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxShield());
	}

	if (Attribute == GetStaminaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxStamina());
	}

	if (Attribute == GetSpecialResourceAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxSpecialResource());
	}
	
	if (Attribute == GetAmmoAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxAmmo());
	}

	if (Attribute == GetGeneralAmmoAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxGeneralAmmo());
	}
	
	if (Attribute == GetSpecialAmmoAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxSpecialAmmo());
	}
	
	if (Attribute == GetImpactAmmoAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxImpactAmmo());
	}
	
	if (Attribute == GetHighPowerAmmoAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHighPowerAmmo());
	}
	
	if (Attribute == GetModuleCapacityAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxModuleCapacity());
	}
	
	if (Attribute == GetMaxShieldAttribute())
	{
		if (UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent())
		{
			ASC->ApplyModToAttributeUnsafe(GetShieldAttribute(), EGameplayModOp::Override, NewValue);
		}
	}
}

void UPlayerAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

}

void UPlayerAttributeSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0, GetMaxHealth()));

		// 체력이 0 이하로 떨어졌는지 확인하고, 그렇다면 델리게이트 호출
		if (GetHealth() <= 0.0f)
		{
			OnHealthDepleted.Broadcast();
		}
	}

	if (Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		SetMana(FMath::Clamp(GetMana(), 0, GetMaxMana()));

		// 마나가 0 이하로 떨어졌는지 확인하고, 그렇다면 델리게이트 호출
		if (GetMana() <= 0.0f)
		{
			OnManaDepleted.Broadcast();
		}
	}

	if (Data.EvaluatedData.Attribute == GetShieldAttribute())
	{
		SetShield(FMath::Clamp(GetShield(), 0, GetMaxShield()));
	}

	if (Data.EvaluatedData.Attribute == GetStaminaAttribute())
	{
		SetStamina(FMath::Clamp(GetStamina(), 0, GetMaxStamina()));
	}

	if (Data.EvaluatedData.Attribute == GetSpecialResourceAttribute())
	{
		SetSpecialResource(FMath::Clamp(GetSpecialResource(), 0, GetMaxSpecialResource()));

		if (GetSpecialResource() <= 0.0f)
		{
			OnSpecialResourceDepleted.Broadcast();
		}
	}

	if (Data.EvaluatedData.Attribute == GetAmmoAttribute())
	{
		SetAmmo(FMath::Clamp(GetAmmo(), 0, GetMaxAmmo()));
	}

	if (Data.EvaluatedData.Attribute == GetSpecialAmmoAttribute())
	{
		SetSpecialAmmo(FMath::Clamp(GetSpecialAmmo(), 0, GetMaxSpecialAmmo()));
	}

	if (Data.EvaluatedData.Attribute == GetImpactAmmoAttribute())
	{
		SetImpactAmmo(FMath::Clamp(GetImpactAmmo(), 0, GetMaxImpactAmmo()));
	}

	if (Data.EvaluatedData.Attribute == GetHighPowerAmmoAttribute())
	{
		SetHighPowerAmmo(FMath::Clamp(GetHighPowerAmmo(), 0, GetMaxHighPowerAmmo()));
	}
	
}

AFDPlayerCharacter* UPlayerAttributeSet::GetPlayerCharacter()
{
	if (const UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent())
	{
		if (APlayerState* PS = Cast<APlayerState>(ASC->GetOwner()))
		{
			if (APawn* Pawn = PS->GetPawn())
			{
				return Cast<AFDPlayerCharacter>(Pawn);
			}
		}
	}

	return nullptr;
}
