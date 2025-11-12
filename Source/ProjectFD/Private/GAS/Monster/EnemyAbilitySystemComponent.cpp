// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Monster/EnemyAbilitySystemComponent.h"

#include "Character/Monster/FDMonsterCharacter.h"


void UEnemyAbilitySystemComponent::OnAbilityInputPressed(const FGameplayTag& InInputTag)
{
	if (!InInputTag.IsValid())
	{
		return;
	}

	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (!AbilitySpec.DynamicAbilityTags.HasTagExact(InInputTag))
		{
			continue;
		}
		
		TryActivateAbility(AbilitySpec.Handle);
	}
}

void UEnemyAbilitySystemComponent::OnAbilityInputReleased(const FGameplayTag& InInputTag)
{
	
}

bool UEnemyAbilitySystemComponent::TryActivateAbilityByTag(FGameplayTag AbilityTagToActivates)
{
	if (AbilityTagToActivates.IsValid())
	{
		TArray<FGameplayAbilitySpec*> FoundAbilitySpecs;
		GetActivatableGameplayAbilitySpecsByAllMatchingTags(AbilityTagToActivates.GetSingleTagContainer(), FoundAbilitySpecs);

		if (!FoundAbilitySpecs.IsEmpty())
		{
			const int32 RandomAbilityIndex = FMath::RandRange(0, FoundAbilitySpecs.Num() - 1);
			FGameplayAbilitySpec* SpecToActivate = FoundAbilitySpecs[RandomAbilityIndex];

			if (SpecToActivate)
			{
				if (!SpecToActivate->IsActive())
				{
					return TryActivateAbility(SpecToActivate->Handle);
				}
			}
		}
	}
	return false;
}
