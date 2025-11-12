// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/DropItem/DropItemBase_Equipment.h"

#include "Character/Player/FDPlayerCharacter.h"
#include "Inventory/InventoryComponent.h"
#include "Inventory/ItemTypes.h"


void ADropItemBase_Equipment::HandlePickUp(AFDPlayerCharacter* Player)
{
	if (!HasAuthority())
	{
		return;
	}
	
	switch (EquipmentType)
	{
	case EEquipmentType::Tier1:
		EquipmentTierOne(Player);
		break;
	case EEquipmentType::Tier2:
		EquipmentTierTwo(Player);
		break;
	case EEquipmentType::Tier3:
		EquipmentTierThree(Player);
		break;
	default:
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Magenta, "None");
		break;
	}
}

void ADropItemBase_Equipment::EquipmentTierOne(AFDPlayerCharacter* Player)
{
	FItemAcquisitionInfo Iteminfo;
	Iteminfo.MinLevel = 10;
	Iteminfo.MaxLevel = 20;
	Player->GetInventoryComponent_Implementation()->AddItem(FGameplayTag::RequestGameplayTag("Item.Equipment.Weapon.LNC.VestigialOrgan"), Iteminfo, 1);
}

void ADropItemBase_Equipment::EquipmentTierTwo(AFDPlayerCharacter* Player)
{
	FItemAcquisitionInfo Iteminfo;
	Iteminfo.MinLevel = 10;
	Iteminfo.MaxLevel = 20;
	Player->GetInventoryComponent_Implementation()->AddItem(FGameplayTag::RequestGameplayTag("Item.Equipment.Weapon.MG.EnduringLegacy"), Iteminfo, 1);
}

void ADropItemBase_Equipment::EquipmentTierThree(AFDPlayerCharacter* Player)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, "Equipment Tier3");
}
