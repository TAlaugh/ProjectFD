
#include "Inventory/InventoryComponent.h"

#include "AbilitySystemComponent.h"
#include "DataAssets/Item/ItemTypesToTables.h"
#include "GAS/FDGameplayStatic.h"
#include "Net/UnrealNetwork.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "NativeGameplayTags.h"
// #include "SAssetView.h"
#include "DataAssets/Item/EquipmentStatEffects.h"
#include "Equipment/EquipmentActor.h"
#include "Equipment/EquipmentDefinition.h"
#include "Equipment/ModuleDefinition.h"
#include "spdlog/fmt/bundled/compile.h"

namespace FDGameplayTags::Static
{
	UE_DEFINE_GAMEPLAY_TAG_STATIC(Category_Equipment, "Item.Equipment");
	UE_DEFINE_GAMEPLAY_TAG_STATIC(Category_Equipment_Weapon, "Item.Equipment.Weapon");
	UE_DEFINE_GAMEPLAY_TAG_STATIC(Category_Equipment_Module, "Item.Equipment.Module");
}


void FFDInventoryList::SetInventoryEntries(TArray<FFDInventoryEntry> InEntries)
{
	Entries = InEntries;
	ClearGarbage();
	
	MarkArrayDirty();
}

void FFDInventoryList::AddItem(const FGameplayTag& ItemTag, const FItemAcquisitionInfo& AcquisitionInfo, int32 NumItems)
{
	ClearGarbage();
	
	if (ItemTag.MatchesTag(FDGameplayTags::Static::Category_Equipment_Weapon) ||
		ItemTag.MatchesTag(FDGameplayTags::Static::Category_Equipment_Module))
	{
		// Do nothing
	}
	else
	{
		for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
		{
			FFDInventoryEntry& Entry = *EntryIt;

			// if enchant level is same
			if (Entry.ItemTag.MatchesTagExact(ItemTag) && Entry.Level == 0)
			{
				Entry.Quantity += NumItems;

				// make it replicated
				MarkItemDirty(Entry);

				// Needed?
				// if (OwnerComponent->GetOwner()->HasAuthority())
				// {
				// 	DirtyItemDelegate.Broadcast(Entry);
				// }
				return;
			}
		}
	}

	const FMasterItemDefinition Item = OwnerComponent->GetItemDefinitionByTag(ItemTag);
	
	FFDInventoryEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.ItemTag = ItemTag;
	NewEntry.ItemName = Item.ItemName;
	NewEntry.Quantity = NumItems;
	NewEntry.ItemID = GenerateID();

	if (NewEntry.ItemTag.MatchesTag(FDGameplayTags::Static::Category_Equipment_Weapon) && IsValid(WeakStats.Get()))
	{
		// Raise Effect according to the EquipmentLevel
		RollAcquisitionInfo(&NewEntry, AcquisitionInfo);
		RollForStats(Item.EquipmentProps.EquipmentClass, &NewEntry);
	}
	else if (NewEntry.ItemTag.MatchesTag(FDGameplayTags::Static::Category_Equipment_Module) && IsValid(WeakStats.Get()))
	{
		// RollAcquisitionInfo(&NewEntry, AcquisitionInfo);
		RollForModuleStats(Item.ModuleProps.ModuleClass, &NewEntry);

		for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
		{
			FFDInventoryEntry& Entry = *EntryIt;

			// if enchant level is same
			if (Entry.ItemTag.MatchesTagExact(ItemTag) && Entry.Level == NewEntry.Level && &Entry != &NewEntry)
			{
				Entry.Quantity += NumItems;

				// make it replicated
				MarkItemDirty(Entry);

				// Needed?
				// if (OwnerComponent->GetOwner()->HasAuthority())
				// {
				// 	DirtyItemDelegate.Broadcast(Entry);
				// }

				for (auto EntryItIt = Entries.CreateIterator(); EntryItIt; ++EntryItIt)
				{
					FFDInventoryEntry& CacheEntry = *EntryItIt;

					if (&CacheEntry == &NewEntry)
					{
						EntryItIt.RemoveCurrent();
					}
				}
				
				return;
			}
		}
	}
	
	MarkItemDirty(NewEntry);

	// Needed?
	// if (OwnerComponent->GetOwner()->HasAuthority())
	// {
	// 	DirtyItemDelegate.Broadcast(NewEntry);
	// }
	
	// Entries.Sort([](const FFDInventoryEntry& A, const FFDInventoryEntry& B)
	// {
	// 	return A.ItemTag.ToString() < B.ItemTag.ToString();
	// });
}

