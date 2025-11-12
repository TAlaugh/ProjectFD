// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Equipment/EquipmentManagerComponent.h"
#include "WeaponSlotWidget.generated.h"

class UTextBlock;
class UImage;
class UWeaponHoveringWidget;
struct FFDInventoryEntry;
struct FMasterItemDefinition;
class UInventorySlotTextures;
struct FFDEquipmentEntry;

DECLARE_DELEGATE_TwoParams(FWeaponOrderChangeSignature, const FFDEquipmentEntry&, int);
DECLARE_DELEGATE_TwoParams(FWeaponEquipSignature, const FFDInventoryEntry&, int);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWeaponSlotSignature, const FFDEquipmentEntry&, EquipmentEntry);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWeaponSlotRightClickSignature, const FFDEquipmentEntry&, EquipmentEntry);
/**
 * 
 */
UCLASS()
class PROJECTFD_API UWeaponSlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	virtual void NativeOnInitialized() override;

	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;

	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	FWeaponOrderChangeSignature WeaponOrderChangeDelegate;
	FWeaponEquipSignature WeaponEquipDelegate;
	FWeaponSlotSignature WeaponSlotDelegate;
	FWeaponSlotRightClickSignature WeaponSlotRightClickDelegate;

	UFUNCTION()
	void SetSlotInfo(const FFDEquipmentEntry& Entry, UInventorySlotTextures* InventorySlotTextures, FMasterItemDefinition ItemDefinition);
	void SetSlotInfoBySlot(UWeaponSlotWidget* WeaponSlotWidget);
	void SetSlotDefault();
	void SetEquipped();
	void SetUnEquipped();
	
	UFUNCTION()
	void SetBackgroundImage(UTexture2D* BGTexture);

	UFUNCTION()
	void SetTierImage(FGameplayTag Tag);

	UFUNCTION()
	void SetWeaponImage(UTexture2D* WeaponTexture);

	UFUNCTION()
	void SetDPSText(float InDPS);

	UFUNCTION()
	void SetLevelText(FText InLevelText);

	UFUNCTION()
	void SetAmmoTypeImage(UTexture2D* AmmoTypeTexture);

	UFUNCTION()
	void SetSelectedImage(bool bShow);

	void InitSlotNum(int SlotNum);
	int64 GetItemID();

	
protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="WeaponSlot|Deco", meta = (BindWidget, AllowPrivateAccess=true))
	UImage* BackgroundImage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="WeaponSlot|Deco", meta = (BindWidget, AllowPrivateAccess=true))
	UImage* TierImage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="WeaponSlot", meta = (BindWidget, AllowPrivateAccess=true))
	UImage* WeaponImage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="WeaponSlot|Deco", meta = (BindWidget, AllowPrivateAccess=true))
	UImage* WeaponSelectedLeftImage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="WeaponSlot|Deco", meta = (BindWidget, AllowPrivateAccess=true))
	UImage* WeaponSelectedRightImage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="WeaponSlot|DPS", meta = (BindWidget, AllowPrivateAccess=true))
	class UHorizontalBox* DPSHorizontalBox;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="WeaponSlot|DPS", meta = (BindWidget, AllowPrivateAccess=true))
	UTextBlock* DPSValueText;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="WeaponSlot|Deco", meta = (BindWidget, AllowPrivateAccess=true))
	UImage* AmmoTypeImage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="WeaponSlot|Level", meta = (BindWidget, AllowPrivateAccess=true))
	UTextBlock* LevelText;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="WeaponSlot|Button", meta = (BindWidget, AllowPrivateAccess=true))
	class UButton* WeaponButton;


private:

	UFUNCTION()
	void OnWeaponSlotClicked();

	void OnWeaponSlotRightClicked();

	void ReplicateEntry(const FFDEquipmentEntry& Entry);

	void RemoveHoveringWidget();

	UFUNCTION()
	void OnDragOperationSuccess(UDragDropOperation* Operation);
	UFUNCTION()
	void OnDragOperationCancelled(UDragDropOperation* Operation);

	UPROPERTY()
	FFDEquipmentEntry CachedEquipmentEntry = FFDEquipmentEntry();

	UPROPERTY()
	TObjectPtr<UWeaponHoveringWidget> WeaponHoveringWidget;

	UPROPERTY(EditDefaultsOnly, Category="Custom Values|SlotWidget")
	TSubclassOf<UWeaponHoveringWidget> WeaponHoveringWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Asset")
	TObjectPtr<UTexture2D> UnEquippedWeaponImage;

	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Asset|Sound")
	TObjectPtr<USoundBase> EquipSFX;

	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Asset|Sound")
	TObjectPtr<USoundBase> UnEquipSFX;

	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Asset|Sound")
	TObjectPtr<USoundBase> HoverSFX;

	bool bIsDragged = false;
	
};
