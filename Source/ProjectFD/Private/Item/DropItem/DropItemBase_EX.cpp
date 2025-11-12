// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/DropItem/DropItemBase_EX.h"

#include "Character/Player/FDPlayerCharacter.h"
#include "GAS/Player/PlayerAttributeSet.h"


void ADropItemBase_EX::HandlePickUp(AFDPlayerCharacter* Player)
{
	if (!HasAuthority())
	{
		return;
	}
	
	switch (EXType)
	{
	case EEXType::Heal:
		EXHpHeal(Player);
		break;
	case EEXType::MpHeal:
		EXMpHeal(Player);
		break;
	case EEXType::Buff:
		EXBuff(Player);
		break;
	default:
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Magenta, "None");
		break;
	}
}

void ADropItemBase_EX::EXHpHeal(AFDPlayerCharacter* Player)
{
	Player->GetAbilitySystemComponent()->SetNumericAttributeBase(UPlayerAttributeSet::GetHealthAttribute(),
		Player->GetAbilitySystemComponent()->GetNumericAttributeBase(UPlayerAttributeSet::GetHealthAttribute()) +
		Player->GetAbilitySystemComponent()->GetNumericAttributeBase(UPlayerAttributeSet::GetMaxHealthAttribute()) * 0.3f);
}

void ADropItemBase_EX::EXMpHeal(AFDPlayerCharacter* Player)
{
	Player->GetAbilitySystemComponent()->SetNumericAttributeBase(UPlayerAttributeSet::GetManaAttribute(),
		Player->GetAbilitySystemComponent()->GetNumericAttributeBase(UPlayerAttributeSet::GetManaAttribute()) +
		Player->GetAbilitySystemComponent()->GetNumericAttributeBase(UPlayerAttributeSet::GetMaxManaAttribute()) * 0.4f);
}

void ADropItemBase_EX::EXBuff(AFDPlayerCharacter* Player)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, "Buff");
}
