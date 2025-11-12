// Fill out your copyright notice in the Description page of Project Settings.


#include "Equipment/EquipmentManagerComponent.h"

#include "Equipment/EquipmentDefinition.h"
#include "Equipment/EquipmentInstance.h"
#include "GAS/Player/PlayerAbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Character/Player/FDPlayerCharacter.h"
#include "Controller/FDPlayerController.h"
#include "Equipment/ModuleDefinition.h"
#include "GameFramework/Character.h"
#include "GAS/FDGameplayStatic.h"
#include "GAS/Player/PlayerAttributeSet.h"
#include "Net/UnrealNetwork.h"


void FFDEquipmentEntry::OnEquip(AActor* Owner, bool bIsActive) const
{
	Instance->OnEquip(Owner, bIsActive);
}

float FFDEquipmentEntry::GetFireTime()
{
	if (FireTime <= 0)
	{
		SetFireTime();
	}

	return FireTime;
}

void FFDEquipmentEntry::SetFireTime()
{
	if (Instance)
	{
		FireTime = Instance->GetFireTime();
	}
}

TSubclassOf<UAnimInstance> FFDEquipmentEntry::GetCharacterAnimLayer() const
{
	return CharacterAnimLayer;
}

TArray<FFDEquipmentEntry> FFDEquipmentList::GetWeaponEntries()
{
	if (not EquipmentEntries.IsEmpty())
	{
		return EquipmentEntries;
	}

	return TArray<FFDEquipmentEntry>();
}

void FFDEquipmentList::SetWeaponEntries(TArray<FFDEquipmentEntry> InEntries)
{
	EquipmentEntries = InEntries;
	MarkArrayDirty();
	for (FFDEquipmentEntry& EntryIt : EquipmentEntries)
	{
		if (EntryIt.EntryTag.MatchesTag(FGameplayTag::RequestGameplayTag("Item.Equipment.Weapon")))
		{
			EntryIt.Instance = NewObject<UEquipmentInstance>(OwnerComponent->GetOwner(), UEquipmentInstance::StaticClass());
			EntryIt.Instance->bIsActiveWeapon = EntryIt.bIsActiveWeapon;

			if (EntryIt.bIsActiveWeapon)
			{
				if (EntryIt.HasStats())
				{
					AddEquipmentStats(&EntryIt);
				}

				if (EntryIt.HasAbilities())
				{
					AddEquipmentAbility(&EntryIt);
				}

				if (EntryIt.CharacterAnimLayer)
				{
					if (AFDPlayerCharacter* PlayerCharacter = Cast<AFDPlayerCharacter>(OwnerComponent->GetOwner()))
					{
						PlayerCharacter->GetMesh()->GetAnimInstance()->LinkAnimClassLayers(EntryIt.CharacterAnimLayer);
						// 상황에 따라 분기 필요
						UFDGameplayStatic::GrantGameplayTagByEffect(PlayerCharacter->GetAbilitySystemComponent(), UFDGameplayStatic::GetOwnerCombatTag());
					}
				}
			}

			SpawnSetEquipmentActors(EntryIt, EntryIt.bIsActiveWeapon, true);
		}
		else if (EntryIt.EntryTag.MatchesTag(FGameplayTag::RequestGameplayTag("Item.Equipment.Module")))
		{
			RestoreModule(EntryIt);
		}
		MarkItemDirty(EntryIt);
	}
}

UPlayerAbilitySystemComponent* FFDEquipmentList::GetAbilitySystemComponent()
{
	check(OwnerComponent);
	check(OwnerComponent->GetOwner());

	return Cast<UPlayerAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwnerComponent->GetOwner()));
}