void FFDInventoryList::RemoveItem(const FFDInventoryEntry& InventoryEntry, int32 NumItems)
{
	ClearGarbage();
	
	for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
	{
		FFDInventoryEntry& Entry = *EntryIt;

		if (Entry.ItemID == InventoryEntry.ItemID)
		{
			Entry.Quantity -= NumItems;

			MarkItemDirty(Entry);

			// if (Entry.Quantity <= 0)
			// {
			// 	Entry.ItemID = 0;
			// 	MarkItemDirty(Entry);
			// 	// need to be removed
			// 	// EntryIt.RemoveCurrent();
			// 	// MarkArrayDirty();
			// }
			// else
			// {
			// 	MarkItemDirty(Entry);
			// }


			// if (OwnerComponent->GetOwner()->HasAuthority())
			// {
			// 	DirtyItemDelegate.Broadcast(Entry);
			// }
			// else
			// {
			// 	InventoryItemRemovedDelegate.Broadcast(Entry.ItemID);
			// 	// 서버랑 달라!!!!!
			// 	// 그렇다고 서버에서 없애면 delegate 보낸 애가 사라짐?
			// 	EntryIt.RemoveCurrent();
			// 	MarkArrayDirty();
			// }
			
			break;
		}
	}
}

bool FFDInventoryList::HasEnough(int64 ItemID, int32 NumItems)
{
	for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
	{
		FFDInventoryEntry& Entry = *EntryIt;

		// if (Entry.ItemTag.MatchesTagExact(ItemTag))
		// {
		// 	if (Entry.Quantity >= NumItems)
		// 	{
		// 		return true;
		// 	}
		// }

		if (Entry.ItemID == ItemID)
		{
			if (Entry.Quantity >= NumItems)
			{
				return true;
			}
		}
	}

	return false;
}

uint64 FFDInventoryList::GenerateID()
{
	uint64 NewID = ++LastAssignedID;

	int32 SignatureIndex = 0;

	while (SignatureIndex < 12)
	{
		if (FMath::RandRange(0, 100) < 85)
		{
			NewID |= (uint64)1 << FMath::RandRange(0, 63);
		}
		++SignatureIndex;
	}

	return NewID;
}

void FFDInventoryList::SetStats(UEquipmentStatEffects* InStats)
{
	WeakStats = InStats;
}

