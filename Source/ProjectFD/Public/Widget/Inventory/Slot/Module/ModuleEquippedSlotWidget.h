// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Equipment/EquipmentManagerComponent.h"
#include "ModuleEquippedSlotWidget.generated.h"

struct FMasterItemDefinition;
class UInventorySlotTextures;
struct FFDInventoryEntry;
class UModuleSlotWidget;
class UImage;

DECLARE_DELEGATE_TwoParams(FModuleEquipSignature, const FFDInventoryEntry&, int);
DECLARE_DELEGATE_OneParam(FModuleUnEquipSignature, const FFDEquipmentEntry&);
DECLARE_DELEGATE_TwoParams(FModuleOrderChangeSignature, const FFDEquipmentEntry&, int);

/**
 * 
 */
UCLASS()
class PROJECTFD_API UModuleEquippedSlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	virtual void NativeOnInitialized() override;

	virtual void NativeConstruct() override;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

	void InitSlotNum(int SlotNum);
	void SetSocketImage(FGameplayTag SocketTag, UInventorySlotTextures* TextureMap);
	void SetDecoImage(int SlotNum);
	
	void SetSlotInfo(const FFDEquipmentEntry& Entry, UInventorySlotTextures* TextureMap, FMasterItemDefinition ItemDefinition, EModuleCapacity CapacityInfo = EModuleCapacity::None);
	void SetDefaultSlotInfo();
	
	FModuleEquipSignature ModuleEquipDelegate;
	FModuleUnEquipSignature ModuleUnEquipDelegate;
	FModuleOrderChangeSignature ModuleOrderChangeDelegate;


protected:

	// Empty Slot Info
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="EmptyModuleSlot", meta = (BindWidget, AllowPrivateAccess=true))
	UImage* BigSkillModuleDecoImage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="EmptyModuleSlot", meta = (BindWidget, AllowPrivateAccess=true))
	UImage* BigToolModuleDecoImage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="EmptyModuleSlot", meta = (BindWidget, AllowPrivateAccess=true))
	UImage* SocketImage;

	// UPROPERTY()
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="EmptyModuleSlot|EquippedSlot", meta = (BindWidget, AllowPrivateAccess=true))
	UModuleSlotWidget* EquippedModuleSlot;

private:

	void ReplicateEntry(const FFDEquipmentEntry& Entry);

	UFUNCTION()
	void OnDragOperationSuccess(UDragDropOperation* Operation);

	UFUNCTION()
	void OnDragOperationCancelled(UDragDropOperation* Operation);

	void OnEquippedModuleRightClicked();

	UPROPERTY()
	FFDEquipmentEntry CachedEquipmentEntry = FFDEquipmentEntry();

	UPROPERTY()
	int ModuleSlotNum = -1;

	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Class", meta=(AllowPrivateAccess=true))
	TSubclassOf<UModuleSlotWidget> ModuleSlotClass;

	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Asset|Sound")
	TObjectPtr<USoundBase> UnEquipSFX;

	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Asset|Sound")
	TObjectPtr<USoundBase> HoverSFX;
	
};
