// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inventory/InventoryComponent.h"
#include "WeaponSlotSmallWidget.generated.h"

class UWeaponHoveringWidget;
struct FMasterItemDefinition;
class UInventorySlotTextures;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWeaponSmallSlotSignature, const FFDInventoryEntry&, Entry);

/**
 * 
 */
UCLASS()
class PROJECTFD_API UWeaponSlotSmallWidget : public UUserWidget
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

	FWeaponSmallSlotSignature WeaponSmallSlotClickedDelegate;

	UFUNCTION()
	void SetSlotInfo(const FFDInventoryEntry& Entry, UInventorySlotTextures* InventorySlotTextures, FMasterItemDefinition ItemDefinition, float FireTime);
	
	UFUNCTION()
	void SetTierImage(FGameplayTag TierTag);

	UFUNCTION()
	void SetWeaponImage(UTexture2D* WeaponTexture);

	UFUNCTION()
	void SetDPSText(float InDPS);

	UFUNCTION()
	void SetLevelText(FText InLevelText);

	FFDInventoryEntry GetInventoryEntry();


protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="WeaponSlot", meta = (BindWidget, AllowPrivateAccess=true))
	class UImage* TierImage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="WeaponSlot", meta = (BindWidget, AllowPrivateAccess=true))
	class UImage* WeaponImage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="WeaponSlot|DPS", meta = (BindWidget, AllowPrivateAccess=true))
	class UHorizontalBox* DPSHorizontalBox;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="WeaponSlot|DPS", meta = (BindWidget, AllowPrivateAccess=true))
	class UTextBlock* DPSValueText;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="WeaponSlot|Level", meta = (BindWidget, AllowPrivateAccess=true))
	class UTextBlock* LevelText;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="WeaponSlot|Button", meta = (BindWidget, AllowPrivateAccess=true))
	class UButton* WeaponButton;


private:

	UFUNCTION()
	void OnWeaponSlotClicked();

	UFUNCTION()
	void ReplicateInventoryEntry(const FFDInventoryEntry& InEntry);

	void RemoveHoveringWidget();

	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Asset|Sound")
	TObjectPtr<USoundBase> EquipSFX;

	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Asset|Sound")
	TObjectPtr<USoundBase> HoverSFX;

	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Class")
	TSubclassOf<UWeaponHoveringWidget> WeaponHoveringWidgetClass;
	UPROPERTY()
	UWeaponHoveringWidget* WeaponHoveringWidget = nullptr;

	FFDInventoryEntry InventoryEntry;
	FGameplayTag RarityTag;
	float WeaponFireTime = 0.f;
	
	bool bIsDragged = false;
};