UEquipmentInstance* FFDEquipmentList::AddEntry(const TSubclassOf<UEquipmentDefinition>& EquipmentDefinition,
	const FEquipmentEffectPackage& EffectPackage, int SlotNum, FExtraEquipmentInfo ItemInfo)
{
	check(EquipmentDefinition);
	check(OwnerComponent);
	check(OwnerComponent->GetOwner()->HasAuthority());

	const UEquipmentDefinition* EquipmentCDO = GetDefault<UEquipmentDefinition>(EquipmentDefinition);
	TSubclassOf<UEquipmentInstance> InstanceType = EquipmentCDO->InstanceType;

	if (!IsValid(InstanceType))
	{
		InstanceType = UEquipmentInstance::StaticClass();
	}

	bool bIsActiveSlot = false;
	bool bIsSameUnarmedLocation = false;

	for (auto EntryIt = EquipmentEntries.CreateIterator(); EntryIt; ++EntryIt)
	{		
		FFDEquipmentEntry& Entry = *EntryIt;

		if (!Entry.EntryTag.IsValid() || !Entry.EntryTag.MatchesTag(FGameplayTag::RequestGameplayTag("Item.Equipment.Weapon")))
		{
			continue;
		}
	
		if (Entry.SlotNum == SlotNum)
		{
			// Done in RemoveEntry
			// Entry.bIsActiveWeapon = false;
			// Entry.Instance->bIsActiveWeapon = false;
			RemoveEntry(Entry.Instance);
		}
		else if (Entry.Instance->bIsActiveWeapon == false && Entry.SlotTag.MatchesTagExact(EquipmentCDO->SlotTag))
		{
			bIsSameUnarmedLocation = true;
		}
	}
	
	FFDEquipmentEntry& NewEntry = EquipmentEntries.AddDefaulted_GetRef();
	UE_LOG(LogTemp, Warning, TEXT("SlotNum : %d, ActiveSlotNum = %d"), SlotNum, Cast<UEquipmentManagerComponent>(OwnerComponent)->GetActiveWeaponSlot());
	if (SlotNum == Cast<UEquipmentManagerComponent>(OwnerComponent)->GetActiveWeaponSlot())
	{
		bIsActiveSlot = true;
	}

	NewEntry.SlotNum = SlotNum;
	NewEntry.EntryTag = EquipmentCDO->ItemTag;
	NewEntry.ItemID = ItemInfo.ItemID;
	NewEntry.ItemName = ItemInfo.ItemName;
	NewEntry.EquipmentLevel = ItemInfo.EquipmentLevel;
	NewEntry.AmmoType = EquipmentCDO->AmmoType;
	NewEntry.SlotTag = EquipmentCDO->SlotTag;
	NewEntry.RarityTag = EquipmentCDO->RarityTag;
	NewEntry.RemoveAbilityTags = EquipmentCDO->PossibleAbilityRolls;
	NewEntry.ActorsToSpawn = EquipmentCDO->ActorsToSpawn;
	NewEntry.CharacterAnimLayer = EquipmentCDO->CharacterAnimLayer;
	NewEntry.EffectPackage = EffectPackage;
	NewEntry.Instance = NewObject<UEquipmentInstance>(OwnerComponent->GetOwner(), InstanceType);

	if (bIsActiveSlot)
	{
		if (NewEntry.HasStats())
		{
			AddEquipmentStats(&NewEntry);
		}

		if (NewEntry.HasAbilities())
		{
			AddEquipmentAbility(&NewEntry);
		}

		if (NewEntry.CharacterAnimLayer)
		{
			if (AFDPlayerCharacter* PlayerCharacter = Cast<AFDPlayerCharacter>(OwnerComponent->GetOwner()))
			{
				PlayerCharacter->GetMesh()->GetAnimInstance()->LinkAnimClassLayers(NewEntry.CharacterAnimLayer);
				// 상황에 따라 분기 필요
				UFDGameplayStatic::GrantGameplayTagByEffect(PlayerCharacter->GetAbilitySystemComponent(), UFDGameplayStatic::GetOwnerCombatTag());
			}
		}
	}

	SpawnSetEquipmentActors(NewEntry, bIsActiveSlot, bIsSameUnarmedLocation);

	MarkItemDirty(NewEntry);
	// EquipmentEntryDelegate.Broadcast(NewEntry);

	return NewEntry.Instance;
}

void FFDEquipmentList::AddEquipmentStats(FFDEquipmentEntry* Entry)
{
	if (UPlayerAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		ASC->AddEquipmentEffects(Entry);
	}
}

void FFDEquipmentList::RemoveEquipmentStats(FFDEquipmentEntry* Entry)
{
	if (UPlayerAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		ASC->RemoveEquipmentEffects(Entry);
	}
}

void FFDEquipmentList::AddEquipmentAbility(FFDEquipmentEntry* Entry)
{
	if (UPlayerAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		ASC->AddEquipmentAbilities(Entry);
	}
}

void FFDEquipmentList::RemoveEquipmentAbility(FFDEquipmentEntry* Entry)
{
	if (UPlayerAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		ASC->RemoveEquipmentAbilities(Entry);
	}
}

void FFDEquipmentList::ChangeActiveEntry(int ActiveWeaponSlot, int SlotNum)
{
	check(OwnerComponent);

	// First Deactivate Weapon
	for (auto EntryIt = EquipmentEntries.CreateIterator(); EntryIt; ++EntryIt)
	{
		FFDEquipmentEntry& Entry = *EntryIt;
		// const UEquipmentDefinition* EquipmentCDO = GetDefault<UEquipmentDefinition>(Entry.EquipmentDefinition);

		if (Entry.SlotNum == ActiveWeaponSlot)
		{
			if (IsValid(Entry.Instance))
			{
				FGameplayTagContainer TagsToRemove(UFDGameplayStatic::GetOwnerCombatTag());
				GetAbilitySystemComponent()->RemoveActiveEffectsWithGrantedTags(TagsToRemove);
				
				Entry.bIsActiveWeapon = false;
				Entry.Instance->UnarmEquipmentActors(Entry.ActorsToSpawn);
				Entry.LoadedAmmo = GetAbilitySystemComponent()->GetNumericAttribute(UPlayerAttributeSet::GetAmmoAttribute());
				RemoveEquipmentStats(&Entry);
				RemoveEquipmentAbility(&Entry);
				MarkItemDirty(Entry);
				break;
			}
		}
	}

	// bool bIsEquippedSlot = false;

	// And then Activate Weapon
	for (auto EntryIt = EquipmentEntries.CreateIterator(); EntryIt; ++EntryIt)
	{
		FFDEquipmentEntry& Entry = *EntryIt;
		// const UEquipmentDefinition* EquipmentCDO = GetDefault<UEquipmentDefinition>(Entry.EquipmentDefinition);
		if (Entry.SlotNum == SlotNum && Entry.EntryTag.MatchesTag(FGameplayTag::RequestGameplayTag("Item.Equipment.Weapon")))
		{
			// bIsEquippedSlot = true;
			if (Entry.HasStats())
			{
				AddEquipmentStats(&Entry);
			}

			if (Entry.HasAbilities())
			{
				AddEquipmentAbility(&Entry);
			}

			if (Entry.CharacterAnimLayer)
			{
				if (AFDPlayerCharacter* PlayerCharacter = Cast<AFDPlayerCharacter>(OwnerComponent->GetOwner()))
				{
					PlayerCharacter->GetMesh()->GetAnimInstance()->LinkAnimClassLayers(Entry.CharacterAnimLayer);
					// 상황에 따라 분기 필요
					UFDGameplayStatic::GrantGameplayTagByEffect(PlayerCharacter->GetAbilitySystemComponent(), UFDGameplayStatic::GetOwnerCombatTag());
				}
			}

			GetAbilitySystemComponent()->SetNumericAttributeBase(UPlayerAttributeSet::GetAmmoAttribute(), Entry.LoadedAmmo);

			SpawnSetEquipmentActors(Entry, true, false);

			MarkItemDirty(Entry);

			break;
		}
	}
}

