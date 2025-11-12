// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Combat/PawnCombatComponent.h"
#include "Item/Weapon/WeaponBase.h"
#include "Net/UnrealNetwork.h"

UPawnCombatComponent::UPawnCombatComponent()
{
	SetIsReplicatedByDefault(true);
}

void UPawnCombatComponent::EquippedWeapon(AWeaponBase* Weapon)
{
	EquippedWeapons.Add(Weapon);
}

AWeaponBase* UPawnCombatComponent::GetEquippedWeapon(EWeaponType Type)
{
	for (AWeaponBase* Weapon : EquippedWeapons)
	{
		if (Weapon->WeaponType == Type)
		{
			return Weapon;
		}
	}
	
	return nullptr;
}

void UPawnCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UPawnCombatComponent, EquippedWeapons);
}