void FFDInventoryList::RollForStats(const TSubclassOf<UEquipmentDefinition> EquipmentDefinition, FFDInventoryEntry* Entry)
{
	UEquipmentStatEffects* StatEffects = WeakStats.Get();
	const UEquipmentDefinition* EquipmentCDO = GetDefault<UEquipmentDefinition>(EquipmentDefinition);
	
#pragma region Abilities
	
	const FGameplayTag& ItemTag = EquipmentCDO->ItemTag;

	if (TObjectPtr<UDataTable>* FoundTable = StatEffects->WeaponAbilityMap.Find(ItemTag))
	{
		if (UDataTable* Table = FoundTable->Get())
		{
			TArray<FEquipmentAbilityGroup*> Rows;
			Table->GetAllRows<FEquipmentAbilityGroup>(TEXT("LoadRows"), Rows);

			for (FEquipmentAbilityGroup* Row : Rows)
			{
				Entry->EffectPackage.Abilities.Add(*Row);
			}
		}
	}
	
#pragma endregion

	// Damage, FireRate, Ammo
	UDataTable* BasicStats = StatEffects->WeaponStatMap.Find(EquipmentCDO->BasicStatRoll)->Get();		
	for (const FName& StatName : BasicStats->GetRowNames())
	{
		FGameplayTag StatTag = FGameplayTag::RequestGameplayTag(StatName);
		
		if (const FEquipmentStatEffectGroup* PossibleStat = UFDGameplayStatic::GetDataTableRowByTag<FEquipmentStatEffectGroup>(BasicStats, StatTag))
		{
			FEquipmentStatEffectGroup NewStat = *PossibleStat;

			NewStat.CurrentValue = PossibleStat->bFractionalStat ? FMath::FRandRange(PossibleStat->MinStatLevel, PossibleStat->MaxStatLevel) * (Entry->Level / 20 + 1) :
				FMath::TruncToInt(FMath::FRandRange(PossibleStat->MinStatLevel, PossibleStat->MaxStatLevel) * (Entry->Level / 20 + 1));

			Entry->EffectPackage.StatEffects.Add(NewStat);
		}
	}

	// Random Options
	UDataTable* PossibleStats = StatEffects->WeaponStatMap.Find(EquipmentCDO->PossibleStatRoll)->Get();
	
	const int32 NumStatsToRoll = FMath::RandRange(EquipmentCDO->MinPossibleStats < PossibleStats->GetRowMap().Num() ? EquipmentCDO->MinPossibleStats : PossibleStats->GetRowMap().Num(),
		EquipmentCDO->MaxPossibleStats < PossibleStats->GetRowMap().Num() ? EquipmentCDO->MaxPossibleStats : PossibleStats->GetRowMap().Num());
	int32 StatRollIndex = 0;
	
	TSet<int32> UniquePossibleIndex = TSet<int32>();
	while (UniquePossibleIndex.Num()<=NumStatsToRoll)
	{
		const int32 PossibleStatIndex = FMath::RandRange(0, PossibleStats->GetRowMap().Num() - 1);
		
		if (UniquePossibleIndex.Find(PossibleStatIndex))
		{
			// Do nothing
		}
		else
		{
			UniquePossibleIndex.Add(PossibleStatIndex);
		}
	}
	for (const FName& StatName : PossibleStats->GetRowNames())
	{
		if (UniquePossibleIndex.Find(StatRollIndex))
		{
			FGameplayTag StatTag = FGameplayTag::RequestGameplayTag(StatName);	
		
			if (const FEquipmentStatEffectGroup* PossibleStat = UFDGameplayStatic::GetDataTableRowByTag<FEquipmentStatEffectGroup>(PossibleStats, StatTag))
			{
				if (FMath::FRandRange(0.f, 1.f) < PossibleStat->ProbabilityToSelect)
				{
					FEquipmentStatEffectGroup NewStat = *PossibleStat;

					NewStat.CurrentValue = PossibleStat->bFractionalStat ? FMath::FRandRange(PossibleStat->MinStatLevel, PossibleStat->MaxStatLevel) * (Entry->Level / 20 + 1) :
						FMath::TruncToInt(FMath::FRandRange(PossibleStat->MinStatLevel, PossibleStat->MaxStatLevel) * (Entry->Level / 20 + 1));

					Entry->EffectPackage.StatEffects.Add(NewStat);
				}
			}
		}
		StatRollIndex++;
	}
}