// ActiveWeaponSlot never changes
void FFDEquipmentList::ChangeWeaponOrder(int ActiveWeaponSlot, int SlotNum1, int SlotNum2)
{
	// if (ActiveWeaponSlot == SlotNum1)
	// {
	// 	ChangeActiveEntry(ActiveWeaponSlot, SlotNum2);
	// 	// Cast<UEquipmentManagerComponent>(OwnerComponent)->SetActiveWeaponSlot(SlotNum2);
	// }
	// else if (ActiveWeaponSlot == SlotNum2)
	// {
	// 	ChangeActiveEntry(ActiveWeaponSlot, SlotNum1);
	// 	// Cast<UEquipmentManagerComponent>(OwnerComponent)->SetActiveWeaponSlot(SlotNum1);
	// }
	for (FFDEquipmentEntry& Entry : EquipmentEntries)
	{
		if (!Entry.EntryTag.MatchesTag(FGameplayTag::RequestGameplayTag("Item.Equipment.Weapon")))
		{
			continue;
		}
		if (Entry.SlotNum == SlotNum1)
		{
			Entry.SlotNum = SlotNum2;
			MarkItemDirty(Entry);
		}
		else if (Entry.SlotNum == SlotNum2)
		{
			Entry.SlotNum = SlotNum1;
			MarkItemDirty(Entry);
		}
	}
}

void FFDEquipmentList::RemoveEntry(UEquipmentInstance* EquipmentInstance)
{
	check(OwnerComponent);

	for (auto EntryIt = EquipmentEntries.CreateIterator(); EntryIt; ++EntryIt)
	{		
		FFDEquipmentEntry& Entry = *EntryIt;
		
		if (!Entry.EntryTag.MatchesTag(FGameplayTag::RequestGameplayTag("Item.Equipment.Weapon")))
		{
			continue;
		}

		if (Entry.Instance == EquipmentInstance)
		{
			if (Entry.bIsActiveWeapon)
			{
				Entry.bIsActiveWeapon = false;
				RemoveEquipmentStats(&Entry);
				RemoveEquipmentAbility(&Entry);
				FGameplayTagContainer TagsToRemove(UFDGameplayStatic::GetOwnerCombatTag());
				GetAbilitySystemComponent()->RemoveActiveEffectsWithGrantedTags(TagsToRemove);
			}
			Entry.Instance->DestroyEquipmentActors();
			UnEquipmentEntryDelegate.Broadcast(Entry);
			EntryIt.RemoveCurrent();
			MarkArrayDirty();

			break;
		}
	}
}

