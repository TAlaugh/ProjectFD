// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/WidgetController/InventoryWidgetController.h"

#include "Controller/FDPlayerController.h"
#include "Gameframework/FDPlayerState.h"
#include "GAS/Monster/EnemyAttributeSet.h"
#include "GAS/Player/PlayerAttributeSet.h"
#include "Inventory/InventoryComponent.h"
#include "Interfaces/InventoryInterface.h"


AActor* UInventoryWidgetController::GetOwningActor()
{
	if (OwningActor)
	{
		return OwningActor;
	}

	return nullptr;
}

void UInventoryWidgetController::SetOwningActor(AActor* InOwner)
{
	OwningActor = InOwner;
}

void UInventoryWidgetController::BindCallbacksToDependencies()
{
	OwningInventory = IInventoryInterface::Execute_GetInventoryComponent(OwningActor);

	if (IsValid(OwningInventory))
	{
		OwningInventory->InventoryList.DirtyItemDelegate.AddLambda(
			[this] (const FFDInventoryEntry& DirtyItem)
			{
				if (DirtyItem.ItemTag.MatchesTag(FGameplayTag::RequestGameplayTag("Item.Equipment.Weapon")))
				{
					InventoryEntryDelegate.Broadcast(DirtyItem, OwningInventory->GetFireMontageLength(DirtyItem.ItemID));
				}
				else if (DirtyItem.ItemTag.MatchesTag(FGameplayTag::RequestGameplayTag("Item.Equipment.Module.Player")))
				{
					PlayerModuleInventoryEntryDelegate.Broadcast(DirtyItem);
				}
			});
	}

	AFDPlayerController* OwningController = Cast<AFDPlayerController>(OwningActor);

	if (OwningController)
	{
		if (AFDPlayerState* PlayerState = OwningController->GetPlayerState<AFDPlayerState>())
		{
			if (UAbilitySystemComponent* ASC = PlayerState->GetAbilitySystemComponent())
			{
				if (const UPlayerAttributeSet* ConstAttrSet = Cast<UPlayerAttributeSet>(ASC->GetAttributeSet(UPlayerAttributeSet::StaticClass())))
				{
					ASC->GetGameplayAttributeValueChangeDelegate(ConstAttrSet->GetModuleCapacityAttribute()).AddLambda(
						[this] (const FOnAttributeChangeData& Data)
						{
							ModuleCapacityChangeDelegate.Broadcast(Data.NewValue);
						});
					ASC->GetGameplayAttributeValueChangeDelegate(ConstAttrSet->GetMaxModuleCapacityAttribute()).AddLambda(
						[this] (const FOnAttributeChangeData& Data)
						{
							MaxModuleCapacityChangeDelegate.Broadcast(Data.NewValue);
						});
				}
			}
		}
	}
}

// void UInventoryWidgetController::BroadcastInitialValues()
// {
// 	if (IsValid(OwningInventory))
// 	{
// 		TArray<FFDInventoryEntry> Entries = OwningInventory->GetInventoryEntries();
// 		for (const FFDInventoryEntry& Entry : Entries)
// 		{
// 			InventoryEntryDelegate.Broadcast(Entry, OwningInventory->GetFireMontageLength(Entry.ItemID));
// 		}
// 	}
// }

void UInventoryWidgetController::BroadcastWeaponInventoryInitialValues()
{
	if (IsValid(OwningInventory))
	{
		TArray<FFDInventoryEntry> Entries = OwningInventory->GetInventoryEntries();
		for (const FFDInventoryEntry& Entry : Entries)
		{
			if (Entry.ItemTag.MatchesTag(FGameplayTag::RequestGameplayTag("Item.Equipment.Weapon")))
			{
				InventoryEntryDelegate.Broadcast(Entry, OwningInventory->GetFireMontageLength(Entry.ItemID));
			}
		}
	}
}

void UInventoryWidgetController::BroadcastPlayerModuleInventoryInitialValues()
{
	if (IsValid(OwningInventory))
	{
		// Reference?
		TArray<FFDInventoryEntry> Entries = OwningInventory->GetInventoryEntries();
		for (const FFDInventoryEntry& Entry : Entries)
		{
			if (Entry.ItemTag.MatchesTag(FGameplayTag::RequestGameplayTag("Item.Equipment.Module.Player")))
			{
				PlayerModuleInventoryEntryDelegate.Broadcast(Entry);
			}
		}
	}
}

