// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "ItemTypes.h"
#include "Components/ActorComponent.h"
#include "Equipment/EquipmentManagerComponent.h"
#include "Equipment/EquipmentTypes.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "InventoryComponent.generated.h"

class UInventoryComponent;
class UEquipmentStatEffects;
class UItemTypesToTables;

DECLARE_MULTICAST_DELEGATE_FourParams(FEquipmentItemUsed, const TSubclassOf<UEquipmentDefinition>&, const FEquipmentEffectPackage& , int, FExtraEquipmentInfo);
DECLARE_MULTICAST_DELEGATE_FourParams(FModuleItemUsed, const TSubclassOf<UModuleDefinition>&, const FEquipmentEffectPackage& , int, FExtraEquipmentInfo);

USTRUCT(BlueprintType)
struct FFDInventoryEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FGameplayTag ItemTag = FGameplayTag();

	UPROPERTY(BlueprintReadOnly)
	FText ItemName = FText();

	UPROPERTY(BlueprintReadOnly)
	int32 Quantity = 0;

	UPROPERTY(BlueprintReadOnly)
	int64 ItemID = 0;

	UPROPERTY()
	int32 Level = 0;

	UPROPERTY(BlueprintReadOnly)
	FEquipmentEffectPackage EffectPackage = FEquipmentEffectPackage();

	bool IsValid() const
	{
		return ItemID != 0;
	}
};

DECLARE_MULTICAST_DELEGATE_OneParam(FDirtyInventoryItemSignature, const FFDInventoryEntry& );
DECLARE_MULTICAST_DELEGATE_OneParam(FDirtyInventoryItemRemoveSignature, const int64 );

USTRUCT()
struct FFDInventoryList : public FFastArraySerializer
{
	GENERATED_BODY()

	FFDInventoryList() : OwnerComponent(nullptr) { }

	FFDInventoryList(UInventoryComponent* InComponent) : OwnerComponent(InComponent) { }

	void SetInventoryEntries(TArray<FFDInventoryEntry> InEntries);

	void AddItem(const FGameplayTag& ItemTag, const FItemAcquisitionInfo& AcquisitionInfo, int32 NumItems = 1);
	void RemoveItem(const FFDInventoryEntry& InventoryEntry, int32 NumItems = 1);
	bool HasEnough(int64 ItemID, int32 NumItems = 1);
	uint64 GenerateID();
	void SetStats(UEquipmentStatEffects* InStats);
	void RollForStats(const TSubclassOf<UEquipmentDefinition> EquipmentDefinition, FFDInventoryEntry* Entry);
	void RollForModuleStats(const TSubclassOf<UModuleDefinition> ModuleDefinition, FFDInventoryEntry* Entry);
	void RollAcquisitionInfo(FFDInventoryEntry* Entry, const FItemAcquisitionInfo& AcquisitionInfo);
	void AddUnEquippedItem(const FFDEquipmentEntry& UnEquippedEntry, const FEquipmentEffectPackage& EffectPackage);
	void AddUnEquippedModule(const FFDEquipmentEntry& UnEquippedEntry, const FEquipmentEffectPackage& EffectPackage);

	void ClearGarbage();

	// FFastArraySerializer Contract
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);

	// Only Replicated Changed Ones
	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FastArrayDeltaSerialize<FFDInventoryEntry, FFDInventoryList>(Entries, DeltaParms, *this);
	}

	FDirtyInventoryItemSignature DirtyItemDelegate;
	FDirtyInventoryItemRemoveSignature InventoryItemRemovedDelegate;

private:

	friend class UInventoryComponent;

	UPROPERTY()
	TArray<FFDInventoryEntry> Entries;

	UPROPERTY(NotReplicated)
	TObjectPtr<UInventoryComponent> OwnerComponent;

	UPROPERTY(NotReplicated)
	uint64 LastAssignedID = 0;

	UPROPERTY(NotReplicated)
	TWeakObjectPtr<UEquipmentStatEffects> WeakStats;
};

template<>
struct TStructOpsTypeTraits<FFDInventoryList> : TStructOpsTypeTraitsBase2<FFDInventoryList>
{
	enum
	{
		WithNetDeltaSerializer = true
	};
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTFD_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UPROPERTY(Replicated)
	FFDInventoryList InventoryList;
	
	UInventoryComponent();

	FEquipmentItemUsed EquipmentItemDelegate;
	FModuleItemUsed ModuleItemDelegate;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable)
	void AddItem(const FGameplayTag ItemTag, const FItemAcquisitionInfo& AcquisitionInfo, int32 NumItems = 1);

	UFUNCTION(BlueprintCallable)
	void UseItem(const FFDInventoryEntry& Entry, int32 NumItems, int SlotNum);
	
	UFUNCTION(BlueprintPure)
	FMasterItemDefinition GetItemDefinitionByTag(const FGameplayTag& ItemTag) const;

	TArray<FFDInventoryEntry> GetInventoryEntries();
	void AddUnEquippedItemEntry(const FFDEquipmentEntry& UnEquippedEntry, const FEquipmentEffectPackage& EffectPackage);
	void AddUnEquippedModuleEntry(const FFDEquipmentEntry& UnEquippedEntry, const FEquipmentEffectPackage& EffectPackage);

	float GetFireMontageLength(int64 ItemID);
	
protected:
	virtual void BeginPlay() override;

private:

	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Stat Effects")
	TObjectPtr<UEquipmentStatEffects> StatEffects;

	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Item Definitions")
	TObjectPtr<UItemTypesToTables> InventoryDefinitions;
	
	UFUNCTION(Server, Reliable)
	void ServerAddItem(const FGameplayTag& ItemTag, const FItemAcquisitionInfo& AcquisitionInfo, int32 NumItems);

	UFUNCTION(Server, Reliable)
	void ServerUseItem(const FFDInventoryEntry& Entry, int32 NumItems, int SlotNum);
	
};