int64 FFDEquipmentList::AddModuleEntry(const TSubclassOf<UModuleDefinition>& ModuleDefinition,
	const FEquipmentEffectPackage& EffectPackage, int SlotNum, FExtraEquipmentInfo ItemInfo)
{
	check(ModuleDefinition);
	check(OwnerComponent);
	check(OwnerComponent->GetOwner()->HasAuthority());

	const UModuleDefinition* ModuleCDO = GetDefault<UModuleDefinition>(ModuleDefinition);

	// Block equip according to ChangeType
	if (SlotNum == 0)
	{
		if (!ModuleCDO->ChangeTypeTag.MatchesTagExact(FGameplayTag::RequestGameplayTag("Module.ChangeType.Skill")))
		{
			return 0;
		}
	}
	else if (SlotNum == 1)
	{
		if (!ModuleCDO->ChangeTypeTag.MatchesTagExact(FGameplayTag::RequestGameplayTag("Module.ChangeType.Tool")))
		{
			return 0;
		}
	}
	else
	{
		if (ModuleCDO->ChangeTypeTag.MatchesTagExact(FGameplayTag::RequestGameplayTag("Module.ChangeType.Skill")) ||
			ModuleCDO->ChangeTypeTag.MatchesTagExact(FGameplayTag::RequestGameplayTag("Module.ChangeType.Tool")))
		{
			return 0;
		}
	}
	
	// block duplication equip
	for (auto EntryIt = EquipmentEntries.CreateIterator(); EntryIt; ++EntryIt)
	{		
		FFDEquipmentEntry& Entry = *EntryIt;

		if (!Entry.EntryTag.IsValid() || !Entry.EntryTag.MatchesTag(FGameplayTag::RequestGameplayTag("Item.Equipment.Module")))
		{
			continue;
		}
	
		if (Entry.EntryTag.MatchesTagExact(ModuleCDO->ItemTag) && Entry.SlotNum != SlotNum)
		{
			return 0;
		}
	}

	EModuleCapacity CapacityCalc;
	
	if (Cast<UEquipmentManagerComponent>(OwnerComponent)->GetModuleSlotInfos()[SlotNum].Sockets.IsEmpty())
	{
		CapacityCalc = EModuleCapacity::None;
	}
	else
	{
		CapacityCalc = EModuleCapacity::Double;
		// for (FGameplayTag Socket : Cast<UEquipmentManagerComponent>(OwnerComponent)->GetModuleSlotInfos()[SlotNum].Sockets)
		// {
		// 	// if (ModuleCDO->SocketTypeTag.MatchesTag(Socket))
		// 	// if (Socket.GetTagName().IsEqual(ModuleCDO->SocketTypeTag.GetTagName()))
		// 	// if (Socket.MatchesTagExact(ModuleCDO->SocketTypeTag))
		// 	if (ModuleCDO->SocketTypeTag.MatchesTag(FGameplayTag::RequestGameplayTag("Module.SocketType.Almandine")))
		// 	{
		// 		CapacityCalc = EModuleCapacity::Half;
		// 		break;
		// 	}
		// }
		
		// if (ModuleCDO->SocketTypeTag.MatchesTag(Socket))
		// if (Socket.GetTagName().IsEqual(ModuleCDO->SocketTypeTag.GetTagName()))
		// if (Socket.MatchesTagExact(ModuleCDO->SocketTypeTag))
		// should loop the entire container...
		if (ModuleCDO->SocketTypeTag.MatchesTag(Cast<UEquipmentManagerComponent>(OwnerComponent)->GetModuleSlotInfos()[SlotNum].Sockets.First()))
		{
			CapacityCalc = EModuleCapacity::Half;
		}
	}

	float MaxModuleCapacity = GetAbilitySystemComponent()->GetNumericAttribute(UPlayerAttributeSet::GetMaxModuleCapacityAttribute());
	float ModuleCapacity = GetAbilitySystemComponent()->GetNumericAttribute(UPlayerAttributeSet::GetModuleCapacityAttribute());
	
	int32 CalculatedCapacity = ModuleCDO->Capacity;
	if (ModuleCDO->ChangeTypeTag.MatchesTag(FGameplayTag::RequestGameplayTag("Module.ChangeType.Skill")))
	{
		CalculatedCapacity -= ItemInfo.EquipmentLevel;
	}
	else if (ModuleCDO->ChangeTypeTag.MatchesTag(FGameplayTag::RequestGameplayTag("Module.ChangeType.Tool")))
	{
		// Tool Module increases MaxCapacity when enchanted
		GetAbilitySystemComponent()->SetNumericAttributeBase(UPlayerAttributeSet::GetMaxModuleCapacityAttribute(), MaxModuleCapacity + ItemInfo.EquipmentLevel);
	}
	else
	{
		CalculatedCapacity += ItemInfo.EquipmentLevel;
	}
	
	if (CapacityCalc == EModuleCapacity::Half)
	{
		CalculatedCapacity += CalculatedCapacity%2;
		CalculatedCapacity /= 2;
	}
	else if (CapacityCalc == EModuleCapacity::Double)
	{
		CalculatedCapacity *= 2;
	}


	for (auto EntryIt = EquipmentEntries.CreateIterator(); EntryIt; ++EntryIt)
	{
		FFDEquipmentEntry& Entry = *EntryIt;

		if (!Entry.EntryTag.IsValid() || !Entry.EntryTag.MatchesTag(FGameplayTag::RequestGameplayTag("Item.Equipment.Module")))
		{
			continue;
		}
	
		if (Entry.SlotNum == SlotNum)
		{
			int PreCapacity = GetEstimatedModuleCapacity(Entry, Entry.SlotNum);
			if (ModuleCapacity - PreCapacity + CalculatedCapacity > MaxModuleCapacity)
			{
				return 0;
			}
			else
			{
				CalculatedCapacity -= PreCapacity;
			}
			RemoveModuleEntry(Entry.ItemID);
		}
	}
	
	// should consider enchant count
	if (ModuleCapacity + CalculatedCapacity > MaxModuleCapacity)
	{
		return 0;
	}
	else
	{
		ModuleCapacity += CalculatedCapacity;
		GetAbilitySystemComponent()->SetNumericAttributeBase(UPlayerAttributeSet::GetModuleCapacityAttribute(), ModuleCapacity);
	}
	
	FFDEquipmentEntry& NewEntry = EquipmentEntries.AddDefaulted_GetRef();	

	NewEntry.SlotNum = SlotNum;
	NewEntry.EntryTag = ModuleCDO->ItemTag;
	NewEntry.ItemID = ItemInfo.ItemID;
	NewEntry.ItemName = ItemInfo.ItemName;
	NewEntry.EquipmentLevel = ItemInfo.EquipmentLevel;
	NewEntry.ModuleCapacity = ModuleCDO->Capacity;
	NewEntry.RarityTag = ModuleCDO->RarityTag;
	NewEntry.SlotTag = ModuleCDO->SocketTypeTag;
	NewEntry.RemoveAbilityTags = ModuleCDO->AbilitiesToRemove;
	NewEntry.EffectPackage = EffectPackage;

	// Apply Effects
	if (NewEntry.HasStats())
	{
		AddEquipmentStats(&NewEntry);
	}

	// for module, need to remove ability
	if (NewEntry.HasAbilities())
	{
		AddEquipmentAbility(&NewEntry);
	}

	MarkItemDirty(NewEntry);

	return NewEntry.ItemID;
}

void FFDEquipmentList::RemoveModuleEntry(int64 ModuleNum)
{
	check(OwnerComponent);

	for (auto EntryIt = EquipmentEntries.CreateIterator(); EntryIt; ++EntryIt)
	{		
		FFDEquipmentEntry& Entry = *EntryIt;
		
		if (!Entry.EntryTag.MatchesTag(FGameplayTag::RequestGameplayTag("Item.Equipment.Module")))
		{
			continue;
		}

		if (Entry.ItemID == ModuleNum)
		{
			// if (Entry.SlotTag.MatchesTag(FGameplayTag::RequestGameplayTag("Module.ChangeType.Tool")))
			if (Entry.EntryTag.MatchesTag(FGameplayTag::RequestGameplayTag("Item.Equipment.Module.Player.MidAirManeuvering")))
			{
				float MaxModuleCapacity = GetAbilitySystemComponent()->GetNumericAttribute(UPlayerAttributeSet::GetMaxModuleCapacityAttribute());
				float ModuleCapacity = GetAbilitySystemComponent()->GetNumericAttribute(UPlayerAttributeSet::GetModuleCapacityAttribute());
				if (ModuleCapacity > MaxModuleCapacity - Entry.EquipmentLevel)
				{
					break;
				}
				GetAbilitySystemComponent()->SetNumericAttributeBase(UPlayerAttributeSet::GetMaxModuleCapacityAttribute(), MaxModuleCapacity - Entry.EquipmentLevel);
			}
			RemoveEquipmentStats(&Entry);
			RemoveEquipmentAbility(&Entry);
			UnEquipmentEntryDelegate.Broadcast(Entry);
			EntryIt.RemoveCurrent();
			MarkArrayDirty();

			break;
		}
	}
}

