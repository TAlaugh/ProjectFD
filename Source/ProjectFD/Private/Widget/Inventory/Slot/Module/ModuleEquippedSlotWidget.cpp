// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Inventory/Slot/Module/ModuleEquippedSlotWidget.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Image.h"
#include "DataAssets/Widget/InventorySlotTextures.h"
#include "Kismet/GameplayStatics.h"
#include "Widget/Inventory/Slot/Module/ModuleSlotWidget.h"


void UModuleEquippedSlotWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	// EquippedModuleSlot = CreateWidget<UModuleSlotWidget>(this, ModuleSlotClass);
}

void UModuleEquippedSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// EquippedModuleSlot->SetVisibility(ESlateVisibility::Collapsed);
}

FReply UModuleEquippedSlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent)
{
	// RemoveHoveringWidget();
	
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		if (CachedEquipmentEntry.EntryTag.IsValid())
		{
			return UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton).NativeReply;
		}
	}
	else if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton && CachedEquipmentEntry.EntryTag.IsValid())
	{
		return FReply::Handled();
	}
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

FReply UModuleEquippedSlotWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		OnEquippedModuleRightClicked();

		return FReply::Handled();
	}
	
	return Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
}

void UModuleEquippedSlotWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent,
                                                     UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	// RemoveHoveringWidget();

	UDragDropOperation* DragOp = UWidgetBlueprintLibrary::CreateDragDropOperation(UDragDropOperation::StaticClass());
    
	// DragOp->DefaultDragVisual = this;
	//
	// DragOp->Payload = this;

	UModuleSlotWidget* DragVisual = CreateWidget<UModuleSlotWidget>(GetWorld(), ModuleSlotClass);
	DragVisual->SetSlotInfoBySlot(EquippedModuleSlot);

	DragOp->DefaultDragVisual = DragVisual;
	DragOp->Payload = this;
	// DragOp->Pivot = EDragPivot::MouseDown;

	EquippedModuleSlot->SetVisibility(ESlateVisibility::Collapsed);

	DragOp->OnDrop.AddDynamic(this, &UModuleEquippedSlotWidget::OnDragOperationSuccess);
	// return true doesn't mean success... could fail cause of cost or slot location
	DragOp->OnDragCancelled.AddDynamic(this, &UModuleEquippedSlotWidget::OnDragOperationCancelled);

	OutOperation = DragOp;
}

bool UModuleEquippedSlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	if (UModuleSlotWidget* DropSmallWidget = Cast<UModuleSlotWidget>(InOperation->Payload))
	{
		// ModuleEquipDelegate.ExecuteIfBound(DropSmallWidget->GetCachedInventoryEntry(), CachedEquipmentEntry.SlotNum);
		ModuleEquipDelegate.ExecuteIfBound(DropSmallWidget->GetCachedInventoryEntry(), ModuleSlotNum);
		return true;
	}
	else if (UModuleEquippedSlotWidget* DropWidget = Cast<UModuleEquippedSlotWidget>(InOperation->Payload))
	{
		if (DropWidget->CachedEquipmentEntry.ItemID != this->CachedEquipmentEntry.ItemID)
		{
			// if (!this->CachedEquipmentEntry.EntryTag.IsValid())
			// {
			// 	// move location
			// 	return true;
			// }
			// ModuleOrderChangeDelegate.ExecuteIfBound(DropWidget->CachedEquipmentEntry, CachedEquipmentEntry.SlotNum);
			ModuleOrderChangeDelegate.ExecuteIfBound(DropWidget->CachedEquipmentEntry, ModuleSlotNum);
			return true;
		}
	}
	return false;
}

void UModuleEquippedSlotWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

	if (CachedEquipmentEntry.EntryTag.IsValid())
	{
		UGameplayStatics::PlaySound2D(GetWorld(), HoverSFX, 1.5f, 1.0f);
	}
}

void UModuleEquippedSlotWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
}

void UModuleEquippedSlotWidget::InitSlotNum(int SlotNum)
{
	ModuleSlotNum = SlotNum;
}

void UModuleEquippedSlotWidget::SetSocketImage(FGameplayTag SocketTag, UInventorySlotTextures* TextureMap)
{
	FString BigSocketTag = SocketTag.ToString() + TEXT(".Big");
	FGameplayTag NewTag = FGameplayTag::RequestGameplayTag(FName(*BigSocketTag));
	
	// if multiple?
	SocketImage->SetBrushFromTexture(*TextureMap->ModuleSlot.Find(NewTag));
}

void UModuleEquippedSlotWidget::SetDecoImage(int SlotNum)
{
	if (SlotNum == 0)
	{
		BigSkillModuleDecoImage->SetVisibility(ESlateVisibility::Visible);
		BigToolModuleDecoImage->SetVisibility(ESlateVisibility::Hidden);
	}
	else if (SlotNum == 1)
	{
		BigSkillModuleDecoImage->SetVisibility(ESlateVisibility::Hidden);
		BigToolModuleDecoImage->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		BigSkillModuleDecoImage->SetVisibility(ESlateVisibility::Hidden);
		BigToolModuleDecoImage->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UModuleEquippedSlotWidget::SetSlotInfo(const FFDEquipmentEntry& Entry, UInventorySlotTextures* TextureMap,
	FMasterItemDefinition ItemDefinition, EModuleCapacity CapacityInfo)
{
	ReplicateEntry(Entry);
	EquippedModuleSlot->SetSlotInfo(Entry, TextureMap, ItemDefinition, CapacityInfo);
	
	if (Entry.EntryTag.IsValid())
	{
		EquippedModuleSlot->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
	else
	{
		EquippedModuleSlot->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UModuleEquippedSlotWidget::SetDefaultSlotInfo()
{
	FFDEquipmentEntry Entry = FFDEquipmentEntry();
	ReplicateEntry(Entry);

	UGameplayStatics::PlaySound2D(GetWorld(), UnEquipSFX, 0.9f, 1.0f);
	
	EquippedModuleSlot->SetVisibility(ESlateVisibility::Collapsed);
}

void UModuleEquippedSlotWidget::ReplicateEntry(const FFDEquipmentEntry& Entry)
{
	CachedEquipmentEntry.SlotNum = Entry.SlotNum;
	CachedEquipmentEntry.EntryTag = Entry.EntryTag;
	CachedEquipmentEntry.ItemID = Entry.ItemID;
	CachedEquipmentEntry.ItemName = Entry.ItemName;
	CachedEquipmentEntry.EquipmentLevel = Entry.EquipmentLevel;
	CachedEquipmentEntry.ModuleCapacity = Entry.ModuleCapacity;
	CachedEquipmentEntry.RarityTag = Entry.RarityTag;
	CachedEquipmentEntry.SlotTag = Entry.SlotTag;
	CachedEquipmentEntry.EffectPackage = Entry.EffectPackage;
}

void UModuleEquippedSlotWidget::OnDragOperationSuccess(UDragDropOperation* Operation)
{
	CachedEquipmentEntry = FFDEquipmentEntry();
}

void UModuleEquippedSlotWidget::OnDragOperationCancelled(UDragDropOperation* Operation)
{
	EquippedModuleSlot->SetVisibility(ESlateVisibility::HitTestInvisible);
}

void UModuleEquippedSlotWidget::OnEquippedModuleRightClicked()
{
	ModuleUnEquipDelegate.ExecuteIfBound(CachedEquipmentEntry);
}
