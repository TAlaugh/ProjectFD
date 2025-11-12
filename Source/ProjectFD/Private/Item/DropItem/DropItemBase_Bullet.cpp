// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/DropItem/DropItemBase_Bullet.h"

#include "AbilitySystemComponent.h"
#include "Character/Player/FDPlayerCharacter.h"
#include "GAS/Player/PlayerAttributeSet.h"
#include "Inventory/InventoryComponent.h"


void ADropItemBase_Bullet::HandlePickUp(AFDPlayerCharacter* Player)
{
	if (!HasAuthority())
	{
		return;
	}
	
	switch (BulletType)
	{
	case EBulletType::Enhanced:
		BulletEnhanced(Player);
		break;
	case EBulletType::General:
		BulletGeneral(Player);
		break;
	case EBulletType::HighPower:
		BulletHighPower(Player);
		break;
	case EBulletType::Impact:
		BulletImpact(Player);
		break;
	default:
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Magenta, "None");
		break;
	}
}

void ADropItemBase_Bullet::BulletEnhanced(AFDPlayerCharacter* Player)
{
	Player->GetAbilitySystemComponent()->SetNumericAttributeBase(UPlayerAttributeSet::GetSpecialAmmoAttribute(),
		Player->GetAbilitySystemComponent()->GetNumericAttributeBase(UPlayerAttributeSet::GetSpecialAmmoAttribute()) + 80);
}

void ADropItemBase_Bullet::BulletGeneral(AFDPlayerCharacter* Player)
{
	Player->GetAbilitySystemComponent()->SetNumericAttributeBase(UPlayerAttributeSet::GetGeneralAmmoAttribute(),
		Player->GetAbilitySystemComponent()->GetNumericAttributeBase(UPlayerAttributeSet::GetGeneralAmmoAttribute()) + 200);
}

void ADropItemBase_Bullet::BulletImpact(AFDPlayerCharacter* Player)
{
	Player->GetAbilitySystemComponent()->SetNumericAttributeBase(UPlayerAttributeSet::GetImpactAmmoAttribute(),
		Player->GetAbilitySystemComponent()->GetNumericAttributeBase(UPlayerAttributeSet::GetImpactAmmoAttribute()) + 80);
}

void ADropItemBase_Bullet::BulletHighPower(AFDPlayerCharacter* Player)
{
	Player->GetAbilitySystemComponent()->SetNumericAttributeBase(UPlayerAttributeSet::GetHighPowerAmmoAttribute(),
		Player->GetAbilitySystemComponent()->GetNumericAttributeBase(UPlayerAttributeSet::GetHighPowerAmmoAttribute()) + 5);
}
