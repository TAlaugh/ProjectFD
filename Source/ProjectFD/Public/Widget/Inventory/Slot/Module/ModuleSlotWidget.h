// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inventory/InventoryComponent.h"
#include "ModuleSlotWidget.generated.h"

struct FGameplayTag;
struct FFDInventoryEntry;
class UInventorySlotTextures;
struct FMasterItemDefinition;
class UTextBlock;
class UImage;
/**
 * 
 */
UCLASS()
class PROJECTFD_API UModuleSlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;

	virtual void NativeConstruct() override;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	// virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

	void SetSlotInfo(const FFDInventoryEntry& Entry, UInventorySlotTextures* TextureMap, FMasterItemDefinition ItemDefinition);
	void SetSlotInfo(const FFDEquipmentEntry& Entry, UInventorySlotTextures* TextureMap, FMasterItemDefinition ItemDefinition, EModuleCapacity CapacityInfo);
	void SetSlotInfoBySlot(UModuleSlotWidget* ModuleSlot);

	FFDInventoryEntry GetCachedInventoryEntry();
	int64 GetItemID();
	FText GetCapacityText();

	void SetImageWithImage(UImage* TargetImage, UImage* SourceImage);

	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Asset")
	UTexture2D* EnchantStackImage;

protected:
	// Doesn't need??
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="ModuleSlot", meta = (BindWidget, AllowPrivateAccess=true))
	UImage* ModuleSlotImage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="ModuleSlot", meta = (BindWidget, AllowPrivateAccess=true))
	UImage* ModuleClassImage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="ModuleSlot", meta = (BindWidget, AllowPrivateAccess=true))
	UImage* ModuleIconImage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="ModuleSlot", meta = (BindWidget, AllowPrivateAccess=true))
	UImage* ModuleSlotTierImage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="ModuleSlot", meta = (BindWidget, AllowPrivateAccess=true))
	class UVerticalBox* EnchantStackContainer;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="ModuleSlot", meta = (BindWidget, AllowPrivateAccess=true))
	class USizeBox* StackCountBox;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="ModuleSlot", meta = (BindWidget, AllowPrivateAccess=true))
	UTextBlock* StackCountText;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="ModuleSlot", meta = (BindWidget, AllowPrivateAccess=true))
	UImage* SkillModuleDecoImage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="ModuleSlot", meta = (BindWidget, AllowPrivateAccess=true))
	UImage* ToolModuleDecoImage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="ModuleSlot", meta = (BindWidget, AllowPrivateAccess=true))
	UTextBlock* ModuleNameText;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="ModuleSlot", meta = (BindWidget, AllowPrivateAccess=true))
	UTextBlock* ModuleRollText;

	// Doesn't need?
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="ModuleSlot", meta = (BindWidget, AllowPrivateAccess=true))
	UImage* ModuleMiddleDecoImage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="ModuleSlot", meta = (BindWidget, AllowPrivateAccess=true))
	UImage* ModuleSocketTypeImage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="ModuleSlot", meta = (BindWidget, AllowPrivateAccess=true))
	UTextBlock* ModuleCapacityText;

	void SetModuleClassImage(UTexture2D* InTexture);
	void SetModuleIconImage(UTexture2D* InTexture);
	void SetModuleSlotTierImage(const FGameplayTag& TierTag);
	void SetEnchantCount(int32 MaxCount, int32 Level);
	void SetStackText(int32 Quantity);
	void SetModuleDecoImage(const FGameplayTag& ChangeTypeTag);
	void SetModuleNameText(FText NameText);
	// void SetRollText(const FGameplayTag& RollTag);
	void SetSocketTypeImage(UTexture2D* InTexture);
	void SetCapacityText(int32 Capacity, EModuleCapacity CapacityInfo = EModuleCapacity::None);

private:

	void ReplicateEntry(const FFDInventoryEntry& InEntry);
	void ReplicateEntry(const FFDEquipmentEntry& InEntry);

	UPROPERTY()
	FFDInventoryEntry CachedInventoryEntry = FFDInventoryEntry();
	
	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Asset|Sound")
	TObjectPtr<USoundBase> HoverSFX;
};
