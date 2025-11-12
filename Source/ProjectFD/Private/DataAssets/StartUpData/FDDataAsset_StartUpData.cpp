// Fill out your copyright notice in the Description page of Project Settings.


#include "DataAssets/StartUpData/FDDataAsset_StartUpData.h"

#include "GAS/FDAbilitySystemComponent.h"
#include "GAS/Abilities/FDGameplayAbility.h"


void UFDDataAsset_StartUpData::GiveToAbilitySystemComponent(UFDAbilitySystemComponent* InASCToGive, int32 ApplyLevel)
{
	if (!InASCToGive)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("!InASCToGive"));
	}
	
	GrantAbilities(ActivateOnGivenAbilities, InASCToGive, ApplyLevel);
	GrantAbilities(ReactiveAbilities, InASCToGive, ApplyLevel);

	if (!StartUpGameplayEffect.IsEmpty())
	{
		for (const TSubclassOf<UGameplayEffect>& EffectClass : StartUpGameplayEffect)
		{
			if (!EffectClass)
			{
				continue;
			}

			UGameplayEffect* EffectCDO = EffectClass->GetDefaultObject<UGameplayEffect>();
			
			InASCToGive->ApplyGameplayEffectToSelf(
				EffectCDO,
				ApplyLevel,
				InASCToGive->MakeEffectContext()
			);
		}
	}
}

void UFDDataAsset_StartUpData::GrantAbilities(const TArray<TSubclassOf<UFDGameplayAbility>>& InAbilitiesToGive,
	UFDAbilitySystemComponent* InASCToGive, int32 ApplyLevel)
{
	if (InAbilitiesToGive.IsEmpty())
	{
		return;
	}

	for (const TSubclassOf<UFDGameplayAbility>& Ability : InAbilitiesToGive)
	{
		if (!Ability)
		{
			continue;
		}

		FGameplayAbilitySpec AbilitySpec(Ability);
		AbilitySpec.SourceObject = InASCToGive->GetAvatarActor();
		AbilitySpec.Level = ApplyLevel;
		
		InASCToGive->GiveAbility(AbilitySpec);
	}
}
