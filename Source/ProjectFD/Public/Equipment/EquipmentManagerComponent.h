// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EquipmentInstance.h"
#include "EquipmentTypes.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "EquipmentDefinition.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "EquipmentManagerComponent.generated.h"

class UModuleDefinition;
class UPlayerAbilitySystemComponent;
class UEquipmentInstance;

// UENUM()
// enum EModuleSocketType
// {
// 	Almandine,
// 	Malachite,
// 	Cerulean,
// 	Xantic,
// 	Rutile
// };

UENUM()
enum EModuleCapacity
{
	None,
	Half,
	Double
};

USTRUCT(BlueprintType)
struct FExtraEquipmentInfo
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly)
	int64 ItemID = 0;

	UPROPERTY(BlueprintReadOnly)
	FText ItemName = FText();

	UPROPERTY(BlueprintReadOnly)
	int EquipmentLevel = 0;
	
};

USTRUCT(BlueprintType)
struct FModuleSlotInfo
{
	GENERATED_BODY()

	UPROPERTY()
	// TArray<EModuleSocketType> Sockets = TArray<EModuleSocketType>();
	FGameplayTagContainer Sockets = FGameplayTagContainer();

};

USTRUCT(BlueprintType)
struct FFDEquipmentEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FGameplayTag EntryTag = FGameplayTag();

	UPROPERTY(BlueprintReadOnly)
	int64 ItemID = 0;

	UPROPERTY(BlueprintReadOnly)
	FText ItemName = FText();

	UPROPERTY(BlueprintReadOnly)
	FGameplayTag AmmoType = FGameplayTag();

	UPROPERTY(BlueprintReadOnly)
	FGameplayTag SlotTag = FGameplayTag();

	UPROPERTY(BlueprintReadOnly)
	int SlotNum = 0;

	UPROPERTY(BlueprintReadOnly)
	int EquipmentLevel = 0;

	UPROPERTY(BlueprintReadOnly)
	int ModuleCapacity = 0;

	UPROPERTY(BlueprintReadOnly)
	FGameplayTag RarityTag = FGameplayTag();

	UPROPERTY(BlueprintReadOnly)
	int LoadedAmmo = 0;

	UPROPERTY()
	FGameplayTagContainer RemoveAbilityTags = FGameplayTagContainer();

	UPROPERTY(BlueprintReadOnly)
	FEquipmentEffectPackage EffectPackage = FEquipmentEffectPackage();

	UPROPERTY(NotReplicated)
	FEquipmentGrantedHandles GrantedHandles = FEquipmentGrantedHandles();

	UPROPERTY()
	TArray<FEquipmentActorToSpawn> ActorsToSpawn = TArray<FEquipmentActorToSpawn>();

	UPROPERTY()
	bool bIsActiveWeapon = false;

	bool HasStats() const
	{
		return !EffectPackage.StatEffects.IsEmpty();
	}

	bool HasAbilities() const
	{
		return !EffectPackage.Abilities.IsEmpty();
	}

	void SetIsActiveWeapon()
	{
		bIsActiveWeapon = Instance.Get()->bIsActiveWeapon;
	}
	// Combine!!
	void OnEquip(AActor* Owner, bool bIsActive) const;

	float GetFireTime();
	void SetFireTime();
	void SetFireTime(float InFireTime)
	{
		FireTime = InFireTime;
	}

	TSubclassOf<UAnimInstance> GetCharacterAnimLayer() const;
	
	UPROPERTY()
	float FireTime = 0.f;

private:

	friend class UEquipmentManagerComponent;
	friend struct FFDEquipmentList;

	// UPROPERTY()
	// TSubclassOf<UEquipmentDefinition> EquipmentDefinition = nullptr;

	UPROPERTY()
	TSubclassOf<UAnimInstance> CharacterAnimLayer = nullptr;

	UPROPERTY()
	TObjectPtr<UEquipmentInstance> Instance = nullptr;
};

DECLARE_MULTICAST_DELEGATE_OneParam(FFDEquipmentEntrySignature, const FFDEquipmentEntry& );
DECLARE_MULTICAST_DELEGATE_OneParam(FFDUnEquipmentEntrySignature, const FFDEquipmentEntry& );

USTRUCT()
struct FFDEquipmentList : public FFastArraySerializer
{
	GENERATED_BODY()

	FFDEquipmentList() : OwnerComponent(nullptr) { }
	FFDEquipmentList(UActorComponent* InComponent) : OwnerComponent(InComponent) { }

	TArray<FFDEquipmentEntry> GetWeaponEntries();
	void SetWeaponEntries(TArray<FFDEquipmentEntry> InEntries);

