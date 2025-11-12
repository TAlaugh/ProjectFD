// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Inventory/Slot/WeaponInventoryWidget.h"

#include "Components/WrapBox.h"
#include "Widget/Inventory/Slot/WeaponSlotSmallWidget.h"


void UWeaponInventoryWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void UWeaponInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UWeaponInventoryWidget::CreateSmallWeaponSlot(const FFDInventoryEntry& Entry, UInventorySlotTextures* TextureMap, FMasterItemDefinition ItemDefinition, float FireTime)
{
	if (UWeaponSlotSmallWidget* NewWeaponSlot = CreateWidget<UWeaponSlotSmallWidget>(this, WeaponSlotSmallClass))
	{
		WeaponWrapBox->AddChildToWrapBox(NewWeaponSlot);
		NewWeaponSlot->SetSlotInfo(Entry, TextureMap, ItemDefinition, FireTime);
		NewWeaponSlot->WeaponSmallSlotClickedDelegate.AddDynamic(this, &UWeaponInventoryWidget::BroadcastWeaponEquip);
	}
}

void UWeaponInventoryWidget::DestroySmallWeaponSlot(int64 ItemID)
{
	for (UWidget* ItWidget : WeaponWrapBox->GetAllChildren())
	{
		if (Cast<UWeaponSlotSmallWidget>(ItWidget)->GetInventoryEntry().ItemID == ItemID)
		{
			ItWidget->RemoveFromParent();
			break;
		}
	}
}

void UWeaponInventoryWidget::ClearSmallWeaponSlots()
{
	WeaponWrapBox->ClearChildren();
}

void UWeaponInventoryWidget::BroadcastWeaponEquip(const FFDInventoryEntry& Entry)
{
	for (int i = 0; i < WeaponWrapBox->GetChildrenCount(); i++)
	{
		if (Entry.ItemID == Cast<UWeaponSlotSmallWidget>(WeaponWrapBox->GetChildAt(i))->GetInventoryEntry().ItemID)
		{
			WeaponWrapBox->GetChildAt(i)->RemoveFromParent();
			break;
		}
	}
	WeaponInventoryDelegate.Broadcast(Entry);
}