void FFDEquipmentList::RemoveModuleEntryAndCapacity(int64 ModuleNum)
{
	check(OwnerComponent);

	for (auto EntryIt = EquipmentEntries.CreateIterator(); EntryIt; ++EntryIt)
	{		
		FFDEquipmentEntry& Entry = *EntryIt;
		
		if (!Entry.EntryTag.MatchesTag(FGameplayTag::RequestGameplayTag("Item.Equipment.Module")))
		{
			continue;
		}

		if (Entry.ItemID == ModuleNum)
		{
			float ModuleCapacity = GetAbilitySystemComponent()->GetNumericAttribute(UPlayerAttributeSet::GetModuleCapacityAttribute());
			// if (Entry.SlotTag.MatchesTag(FGameplayTag::RequestGameplayTag("Module.ChangeType.Tool")))
			if (Entry.EntryTag.MatchesTag(FGameplayTag::RequestGameplayTag("Item.Equipment.Module.Player.MidAirManeuvering")))
			{
				float MaxModuleCapacity = GetAbilitySystemComponent()->GetNumericAttribute(UPlayerAttributeSet::GetMaxModuleCapacityAttribute());
				if (ModuleCapacity > MaxModuleCapacity - Entry.EquipmentLevel)
				{
					break;
				}
				GetAbilitySystemComponent()->SetNumericAttributeBase(UPlayerAttributeSet::GetMaxModuleCapacityAttribute(), MaxModuleCapacity - Entry.EquipmentLevel);
			}
			else
			{
				GetAbilitySystemComponent()->SetNumericAttributeBase(UPlayerAttributeSet::GetModuleCapacityAttribute(), ModuleCapacity - GetEstimatedModuleCapacity(Entry, Entry.SlotNum));
			}
			RemoveEquipmentStats(&Entry);
			RemoveEquipmentAbility(&Entry);
			UnEquipmentEntryDelegate.Broadcast(Entry);
			EntryIt.RemoveCurrent();
			MarkArrayDirty();

			break;
		}
	}
}

void FFDEquipmentList::RestoreModule(FFDEquipmentEntry Entry)
{
	EModuleCapacity CapacityCalc;
	
	if (Cast<UEquipmentManagerComponent>(OwnerComponent)->GetModuleSlotInfos()[Entry.SlotNum].Sockets.IsEmpty())
	{
		CapacityCalc = EModuleCapacity::None;
	}
	else
	{
		CapacityCalc = EModuleCapacity::Double;
		if (Entry.SlotTag.MatchesTag(Cast<UEquipmentManagerComponent>(OwnerComponent)->GetModuleSlotInfos()[Entry.SlotNum].Sockets.First()))
		{
			CapacityCalc = EModuleCapacity::Half;
		}
	}

	float MaxModuleCapacity = GetAbilitySystemComponent()->GetNumericAttribute(UPlayerAttributeSet::GetMaxModuleCapacityAttribute());
	float ModuleCapacity = GetAbilitySystemComponent()->GetNumericAttribute(UPlayerAttributeSet::GetModuleCapacityAttribute());
	
	int32 CalculatedCapacity = Entry.ModuleCapacity;
	if (Entry.SlotNum == 0)
	{
		CalculatedCapacity -= Entry.EquipmentLevel;
	}
	else if (Entry.SlotNum == 1)
	{
		// Tool Module increases MaxCapacity when enchanted
		GetAbilitySystemComponent()->SetNumericAttributeBase(UPlayerAttributeSet::GetMaxModuleCapacityAttribute(), MaxModuleCapacity + Entry.EquipmentLevel);
	}
	else
	{
		CalculatedCapacity += Entry.EquipmentLevel;
	}
	
	if (CapacityCalc == EModuleCapacity::Half)
	{
		CalculatedCapacity += CalculatedCapacity%2;
		CalculatedCapacity /= 2;
	}
	else if (CapacityCalc == EModuleCapacity::Double)
	{
		CalculatedCapacity *= 2;
	}
	
	// should consider enchant count
	if (ModuleCapacity + CalculatedCapacity > MaxModuleCapacity)
	{
		return;
	}
	else
	{
		ModuleCapacity += CalculatedCapacity;
		GetAbilitySystemComponent()->SetNumericAttributeBase(UPlayerAttributeSet::GetModuleCapacityAttribute(), ModuleCapacity);
	}

	// Apply Effects
	if (Entry.HasStats())
	{
		AddEquipmentStats(&Entry);
	}

	// for module, need to remove ability
	if (Entry.HasAbilities())
	{
		AddEquipmentAbility(&Entry);
	}

	MarkItemDirty(Entry);
}

