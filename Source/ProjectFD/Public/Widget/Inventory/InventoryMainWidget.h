// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widget/BaseWidget.h"
#include "InventoryMainWidget.generated.h"

class UModuleInventoryWidget;
class UHorizontalBox;
class UInputMappingContext;
class UInputAction;
class UWeaponInventoryWidget;
struct FFDEquipmentEntry;
class UInventorySlotTextures;
class UEquipmentManagerComponent;
class AFDPlayerController;
struct FFDInventoryEntry;
class UInventoryComponent;
class UInventoryWidgetController;
class UWeaponSlotWidget;
class UMenuButtonWidget;
class UOverlay;
class UVerticalBox;
class USpacer;
class UTextBlock;
class USizeBox;
class UButton;
class UImage;

UENUM()
enum ImageAlignment
{
	Left,
	Middle,
	Right,
	Hide
};

/**
 * 
 */
UCLASS()
class PROJECTFD_API UInventoryMainWidget : public UBaseWidget
{
	GENERATED_BODY()
	
public:

	virtual void NativeOnInitialized() override;

	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

	virtual void OnWidgetControllerSet() override;

	void SetPreviewImage(UTextureRenderTarget2D* RTTexture);

	UFUNCTION() // WeaponList
	void CreateEquipmentList(const FFDInventoryEntry& Entry, float FireTime);

	UFUNCTION()
	void CreatePlayerModuleList(const FFDInventoryEntry& Entry);
	
	UFUNCTION()
	void CreateEquipmentSlots();

	UFUNCTION()
	void ShowWeaponInventory(bool bShow);

	UFUNCTION()
	void ShowPlayerModuleInventory(bool bShow);

	UFUNCTION()
	UInventoryComponent* GetInventoryComponent();

	void SetUnEquippedWeaponSlots(int SlotNum);
	void SetUnEquippedModuleSlots(int SlotNum);
	void SetModuleEffectSlot();


protected:

	friend class UModuleInventoryWidget;

	UPROPERTY()
	UInventoryWidgetController* CachedInventoryWidgetController = nullptr;

	UPROPERTY()
	AFDPlayerController* CachedPlayerController = nullptr;
	
	UPROPERTY()
	UInventoryComponent* CachedInventoryComponent = nullptr;

	UPROPERTY()
	UEquipmentManagerComponent* CachedEquipmentComponent = nullptr;

	// Preview Character && Background
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Inventory", meta = (BindWidget, AllowPrivateAccess=true))
	UImage* PreviewCharacterImage;


#pragma region MenuBar
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Menu", meta = (BindWidget, AllowPrivateAccess=true))
	UHorizontalBox* MenuHorizontalBox;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Menu|Shop", meta = (BindWidget, AllowPrivateAccess=true))
	UImage* BackgroundImage1;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Menu|Shop", meta = (BindWidget, AllowPrivateAccess=true))
	UButton* ShopButton;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Menu|BattlePass", meta = (BindWidget, AllowPrivateAccess=true))
	UImage* BackgroundImage2;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Menu|BattlePass", meta = (BindWidget, AllowPrivateAccess=true))
	UButton* BattlePassButton;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Menu|Customizing", meta = (BindWidget, AllowPrivateAccess=true))
	UImage* BackgroundImage3;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Menu|Customizing", meta = (BindWidget, AllowPrivateAccess=true))
	UButton* CustomizingButton;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Menu|Inventory", meta = (BindWidget, AllowPrivateAccess=true))
	UImage* BackgroundImage4;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Menu|Inventory", meta = (BindWidget, AllowPrivateAccess=true))
	UButton* InventoryButton;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Menu|Operator", meta = (BindWidget, AllowPrivateAccess=true))
	UImage* BackgroundImage5;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Menu|Operator", meta = (BindWidget, AllowPrivateAccess=true))
	UButton* OperatorButton;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Menu|Consumable", meta = (BindWidget, AllowPrivateAccess=true))
	UImage* BackgroundImage6;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Menu|Consumable", meta = (BindWidget, AllowPrivateAccess=true))
	UButton* ConsumableButton;

	
#pragma endregion