void FFDInventoryList::RollForModuleStats(const TSubclassOf<UModuleDefinition> ModuleDefinition,
	FFDInventoryEntry* Entry)
{
	UEquipmentStatEffects* StatEffects = WeakStats.Get();
	const UModuleDefinition* ModuleCDO = GetDefault<UModuleDefinition>(ModuleDefinition);

	// GetAbilities
	const FGameplayTag& ItemTag = ModuleCDO->ItemTag;

	if (TObjectPtr<UDataTable>* FoundTable = StatEffects->ModuleAbilityMap.Find(ItemTag))
	{
		if (UDataTable* Table = FoundTable->Get())
		{
			TArray<FEquipmentAbilityGroup*> Rows;
			Table->GetAllRows<FEquipmentAbilityGroup>(TEXT("LoadRows"), Rows);

			for (FEquipmentAbilityGroup* Row : Rows)
			{
				Entry->EffectPackage.Abilities.Add(*Row);
			}
		}
	}

	// GetEffects
	bool bRandLevel = false;
	if (TObjectPtr<UDataTable>* FoundTable = StatEffects->ModuleStatMap.Find(ModuleCDO->ItemTag))
	{
		if (UDataTable* BasicStats = FoundTable->Get())
		{
			for (const FName& StatName : BasicStats->GetRowNames())
			{
				FGameplayTag StatTag = FGameplayTag::RequestGameplayTag(StatName);
		
				if (const FEquipmentStatEffectGroup* PossibleStat = UFDGameplayStatic::GetDataTableRowByTag<FEquipmentStatEffectGroup>(BasicStats, StatTag))
				{
					FEquipmentStatEffectGroup NewStat = *PossibleStat;

					if (not bRandLevel)
					{
						int32 Rand = FMath::RandRange(1, 10);
						if (Rand % 3  == 0)
						{
							NewStat.CurrentValue = FMath::FloorToInt32(FMath::RandRange(PossibleStat->MinStatLevel + 1, PossibleStat->MaxStatLevel));
						}
						else
						{
							NewStat.CurrentValue = PossibleStat->MinStatLevel;
						}
						// UE_LOG(LogTemp, Warning, TEXT("CurrentValue : %f"), NewStat.CurrentValue);
						Entry->Level = NewStat.CurrentValue;
						bRandLevel = true;
					}
					else
					{
						NewStat.CurrentValue = Entry->Level;
					}

					Entry->EffectPackage.StatEffects.Add(NewStat);
				}
			}
		}
	}
	else
	{
		int32 Rand = FMath::RandRange(1, 10);
		if (Rand % 3  == 0)
		{
			Entry->Level = FMath::RandRange(1, ModuleCDO->EnchantMaxCount);
		}
		else
		{
			Entry->Level = 0;
		}
	}
}

void FFDInventoryList::RollAcquisitionInfo(FFDInventoryEntry* Entry, const FItemAcquisitionInfo& AcquisitionInfo)
{
	Entry->Level = FMath::RandRange(AcquisitionInfo.MinLevel, AcquisitionInfo.MaxLevel);
}

void FFDInventoryList::AddUnEquippedItem(const FFDEquipmentEntry& UnEquippedEntry,
                                         const FEquipmentEffectPackage& EffectPackage)
{
	for (const FFDInventoryEntry& EntryIt : Entries)
	{
		if (EntryIt.ItemID == UnEquippedEntry.ItemID && EntryIt.Quantity > 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("ItemID : %lld"), EntryIt.ItemID);
			return;
		}
	}
	FFDInventoryEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.ItemTag = UnEquippedEntry.EntryTag;
	NewEntry.ItemName = UnEquippedEntry.ItemName;
	NewEntry.Quantity = 1;
	NewEntry.ItemID = UnEquippedEntry.ItemID;
	NewEntry.EffectPackage = EffectPackage;
	NewEntry.Level = UnEquippedEntry.EquipmentLevel;

	MarkItemDirty(NewEntry);
	// DirtyItemDelegate.Broadcast(NewEntry);
}

