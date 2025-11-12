// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GE/Player/GEEnterCombat.h"

#include "GAS/FDGameplayStatic.h"


UGEEnterCombat::UGEEnterCombat()
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	InheritableOwnedTagsContainer.AddTag(UFDGameplayStatic::GetOwnerCombatTag());
}