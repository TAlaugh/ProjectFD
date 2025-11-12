// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Monster/EnemyAttributeSet.h"

#include "FunctionLibrary/FDFunctionLibrary.h"
#include "GAS/FDGameplayStatic.h"
#include "Net/UnrealNetwork.h"


UEnemyAttributeSet::UEnemyAttributeSet()
{
	InitHealth(1.f);
	InitMaxHealth(1.f);
	InitBerserkGage(0.f);
	InitMaxBerserkGage(100.f);
	InitShield(1.f);
	InitMaxShield(1.f);
	InitAttack(1.f);
	InitDefence(1.f);
}

void UEnemyAttributeSet::OnRep_Health(const FGameplayAttributeData& OldValue)
{
	Super::OnRep_Health(OldValue);
}

void UEnemyAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldValue)
{
	Super::OnRep_MaxHealth(OldValue);
}

void UEnemyAttributeSet::OnRep_BerserkGage(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UEnemyAttributeSet, BerserkGage, OldValue);
}

void UEnemyAttributeSet::OnRep_MaxBerserkGage(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UEnemyAttributeSet, MaxBerserkGage, OldValue);
}

void UEnemyAttributeSet::OnRep_Shield(const FGameplayAttributeData& OldValue)
{
	Super::OnRep_Shield(OldValue);
}

void UEnemyAttributeSet::OnRep_MaxShield(const FGameplayAttributeData& OldValue)
{
	Super::OnRep_MaxShield(OldValue);
}

void UEnemyAttributeSet::OnRep_Attack(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UEnemyAttributeSet, Attack, OldValue);
}

void UEnemyAttributeSet::OnRep_Defence(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UEnemyAttributeSet, Defence, OldValue);
}

void UEnemyAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicated
	DOREPLIFETIME_CONDITION_NOTIFY(UEnemyAttributeSet, BerserkGage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UEnemyAttributeSet, MaxBerserkGage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UEnemyAttributeSet, Attack, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UEnemyAttributeSet, Defence, COND_None, REPNOTIFY_Always);
}