void FFDEquipmentList::ChangeModuleOrder(const FFDEquipmentEntry& SourceEntry, int DestinationSlotNum)
{
	bool bIsDestinationValid = false;
	int SourceCapacity = SourceEntry.ModuleCapacity + SourceEntry.EquipmentLevel;
	int DestinationCapacity = 0;
	int EstimatedSourceCapacity = 0;
	int64 DestinationItemID = 0;

	if (SourceEntry.SlotNum == 0 || SourceEntry.SlotNum == 1 || DestinationSlotNum == 0 || DestinationSlotNum == 1)
	{
		return;
	}

	float MaxModuleCapacity = GetAbilitySystemComponent()->GetNumericAttribute(UPlayerAttributeSet::GetMaxModuleCapacityAttribute());
	float ModuleCapacity = GetAbilitySystemComponent()->GetNumericAttribute(UPlayerAttributeSet::GetModuleCapacityAttribute());

	// Get Current Applied Capacity && DestinationValidation
	for (auto EntryIt = EquipmentEntries.CreateIterator(); EntryIt; ++EntryIt)
	{
		FFDEquipmentEntry& Entry = *EntryIt;

		if (!Entry.EntryTag.IsValid() || !Entry.EntryTag.MatchesTag(FGameplayTag::RequestGameplayTag("Item.Equipment.Module")))
		{
			continue;
		}

		if (Entry.SlotNum == SourceEntry.SlotNum)
		{
			if (!Cast<UEquipmentManagerComponent>(OwnerComponent)->GetModuleSlotInfos()[SourceEntry.SlotNum].Sockets.IsEmpty())
			{
				if (SourceEntry.SlotTag.MatchesTag(Cast<UEquipmentManagerComponent>(OwnerComponent)->GetModuleSlotInfos()[SourceEntry.SlotNum].Sockets.First()))
				{
					SourceCapacity = (SourceCapacity + SourceCapacity % 2) / 2;
				}
				else
				{
					SourceCapacity *= 2;
				}
			}
		}

		if (Entry.SlotNum == DestinationSlotNum)
		{
			DestinationItemID = Entry.ItemID;
			bIsDestinationValid = true;
			DestinationCapacity = Entry.ModuleCapacity + Entry.EquipmentLevel;
			EstimatedSourceCapacity = GetEstimatedModuleCapacity(Entry, SourceEntry.SlotNum);
			if (!Cast<UEquipmentManagerComponent>(OwnerComponent)->GetModuleSlotInfos()[Entry.SlotNum].Sockets.IsEmpty())
			{
				if (Entry.SlotTag.MatchesTag(Cast<UEquipmentManagerComponent>(OwnerComponent)->GetModuleSlotInfos()[Entry.SlotNum].Sockets.First()))
				{
					DestinationCapacity = (DestinationCapacity + DestinationCapacity % 2) / 2;
				}
				else
				{
					DestinationCapacity *= 2;
				}
			}
		}
	}

	if (bIsDestinationValid)
	{
		int EstimatedDestinationCapacity = GetEstimatedModuleCapacity(SourceEntry, DestinationSlotNum);
		if (EstimatedDestinationCapacity > 0 && EstimatedSourceCapacity > 0)
		{
			if (ModuleCapacity - SourceCapacity - DestinationCapacity + EstimatedSourceCapacity + EstimatedDestinationCapacity <= MaxModuleCapacity)
			{
				int CachedSourceSlotNum = SourceEntry.SlotNum;
				for (auto EntryIt = EquipmentEntries.CreateIterator(); EntryIt; ++EntryIt)
				{
					FFDEquipmentEntry& Entry = *EntryIt;

					if (Entry.ItemID == SourceEntry.ItemID)
					{
						Entry.SlotNum = DestinationSlotNum;
						MarkItemDirty(Entry);
					}
					else if (Entry.ItemID == DestinationItemID)
					{
						Entry.SlotNum = CachedSourceSlotNum;
						MarkItemDirty(Entry);
					}
				}
				GetAbilitySystemComponent()->SetNumericAttributeBase(UPlayerAttributeSet::GetModuleCapacityAttribute(), ModuleCapacity - SourceCapacity - DestinationCapacity + EstimatedSourceCapacity + EstimatedDestinationCapacity);
			}
		}
	}
	else
	{
		int EstimatedCapacity = GetEstimatedModuleCapacity(SourceEntry, DestinationSlotNum);
		if (EstimatedCapacity > 0)
		{
			if (ModuleCapacity - SourceCapacity + EstimatedCapacity <= MaxModuleCapacity)
			{
				for (auto EntryIt = EquipmentEntries.CreateIterator(); EntryIt; ++EntryIt)
				{
					FFDEquipmentEntry& Entry = *EntryIt;

					if (Entry.ItemID == SourceEntry.ItemID)
					{
						// F
						Entry.SlotNum = DestinationSlotNum;
						MarkItemDirty(Entry);
					}
				}
				GetAbilitySystemComponent()->SetNumericAttributeBase(UPlayerAttributeSet::GetModuleCapacityAttribute(), ModuleCapacity - SourceCapacity + EstimatedCapacity);
			}
		}
	}
}

void FFDEquipmentList::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	for (const int32 Index : RemovedIndices)
	{
		FFDEquipmentEntry& Entry = EquipmentEntries[Index];

		// EquipmentEntryDelegate.Broadcast(Entry);

		/*GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,
			FString::Printf("Unequiped Item : %s", *Entry.EntryTag.ToString()));*/
	}
}

void FFDEquipmentList::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	for (const int32 Index : AddedIndices)
	{
		FFDEquipmentEntry& Entry = EquipmentEntries[Index];

		EquipmentEntryDelegate.Broadcast(Entry);

		// if (Entry.Instance)
		// {
		// 	Entry.Instance.Get()->OnEquip(OwnerComponent->GetOwner(), Entry.bIsActiveWeapon);
		// }

		/*GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,
			FString::Printf("Equipped Item : %s", *Entry.EntryTag.ToString()));*/
	}
}