	UPlayerAbilitySystemComponent* GetAbilitySystemComponent();
	void AddEquipmentStats(FFDEquipmentEntry* Entry);
	void RemoveEquipmentStats(FFDEquipmentEntry* Entry);
	void AddEquipmentAbility(FFDEquipmentEntry* Entry);
	void RemoveEquipmentAbility(FFDEquipmentEntry* Entry);

	void ChangeActiveEntry(int ActiveWeaponSlot, int SlotNum);
	void ChangeWeaponOrder(int ActiveWeaponSlot, int SlotNum1, int SlotNum2);
	void ChangeModuleOrder(const FFDEquipmentEntry& SourceEntry, int DestinationSlotNum);

	UEquipmentInstance* AddEntry(const TSubclassOf<UEquipmentDefinition>& EquipmentDefinition, const FEquipmentEffectPackage& EffectPackage, int SlotNum, FExtraEquipmentInfo ItemInfo);
	void RemoveEntry(UEquipmentInstance* EquipmentInstance);
	int64 AddModuleEntry(const TSubclassOf<UModuleDefinition>& ModuleDefinition, const FEquipmentEffectPackage& EffectPackage, int SlotNum, FExtraEquipmentInfo ItemInfo);
	void RemoveModuleEntry(int64 ModuleNum);
	void RemoveModuleEntryAndCapacity(int64 ModuleNum);

	void RestoreModule(FFDEquipmentEntry Entry);

	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);

	void MarkItemDirtyByItemID(int64 ItemID);

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FastArrayDeltaSerialize<FFDEquipmentEntry, FFDEquipmentList>(EquipmentEntries, DeltaParms, *this);
	}

	FFDEquipmentEntrySignature EquipmentEntryDelegate;
	FFDUnEquipmentEntrySignature UnEquipmentEntryDelegate;
	
private:
	
	UPROPERTY()
	TArray<FFDEquipmentEntry> EquipmentEntries;

	UPROPERTY()
	TObjectPtr<UActorComponent> OwnerComponent;

	void SpawnSetEquipmentActors(FFDEquipmentEntry& Entry, bool bIsActiveSlot, bool bIsSameUnarmedLocation);
	int GetEstimatedModuleCapacity(const FFDEquipmentEntry& InEntry, int SlotNum);
};

template<>
struct TStructOpsTypeTraits<FFDEquipmentList> : TStructOpsTypeTraitsBase2<FFDEquipmentList>
{
	enum
	{
		WithNetDeltaSerializer = true
	};
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTFD_API UEquipmentManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	// to private...
	UPROPERTY(Replicated)
	FFDEquipmentList EquipmentList;
	
	UEquipmentManagerComponent();

	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	void InitModuleSlots();

	void EquipItem(const TSubclassOf<UEquipmentDefinition>& EquipmentDefinition, const FEquipmentEffectPackage& EffectPackage, int SlotNum, FExtraEquipmentInfo ItemInfo);
	void EquipModule(const TSubclassOf<UModuleDefinition>& ModuleDefinition, const FEquipmentEffectPackage& EffectPackage, int SlotNum, FExtraEquipmentInfo ItemInfo);
	void UnequipItem(const FFDEquipmentEntry& Entry);
	void ChangeWeapon(int SlotNum);
	void ChangeOrderWeapon(int SlotNum1, int SlotNum2);
	void ChangeOrderModule(const FFDEquipmentEntry& SourceEntry, int DestinationSlotNum);

	int GetActiveWeaponSlot();
	FGameplayTag GetActiveWeaponAmmoType();
	TArray<FModuleSlotInfo> GetModuleSlotInfos();

private:

	UFUNCTION(Server, Reliable)
	void ServerEquipItem(TSubclassOf<UEquipmentDefinition> EquipmentDefinition, const FEquipmentEffectPackage& EffectPackage, int SlotNum, FExtraEquipmentInfo ItemInfo);

	UFUNCTION(Server, Reliable)
	void ServerEquipModule(TSubclassOf<UModuleDefinition> ModuleDefinition, const FEquipmentEffectPackage& EffectPackage, int SlotNum, FExtraEquipmentInfo ItemInfo);

	UFUNCTION(Server, Reliable)
	void ServerUnequipItem(const FFDEquipmentEntry& Entry);

	UFUNCTION(Server, Reliable)
	void ServerChangeWeapon(int SlotNum);
	UFUNCTION(Server, Reliable)
	void ServerChangeOrderWeapon(int SlotNum1, int SlotNum2);
	UFUNCTION(Server, Reliable)
	void ServerChangeOrderModule(const FFDEquipmentEntry& SourceEntry, int DestinationSlotNum);
	
	UPROPERTY(Replicated)
	TArray<FModuleSlotInfo> ModuleSlotInfos = TArray<FModuleSlotInfo>();
	
};
