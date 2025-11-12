// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Player/FDDummyCharacter.h"

#include "AbilitySystemComponent.h"
#include "Gameframework/FDPlayerState.h"
#include "GAS/Player/PlayerAttributeSet.h"


AFDDummyCharacter::AFDDummyCharacter()
{
	ASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));
	ASC->SetIsReplicated(true);
	AttributeSet = CreateDefaultSubobject<UFDAttributeSet>(TEXT("AttributeSet"));

	bReplicates = true;
	TeamID = 2;
	Level = 1.f;
}

UAbilitySystemComponent* AFDDummyCharacter::GetAbilitySystemComponent() const
{
	return ASC;
}

void AFDDummyCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	if (HasAuthority())
	{
		ASC->InitAbilityActorInfo(this, this);
		for ( const TSubclassOf<UGameplayEffect>& GEClass : InitStatEffect)
		{
			FGameplayEffectContextHandle EffectContextHandle = ASC->MakeEffectContext();
			EffectContextHandle.AddSourceObject(this);
			FGameplayEffectSpecHandle EffectSpecHandle = ASC->MakeOutgoingSpec(GEClass, Level, EffectContextHandle);
			if (EffectSpecHandle.IsValid())
			{
				ASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
			}
		}
	}
}

