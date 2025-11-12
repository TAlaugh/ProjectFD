// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ModuleInventoryWidget.generated.h"

struct FGameplayTag;
class UFilterOptionWidget;
class UMenuAnchor;
class UButton;
class UVerticalBox;
struct FFDEquipmentEntry;
class UInventoryMainWidget;
class UInventoryComponent;
class UEquipmentManagerComponent;
class UInventorySlotTextures;
struct FMasterItemDefinition;
struct FFDInventoryEntry;
class UWrapBox;
class UModuleSlotWidget;
class UModuleEquippedSlotWidget;
class UScrollBox;
class UImage;
class UTextBlock;

UENUM()
enum EModuleSortType
{
	Newest,
	Rarity,
	Capacity,
	Quantity,
	Enchant,
	Socket,
	Name
};

/**
 * 
 */
UCLASS()
class PROJECTFD_API UModuleInventoryWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	virtual void NativeOnInitialized() override;

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	void SetAppliedEffectBox();
	void SetEquippedModuleSlot();
	void SetEquippedModuleSlotToDefault(int SlotNum);
	void CreateUnEquippedModuleSlot(const FFDInventoryEntry& Entry, UInventorySlotTextures* TextureMap, FMasterItemDefinition ItemDefinition);
	void DestroyUnEquippedModuleSlot(int64 ItemID);
	void ClearAllUnEquippedSlots();

	void SetInventoryMainWidget(UInventoryMainWidget* InInventoryMainWidget);
	void SetInventoryComponent(UInventoryComponent* InInventoryComponent);
	void SetEquipmentComponent(UEquipmentManagerComponent* InEquipmentComponent);

protected:

	UFUNCTION()
	void OnModuleEquip(const FFDInventoryEntry& Entry, int SlotNum);
	UFUNCTION()
	void OnModuleUnEquip(const FFDEquipmentEntry& Entry);
	UFUNCTION()
	void OnModuleOrderChange(const FFDEquipmentEntry& SourceEntry, int DestinationSlotNum);

	UFUNCTION()
	void OnOrderButtonClicked();
	UFUNCTION()
	void OnSortButtonClicked();
	UFUNCTION()
	UWidget* OnSortMenuAnchorActivate();

	void SetModuleCapacity(float CurrentCapacity);
	void SetMaxModuleCapacity(float MaxCapacity);
	void SetModuleCostInfo();
	void SetOrderImage();

	void SortUnEquippedModules(int SortType);

	// CharacterInfo
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="ModuleInventory|CharacterInfo", meta = (BindWidget, AllowPrivateAccess=true))
	UTextBlock* CharacterLevelText;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="ModuleInventory|CharacterInfo", meta = (BindWidget, AllowPrivateAccess=true))
	UTextBlock* CharacterNameText;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="ModuleInventory|CharacterInfo", meta = (BindWidget, AllowPrivateAccess=true))
	UImage* CharacterImage;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="ModuleInventory|CharacterInfo", meta = (BindWidget, AllowPrivateAccess=true))
	UScrollBox* AppliedEffectScrollBox;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="ModuleInventory|CharacterInfo", meta = (BindWidget, AllowPrivateAccess=true))
	UVerticalBox* AppliedStatsScrollBox;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="ModuleInventory|CharacterInfo", meta = (BindWidget, AllowPrivateAccess=true))
	UVerticalBox* AppliedAbilityScrollBox;

	// EquippedModule
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="ModuleInventory|EquippedModuleInfo", meta = (BindWidget, AllowPrivateAccess=true))
	UWrapBox* EquippedModuleContainer;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="ModuleInventory|EquippedModuleInfo", meta = (BindWidget, AllowPrivateAccess=true))
	UTextBlock* ModuleCostInfoText;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="ModuleInventory|EquippedModuleInfo", meta = (BindWidget, AllowPrivateAccess=true))
	class UProgressBar* ModuleCostProgressBar;

	// Need to add Filter
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="ModuleInventory|Filter", meta = (BindWidget, AllowPrivateAccess=true))
	UImage* OrderAscendingImage;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="ModuleInventory|Filter", meta = (BindWidget, AllowPrivateAccess=true))
	UImage* OrderDescendingImage;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="ModuleInventory|Filter", meta = (BindWidget, AllowPrivateAccess=true))
	UButton* OrderButton;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="ModuleInventory|Filter", meta = (BindWidget, AllowPrivateAccess=true))
	UTextBlock* SortTypeText;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="ModuleInventory|Filter", meta = (BindWidget, AllowPrivateAccess=true))
	UButton* SortButton;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="ModuleInventory|Filter", meta = (BindWidget, AllowPrivateAccess=true))
	UMenuAnchor* SortMenuAnchor;
	
	// UnEquippedModule
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="ModuleInventory|UnEquippedModuleInfo", meta = (BindWidget, AllowPrivateAccess=true))
	UWrapBox* UnEquippedModuleContainer;


private:

	void BindCallbacksToDependencies();
	int32 GetRarityPriority(FGameplayTag RarityTag);

	float ModuleCapacity = -1;
	float MaxModuleCapacity = -1;
	bool bIsAscending = true;
	EModuleSortType CurrentSortType = Name;

	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Class", meta=(AllowPrivateAccess=true))
	TSubclassOf<UModuleEquippedSlotWidget> ModuleEquippedSlotClass;
	
	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Class", meta=(AllowPrivateAccess=true))
	TSubclassOf<UModuleSlotWidget> ModuleSlotClass;
	
	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Class", meta=(AllowPrivateAccess=true))
	TSubclassOf<UFilterOptionWidget> FilterOptionSlotClass;
	
	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Asset", meta=(AllowPrivateAccess=true))
	UInventorySlotTextures* InventorySlotTextures;
	
	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Asset|Sound")
	TObjectPtr<USoundBase> EquipModuleSFX;

	UPROPERTY()
	UInventoryMainWidget* CachedInventoryMainWidget = nullptr;

	UPROPERTY()
	UInventoryComponent* CachedInventoryComponent = nullptr;
	
	UPROPERTY()
	UEquipmentManagerComponent* CachedEquipmentComponent = nullptr;

	FDelegateHandle EquipmentEntryDelegateHandle;
	
	
};
