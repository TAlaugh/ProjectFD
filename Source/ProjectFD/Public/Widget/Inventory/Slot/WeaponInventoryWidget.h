// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WeaponInventoryWidget.generated.h"

class UWeaponSlotSmallWidget;
struct FMasterItemDefinition;
class UInventorySlotTextures;
struct FFDInventoryEntry;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWeaponInventorySignature, const FFDInventoryEntry&, Entry);

/**
 * 
 */
UCLASS()
class PROJECTFD_API UWeaponInventoryWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	virtual void NativeOnInitialized() override;

	virtual void NativeConstruct() override;

	void CreateSmallWeaponSlot(const FFDInventoryEntry& Entry, UInventorySlotTextures* TextureMap, FMasterItemDefinition ItemDefinition, float FireTime);
	void DestroySmallWeaponSlot(int64 ItemID);
	void ClearSmallWeaponSlots();

	FWeaponInventorySignature WeaponInventoryDelegate;

	UPROPERTY(EditDefaultsOnly, Category="Custom Value|SlotWidget")
	TSubclassOf<UWeaponSlotSmallWidget> WeaponSlotSmallClass;


protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="WeaponInventory", meta = (BindWidget, AllowPrivateAccess=true))
	class UVerticalBox* WeaponInventoryVerticalBox;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="WeaponInventory", meta = (BindWidget, AllowPrivateAccess=true))
	class UHorizontalBox* SortingHorizontalBox;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="WeaponInventory", meta = (BindWidget, AllowPrivateAccess=true))
	class UWrapBox* WeaponWrapBox;

private:

	UFUNCTION()
	void BroadcastWeaponEquip(const FFDInventoryEntry& Entry);
	
};