	// Main Content
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Inventory|InventoryWrap", meta = (BindWidget, AllowPrivateAccess=true))
	UHorizontalBox* InventoryWrapHorizontalBox;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Inventory|MainLeft", meta = (BindWidget, AllowPrivateAccess=true))
	UVerticalBox* MainLeftVerticalBox;

	
	// Player Info
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Inventory|PlayerInfo", meta = (BindWidget, AllowPrivateAccess=true))
	UVerticalBox* PlayerInfoVerticalBox;


	// Weapon Info
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Inventory|Weapon", meta = (BindWidget, AllowPrivateAccess=true))
	UVerticalBox* WeaponVerticalBox;


	// ETCMenu
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Inventory|ETC", meta = (BindWidget, AllowPrivateAccess=true))
	UVerticalBox* ETCVerticalBox;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Inventory|ETC", meta = (BindWidget, AllowPrivateAccess=true))
	UButton* WeaponBookButton;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Inventory|ETC", meta = (BindWidget, AllowPrivateAccess=true))
	UButton* PlayerModuleButton;


	// Equipment Inventories(Widgets & Classes)
	UPROPERTY()
	UWeaponInventoryWidget* WeaponInventoryWidget;

	UPROPERTY()
	UModuleInventoryWidget* ModuleInventoryWidget;

	UPROPERTY(EditDefaultsOnly, Category="Custom Values|SlotWidget")
	TSubclassOf<UWeaponInventoryWidget> WeaponInventoryClass;

	UPROPERTY(EditDefaultsOnly, Category="Custom Values|SlotWidget")
	TSubclassOf<UModuleInventoryWidget> ModuleInventoryClass;

	UPROPERTY(EditDefaultsOnly, Category="Custom Values|SlotWidget")
	TSubclassOf<UWeaponSlotWidget> WeaponSlotClass;

private:

	UFUNCTION()
	void SetPreviewImageAlignment(ImageAlignment Location);

	UFUNCTION()
	void OnMenuInventoryButtonClicked();

	UFUNCTION()
	void OnWeaponBookButtonClicked();

	UFUNCTION()
	void OnPlayerModuleButtonClicked();

	UFUNCTION()
	void OnWeaponSlotClicked(const FFDEquipmentEntry& EquipmentEntry);

	UFUNCTION()
	void OnWeaponSlotRightClicked(const FFDEquipmentEntry& EquipmentEntry);
	
	UFUNCTION()
	void OnWeaponEquip(const FFDInventoryEntry& Entry);
	
	UFUNCTION()
	void OnWeaponDropEquip(const FFDInventoryEntry& Entry, int DropSlotNum);

	UFUNCTION()
	void OnWeaponOrderChange(const FFDEquipmentEntry& EquipmentEntry, int DropSlotNum);

	UFUNCTION()
	void OnCloseInventory();

	UFUNCTION()
	void OnEscapeWidget();

	
	UPROPERTY(EditDefaultsOnly, meta=(AllowPrivateAccess = true))
	UInventorySlotTextures* InventorySlotTextures = nullptr;

	FDelegateHandle EquipmentEntryDelegateHandle;	

	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Inputs")
	TObjectPtr<UInputMappingContext> InventoryInputMappingContext;
	
	UPROPERTY(EditDefaultsOnly, Category = "Custom Values|Inputs")
	TObjectPtr<UInputAction> IA_CloseInventory;

	UPROPERTY(EditDefaultsOnly, Category = "Custom Values|Inputs")
	TObjectPtr<UInputAction> IA_Escape;

	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Asset|Sound")
	TObjectPtr<USoundBase> EquipSFX;
	
};