void FFDInventoryList::AddUnEquippedModule(const FFDEquipmentEntry& UnEquippedEntry,
	const FEquipmentEffectPackage& EffectPackage)
{
	for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
	{
		FFDInventoryEntry& Entry = *EntryIt;

		if (Entry.ItemTag.MatchesTag(FDGameplayTags::Static::Category_Equipment_Module))
		{
			// if enchant level is same
			if (Entry.ItemTag.MatchesTagExact(UnEquippedEntry.EntryTag))
			{
				if (Entry.Level == UnEquippedEntry.EquipmentLevel)
				{
					Entry.Quantity += 1;

					// make it replicated
					MarkItemDirty(Entry);

					// if (OwnerComponent->GetOwner()->HasAuthority())
					// {
					// 	DirtyItemDelegate.Broadcast(Entry);
					// }
					return;
				}
			}
		}
	}
	FFDInventoryEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.ItemTag = UnEquippedEntry.EntryTag;
	NewEntry.ItemName = UnEquippedEntry.ItemName;
	NewEntry.Quantity = 1;
	// should generate ItemID for every single module?
	NewEntry.ItemID = UnEquippedEntry.ItemID;
	NewEntry.EffectPackage = UnEquippedEntry.EffectPackage;
	NewEntry.Level = UnEquippedEntry.EquipmentLevel;

	Entries.Sort([](const FFDInventoryEntry& A, const FFDInventoryEntry& B)
	{
		return A.ItemTag.ToString() < B.ItemTag.ToString();
	});

	MarkItemDirty(NewEntry);
}

void FFDInventoryList::ClearGarbage()
{
	for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
	{
		FFDInventoryEntry& Entry = *EntryIt;

		if (Entry.ItemTag.MatchesTag(FDGameplayTags::Static::Category_Equipment_Weapon))
		{
			if (Entry.Quantity <= 0)
			{
				EntryIt.RemoveCurrent();
				MarkArrayDirty();
			}
		}
	}
}

void FFDInventoryList::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	// not good. what uhr said
}

void FFDInventoryList::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	for (const int32 Index : AddedIndices)
	{
		FFDInventoryEntry& Entry = Entries[Index];

		DirtyItemDelegate.Broadcast(Entry);
	}
}

void FFDInventoryList::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	for (const int32 Index : ChangedIndices)
	{
		FFDInventoryEntry& Entry = Entries[Index];

		DirtyItemDelegate.Broadcast(Entry);
	}
}

UInventoryComponent::UInventoryComponent() : InventoryList(this)
{	
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;

}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInventoryComponent, InventoryList);
}

void UInventoryComponent::AddItem(const FGameplayTag ItemTag, const FItemAcquisitionInfo& AcquisitionInfo, int32 NumItems)
{
	AActor* Owner = GetOwner();
	if (!IsValid(Owner)) return;

	if (!Owner->HasAuthority())
	{
		ServerAddItem(ItemTag, AcquisitionInfo, NumItems);
		return;
	}

	InventoryList.AddItem(ItemTag, AcquisitionInfo, NumItems);
}

void UInventoryComponent::UseItem(const FFDInventoryEntry& Entry, int32 NumItems, int SlotNum)
{
	AActor* Owner = GetOwner();
	if (!IsValid(Owner)) return;

	if (!Owner->HasAuthority())
	{
		ServerUseItem(Entry, NumItems, SlotNum);
		return;
	}

	if (InventoryList.HasEnough(Entry.ItemID, NumItems))
	{
		const FMasterItemDefinition Item = GetItemDefinitionByTag(Entry.ItemTag);

		if (UAbilitySystemComponent* OwnerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Owner))
		{
			if (IsValid(Item.ConsumableProps.ItemEffectClass))
			{
				const FGameplayEffectContextHandle ContextHandle = OwnerASC->MakeEffectContext();
				FGameplayEffectSpecHandle SpecHandle = OwnerASC->MakeOutgoingSpec(Item.ConsumableProps.ItemEffectClass,
					Item.ConsumableProps.ItemEffectLevel, ContextHandle);
				OwnerASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

				InventoryList.RemoveItem(Entry);

				return;
			}

			// if (IsValid(Item.EquipmentProps.EquipmentClass))
			// {
			// 	EquipmentItemDelegate.Broadcast(Item.EquipmentProps.EquipmentClass, Entry.EffectPackage);
			// 	InventoryList.RemoveItem(Entry);
			// }
			if (IsValid(Item.EquipmentProps.EquipmentClass))
			{
				FExtraEquipmentInfo ExtraEquipmentInfo = FExtraEquipmentInfo();
				ExtraEquipmentInfo.ItemID = Entry.ItemID;
				ExtraEquipmentInfo.ItemName = Entry.ItemName;
				ExtraEquipmentInfo.EquipmentLevel = Entry.Level;
				EquipmentItemDelegate.Broadcast(Item.EquipmentProps.EquipmentClass, Entry.EffectPackage, SlotNum, ExtraEquipmentInfo);
				InventoryList.RemoveItem(Entry);

				return;
			}

			if (IsValid(Item.ModuleProps.ModuleClass))
			{
				FExtraEquipmentInfo ExtraEquipmentInfo = FExtraEquipmentInfo();
				ExtraEquipmentInfo.ItemID = Entry.ItemID;
				ExtraEquipmentInfo.ItemName = Entry.ItemName;
				ExtraEquipmentInfo.EquipmentLevel = Entry.Level;
				ModuleItemDelegate.Broadcast(Item.ModuleProps.ModuleClass, Entry.EffectPackage, SlotNum, ExtraEquipmentInfo);
				// if equip possible
				InventoryList.RemoveItem(Entry);

				return;
			}
		}
	}
}