void FFDEquipmentList::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	for (const int32 Index : ChangedIndices)
	{
		FFDEquipmentEntry& Entry = EquipmentEntries[Index];

		EquipmentEntryDelegate.Broadcast(Entry);
	}
}

void FFDEquipmentList::MarkItemDirtyByItemID(int64 ItemID)
{
	for (auto EntryIt = EquipmentEntries.CreateIterator(); EntryIt; ++EntryIt)
	{
		FFDEquipmentEntry& Entry = *EntryIt;

		if (Entry.ItemID == ItemID)
		{
			MarkItemDirty(Entry);
		
			break;
		}
	}
}

void FFDEquipmentList::SpawnSetEquipmentActors(FFDEquipmentEntry& Entry, bool bIsActiveSlot, bool bIsSameUnarmedLocation)
{
	Entry.Instance->SpawnEquipmentActors(Entry.ActorsToSpawn, bIsActiveSlot, bIsSameUnarmedLocation);
	Entry.SetFireTime();
	if (bIsActiveSlot)
	{
		Entry.bIsActiveWeapon = true;
		// Entry.SetIsActiveWeapon();
	}
}

int FFDEquipmentList::GetEstimatedModuleCapacity(const FFDEquipmentEntry& InEntry, int SlotNum)
{
	if (!InEntry.EntryTag.IsValid() || InEntry.SlotNum < 0)
	{
		return -1;
	}
	int CalculatedCapacity = InEntry.ModuleCapacity;
	if (InEntry.EntryTag.MatchesTag(FGameplayTag::RequestGameplayTag("Item.Equipment.Module.Player.HighVoltage")))
	{
		CalculatedCapacity -= InEntry.EquipmentLevel;
	}
	else
	{
		CalculatedCapacity += InEntry.EquipmentLevel;
	}
	// Should be Revise
	if (InEntry.EntryTag.MatchesTag(FGameplayTag::RequestGameplayTag("Item.Equipment.Module.Player.MidAirManeuvering")))
	{
		CalculatedCapacity = 0;
	}
	FGameplayTagContainer SocketInfos = Cast<UEquipmentManagerComponent>(OwnerComponent)->GetModuleSlotInfos()[SlotNum].Sockets;
	if (!SocketInfos.IsEmpty())
	{
		if (InEntry.SlotTag.MatchesTag(SocketInfos.First()))
		{
			return (CalculatedCapacity + CalculatedCapacity % 2) / 2;
		}
		else
		{
			return CalculatedCapacity * 2;
		}
	}
	else
	{
		return CalculatedCapacity;
	}
}

UEquipmentManagerComponent::UEquipmentManagerComponent() : EquipmentList(this)
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;

	SetIsReplicatedByDefault(true);

}

void UEquipmentManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	InitModuleSlots();
	
	// FTimerHandle TimerHandle;
	//
	// GetWorld()->GetTimerManager().SetTimer(TimerHandle,
	// 	[this] ()
	// 	{
	// 		for (int i = 0; i < 12; i++)
	// 		{
	// 			ModuleSlotInfos.AddDefaulted();
	// 		}
	//
	// 		ModuleSlotInfos[6].Sockets.Add(FGameplayTag::RequestGameplayTag(FName("Module.SocketType.Almandine"), true));
	// 		ModuleSlotInfos[9].Sockets.Add(FGameplayTag::RequestGameplayTag(FName("Module.SocketType.Malachite"), true));			
	// 	},
	// 	3.f,
	// 	false
	// 	);
}

void UEquipmentManagerComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UEquipmentManagerComponent, EquipmentList);
	DOREPLIFETIME(UEquipmentManagerComponent, ModuleSlotInfos);
}

void UEquipmentManagerComponent::InitModuleSlots()
{
	for (int i = 0; i < 12; i++)
	{
		ModuleSlotInfos.AddDefaulted();
	}
	
	FGameplayTag AlmandineTag = FGameplayTag::RequestGameplayTag(
		FName("Module.SocketType.Almandine"), true);

	FGameplayTag MalachiteTag = FGameplayTag::RequestGameplayTag(
		FName("Module.SocketType.Malachite"), true);

	ModuleSlotInfos[6].Sockets.AddTag(AlmandineTag);
	ModuleSlotInfos[9].Sockets.AddTag(MalachiteTag);
}

void UEquipmentManagerComponent::EquipItem(const TSubclassOf<UEquipmentDefinition>& EquipmentDefinition,
                                           const FEquipmentEffectPackage& EffectPackage, int SlotNum, FExtraEquipmentInfo ItemInfo)
{
	if (!GetOwner()->HasAuthority())
	{
		ServerEquipItem(EquipmentDefinition, EffectPackage, SlotNum, ItemInfo);
		return;
	}
	if (UEquipmentInstance* EquipmentInstance = EquipmentList.AddEntry(EquipmentDefinition, EffectPackage, SlotNum, ItemInfo))
	{
		// FTimerHandle TimerHandle;
		// GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this, EquipmentInstance, SlotNum]()
		// {
		// 	EquipmentInstance->OnEquip(GetOwner(), GetActiveWeaponSlot() == SlotNum);
		// },
		// 0.2f,
		// false);
	}
}

