// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GE/Player/GEToggleCombatState.h"

#include "GAS/FDGameplayStatic.h"
#include "GAS/Player/PlayerAbilitySystemComponent.h"


UGEToggleCombatState::UGEToggleCombatState()
{
	if (!ASC) return;

	FGameplayTag CombatTag = UFDGameplayStatic::GetOwnerCombatTag();
	FGameplayTag NormalTag = UFDGameplayStatic::GetOwnerNormalTag();

	if (ASC->HasMatchingGameplayTag(NormalTag))
	{
		ASC->RemoveLooseGameplayTag(NormalTag);
		ASC->AddLooseGameplayTag(CombatTag);
	}
	else if (ASC->HasMatchingGameplayTag(CombatTag))
	{
		ASC->RemoveLooseGameplayTag(CombatTag);
		ASC->AddLooseGameplayTag(NormalTag);
	}
}
