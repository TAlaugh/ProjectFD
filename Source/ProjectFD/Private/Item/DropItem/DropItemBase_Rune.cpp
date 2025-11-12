// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/DropItem/DropItemBase_Rune.h"

#include "Character/Player/FDPlayerCharacter.h"
#include "Inventory/InventoryComponent.h"
#include "Inventory/ItemTypes.h"


void ADropItemBase_Rune::HandlePickUp(AFDPlayerCharacter* Player)
{
	if (!HasAuthority())
	{
		return;
	}
	
	switch (RuneType)
	{
	case ERuneType::Tier1:
		RuneTierOne(Player);
		break;
	case ERuneType::Tier2:
		RuneTierTwo(Player);
		break;
	case ERuneType::Tier3:
		RuneTierThree(Player);
		break;
	case ERuneType::Tier4:
		RuneTierFour(Player);
		break;
	default:
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Magenta, "None");
		break;
	}
}

void ADropItemBase_Rune::RuneTierOne(AFDPlayerCharacter* Player)
{
	TArray<FName> RuneTags = {
		"Item.Equipment.Module.Player.SkillConcentration",
		"Item.Equipment.Module.Player.SkillInsight",
		"Item.Equipment.Module.Player.IncreasedShield",
	};
	
	int RandIndex = FMath::RandRange(0, RuneTags.Num() - 1);
	
	FItemAcquisitionInfo Iteminfo;
	Iteminfo.MinLevel = 1;
	Iteminfo.MaxLevel = 10;
	
	Player->GetInventoryComponent_Implementation()->AddItem(FGameplayTag::RequestGameplayTag(RuneTags[RandIndex]), Iteminfo, 1);
}

void ADropItemBase_Rune::RuneTierTwo(AFDPlayerCharacter* Player)
{
	FItemAcquisitionInfo Iteminfo;
	Iteminfo.MinLevel = 1;
	Iteminfo.MaxLevel = 10;
	Player->GetInventoryComponent_Implementation()->AddItem(FGameplayTag::RequestGameplayTag("Item.Equipment.Module.Player.EmergencyMeasures"), Iteminfo, 1);
}

void ADropItemBase_Rune::RuneTierThree(AFDPlayerCharacter* Player)
{
	FItemAcquisitionInfo Iteminfo;
	Iteminfo.MinLevel = 1;
	Iteminfo.MaxLevel = 10;
	Player->GetInventoryComponent_Implementation()->AddItem(FGameplayTag::RequestGameplayTag("Item.Equipment.Module.Player.MidAirManeuvering"), Iteminfo, 1);
}

void ADropItemBase_Rune::RuneTierFour(AFDPlayerCharacter* Player)
{
	FItemAcquisitionInfo Iteminfo;
	Iteminfo.MinLevel = 1;
	Iteminfo.MaxLevel = 10;
	Player->GetInventoryComponent_Implementation()->AddItem(FGameplayTag::RequestGameplayTag("Item.Equipment.Module.Player.HighVoltage"), Iteminfo, 1);
}