void UEquipmentManagerComponent::EquipModule(const TSubclassOf<UModuleDefinition>& ModuleDefinition,
	const FEquipmentEffectPackage& EffectPackage, int SlotNum, FExtraEquipmentInfo ItemInfo)
{
	if (!GetOwner()->HasAuthority())
	{
		ServerEquipModule(ModuleDefinition, EffectPackage, SlotNum, ItemInfo);
		return;
	}
	
	int64 AddedItemID = EquipmentList.AddModuleEntry(ModuleDefinition, EffectPackage, SlotNum, ItemInfo);

	if (AddedItemID == 0)
	{
		FFDEquipmentEntry UnEquipmentEntry;
		const UModuleDefinition* ModuleCDO = GetDefault<UModuleDefinition>(ModuleDefinition);
		UnEquipmentEntry.EntryTag = ModuleCDO->ItemTag;
		UnEquipmentEntry.EquipmentLevel = ItemInfo.EquipmentLevel;
		UnEquipmentEntry.SlotNum = SlotNum;
		EquipmentList.UnEquipmentEntryDelegate.Broadcast(UnEquipmentEntry);
	}
}

void UEquipmentManagerComponent::UnequipItem(const FFDEquipmentEntry& Entry)
{
	if (!GetOwner()->HasAuthority())
	{
		ServerUnequipItem(Entry);
		return;
	}
	for (const FFDEquipmentEntry& EntryIt : EquipmentList.GetWeaponEntries())
	{
		if (EntryIt.ItemID == Entry.ItemID)
		{
			if (EntryIt.EntryTag.MatchesTag(FGameplayTag::RequestGameplayTag("Item.Equipment.Weapon")))
			{
				if (EntryIt.bIsActiveWeapon)
				{
					EntryIt.Instance->OnUnequip();
				}
				EquipmentList.RemoveEntry(EntryIt.Instance);
			}
			else if (EntryIt.EntryTag.MatchesTag(FGameplayTag::RequestGameplayTag("Item.Equipment.Module")))
			{
				EquipmentList.RemoveModuleEntryAndCapacity(EntryIt.ItemID);
			}
		}
	}
}

void UEquipmentManagerComponent::ChangeWeapon(int SlotNum)
{
	if (!GetOwner()->HasAuthority())
	{
		ServerChangeWeapon(SlotNum);
		return;
	}
	
	EquipmentList.ChangeActiveEntry(GetActiveWeaponSlot(), SlotNum);
	
}

void UEquipmentManagerComponent::ChangeOrderWeapon(int SlotNum1, int SlotNum2)
{
	if (!GetOwner()->HasAuthority())
	{
		ServerChangeOrderWeapon(SlotNum1, SlotNum2);
		return;
	}
	
	EquipmentList.ChangeWeaponOrder(GetActiveWeaponSlot(), SlotNum1, SlotNum2);
}

void UEquipmentManagerComponent::ChangeOrderModule(const FFDEquipmentEntry& SourceEntry, int DestinationSlotNum)
{
	if (!GetOwner()->HasAuthority())
	{
		ServerChangeOrderModule(SourceEntry, DestinationSlotNum);
		return;
	}
	
	EquipmentList.ChangeModuleOrder(SourceEntry, DestinationSlotNum);
	for (auto EntryIt = EquipmentList.GetWeaponEntries().CreateIterator(); EntryIt; ++EntryIt)
	{
		FFDEquipmentEntry& Entry = *EntryIt;

		// if (Entry.ItemID == SourceEntry.ItemID && Entry.SlotNum == SourceEntry.SlotNum)
		if (Entry.ItemID == SourceEntry.ItemID && Entry.SlotNum != DestinationSlotNum)
		{
			EquipmentList.MarkItemDirtyByItemID(Entry.ItemID);
			break;
		}
	}
}

int UEquipmentManagerComponent::GetActiveWeaponSlot()
{
	for (FFDEquipmentEntry EntryIt : EquipmentList.GetWeaponEntries())
	{
		if (not EntryIt.EntryTag.MatchesTag(FGameplayTag::RequestGameplayTag("Item.Equipment.Weapon")))
		{
			continue;
		}
		if (EntryIt.bIsActiveWeapon)
		{
			return EntryIt.SlotNum;
		}
	}
	return 0;
}

FGameplayTag UEquipmentManagerComponent::GetActiveWeaponAmmoType()
{
	for (FFDEquipmentEntry EntryIt : EquipmentList.GetWeaponEntries())
	{
		if (EntryIt.bIsActiveWeapon)
		{
			return EntryIt.AmmoType;
		}
	}
	return FGameplayTag();
}

TArray<FModuleSlotInfo> UEquipmentManagerComponent::GetModuleSlotInfos()
{
	return ModuleSlotInfos;
}

void UEquipmentManagerComponent::ServerChangeOrderModule_Implementation(const FFDEquipmentEntry& SourceEntry,
	int DestinationSlotNum)
{
	ChangeOrderModule(SourceEntry, DestinationSlotNum);
}

void UEquipmentManagerComponent::ServerEquipItem_Implementation(
	TSubclassOf<UEquipmentDefinition> EquipmentDefinition, const FEquipmentEffectPackage& EffectPackage, int SlotNum, FExtraEquipmentInfo ItemInfo)
{
	EquipItem(EquipmentDefinition, EffectPackage, SlotNum, ItemInfo);
}

void UEquipmentManagerComponent::ServerEquipModule_Implementation(TSubclassOf<UModuleDefinition> ModuleDefinition,
                                                                  const FEquipmentEffectPackage& EffectPackage, int SlotNum, FExtraEquipmentInfo ItemInfo)
{
	EquipModule(ModuleDefinition, EffectPackage, SlotNum, ItemInfo);
}

void UEquipmentManagerComponent::ServerChangeWeapon_Implementation(int SlotNum)
{
	ChangeWeapon(SlotNum);
}

void UEquipmentManagerComponent::ServerChangeOrderWeapon_Implementation(int SlotNum1, int SlotNum2)
{
	ChangeOrderWeapon(SlotNum1, SlotNum2);
}

void UEquipmentManagerComponent::ServerUnequipItem_Implementation(const FFDEquipmentEntry& Entry)
{
	UnequipItem(Entry);
}



