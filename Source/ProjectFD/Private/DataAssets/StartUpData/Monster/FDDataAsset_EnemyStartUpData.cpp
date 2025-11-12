// Fill out your copyright notice in the Description page of Project Settings.


#include "DataAssets/StartUpData/Monster/FDDataAsset_EnemyStartUpData.h"

#include "GAS/FDAbilitySystemComponent.h"
#include "GAS/Abilities/Monster/EnemyGameplayAbility.h"


void UFDDataAsset_EnemyStartUpData::GiveToAbilitySystemComponent(UFDAbilitySystemComponent* InASCToGive,
                                                                 int32 ApplyLevel)
{
	Super::GiveToAbilitySystemComponent(InASCToGive, ApplyLevel);

	if (!EnemyCombatAbilities.IsEmpty())
	{
		for (const TSubclassOf<UEnemyGameplayAbility>& AbilityClass : EnemyCombatAbilities)
		{
			if (!AbilityClass)
			{
				continue;
			}

			FGameplayAbilitySpec AbilitySpec(AbilityClass);
			AbilitySpec.SourceObject = InASCToGive->GetAvatarActor();
			AbilitySpec.Level = ApplyLevel;
			
			InASCToGive->GiveAbility(AbilitySpec);
		}
	}

	if (!DestroyAbilities.IsEmpty())
	{
		for (const TSubclassOf<UEnemyGameplayAbility>& AbilityClass : DestroyAbilities)
		{
			if (!AbilityClass)
			{
				continue;
			}

			FGameplayAbilitySpec AbilitySpec(AbilityClass);
			AbilitySpec.SourceObject = InASCToGive->GetAvatarActor();
			AbilitySpec.Level = ApplyLevel;
			
			InASCToGive->GiveAbility(AbilitySpec);
		}
	}
}
