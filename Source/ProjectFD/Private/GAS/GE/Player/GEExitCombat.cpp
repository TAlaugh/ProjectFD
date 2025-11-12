// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GE/Player/GEExitCombat.h"

#include "GAS/FDGameplayStatic.h"


UGEExitCombat::UGEExitCombat()
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;
	
	InheritableOwnedTagsContainer.AddTag(UFDGameplayStatic::GetOwnerNormalTag());
}