FMasterItemDefinition UInventoryComponent::GetItemDefinitionByTag(const FGameplayTag& ItemTag) const
{
	checkf(InventoryDefinitions, TEXT("No Inventory Definitions Inside Component %s"), *GetNameSafe(this));

	for (const auto& Pair : InventoryDefinitions->TagsToTables)
	{
		if (ItemTag.MatchesTag(Pair.Key))
		{
			return *UFDGameplayStatic::GetDataTableRowByTag<FMasterItemDefinition>(Pair.Value, ItemTag);
		}
	}

	return FMasterItemDefinition();
}

TArray<FFDInventoryEntry> UInventoryComponent::GetInventoryEntries()
{
	return InventoryList.Entries;
}

void UInventoryComponent::AddUnEquippedItemEntry(const FFDEquipmentEntry& UnEquippedEntry,
	const FEquipmentEffectPackage& EffectPackage)
{
	InventoryList.AddUnEquippedItem(UnEquippedEntry, EffectPackage);
}

void UInventoryComponent::AddUnEquippedModuleEntry(const FFDEquipmentEntry& UnEquippedEntry,
	const FEquipmentEffectPackage& EffectPackage)
{
	InventoryList.AddUnEquippedModule(UnEquippedEntry, EffectPackage);
}

float UInventoryComponent::GetFireMontageLength(int64 ItemID)
{
	FFDInventoryEntry TargetEntry = FFDInventoryEntry();
	for (FFDInventoryEntry& EntryIt : GetInventoryEntries())
	{
		if (EntryIt.ItemID == ItemID)
		{
			TargetEntry = EntryIt;
			break;
		}
	}

	if (not TargetEntry.IsValid())
	{
		return 1.f;
	}
	
	const FMasterItemDefinition Item = GetItemDefinitionByTag(TargetEntry.ItemTag);
	const UEquipmentDefinition* EquipmentCDO = GetDefault<UEquipmentDefinition>(Item.EquipmentProps.EquipmentClass);
	
	if (UClass* EquipClass = EquipmentCDO->ActorsToSpawn[0].EquipmentClass.LoadSynchronous())
	{
		// CDO (Class Default Object) 가져오기
		AEquipmentActor* DefaultObj = Cast<AEquipmentActor>(EquipClass->GetDefaultObject());
		if (DefaultObj)
		{
			return DefaultObj->GetFireMontageLength();
		}
	}

	return 1.f;
}

void UInventoryComponent::ServerAddItem_Implementation(const FGameplayTag& ItemTag, const FItemAcquisitionInfo& AcquisitionInfo, int32 NumItems)
{
	AddItem(ItemTag, AcquisitionInfo, NumItems);
}


void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwner()->HasAuthority())
	{
		InventoryList.SetStats(StatEffects);
	}
}

void UInventoryComponent::ServerUseItem_Implementation(const FFDInventoryEntry& Entry, int32 NumItems, int SlotNum)
{
	UseItem(Entry, NumItems, SlotNum);
}

