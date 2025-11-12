// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Inventory/Slot/Module/ModuleSlotWidget.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "DataAssets/Widget/InventorySlotTextures.h"
#include "Equipment/ModuleDefinition.h"
#include "Inventory/InventoryComponent.h"
#include "Inventory/ItemTypes.h"
#include "Kismet/GameplayStatics.h"


void UModuleSlotWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void UModuleSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

FReply UModuleSlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	// RemoveHoveringWidget();
	
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		return UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton).NativeReply;
	}
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void UModuleSlotWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent,
                                             UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	// RemoveHoveringWidget();

	UDragDropOperation* DragOp = UWidgetBlueprintLibrary::CreateDragDropOperation(UDragDropOperation::StaticClass());
    
	UModuleSlotWidget* DragVisual = CreateWidget<UModuleSlotWidget>(GetWorld(), GetClass());
	DragVisual->SetSlotInfoBySlot(this);

	DragOp->DefaultDragVisual = DragVisual;
	DragOp->Payload = this;

	OutOperation = DragOp;
}

void UModuleSlotWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

	if (CachedInventoryEntry.ItemTag.IsValid())
	{
		UGameplayStatics::PlaySound2D(GetWorld(), HoverSFX, 1.5f, 1.0f);
	}
}

void UModuleSlotWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
}

void UModuleSlotWidget::SetSlotInfo(const FFDInventoryEntry& Entry, UInventorySlotTextures* TextureMap,
                                    FMasterItemDefinition ItemDefinition)
{
	if (Entry.ItemTag.IsValid())
	{
		ReplicateEntry(Entry);

		const UModuleDefinition* ModuleCDO = GetDefault<UModuleDefinition>(ItemDefinition.ModuleProps.ModuleClass.Get());
	
		SetModuleClassImage(ItemDefinition.SmallIcon);
		SetModuleIconImage(ItemDefinition.Icon);
		SetModuleSlotTierImage(ItemDefinition.RarityTag);
		SetEnchantCount(ItemDefinition.ItemAcquisitionInfo.MaxLevel, CachedInventoryEntry.Level);
		SetStackText(CachedInventoryEntry.Quantity);
		SetModuleNameText(CachedInventoryEntry.ItemName);
		SetModuleDecoImage(ModuleCDO->ChangeTypeTag);
		SetSocketTypeImage(*TextureMap->ModuleSlot.Find(ModuleCDO->SocketTypeTag));
		int TempCapacity = ModuleCDO->Capacity;
		if (SkillModuleDecoImage->GetVisibility() == ESlateVisibility::Visible)
		{
			TempCapacity -= CachedInventoryEntry.Level;
		}
		else if (ToolModuleDecoImage->GetVisibility() != ESlateVisibility::Visible)
		{
			TempCapacity += CachedInventoryEntry.Level;
		}
		SetCapacityText(TempCapacity);
	}
	else
	{
		// CachedInventoryEntry = FFDInventoryEntry();
	}
}

void UModuleSlotWidget::SetSlotInfo(const FFDEquipmentEntry& Entry, UInventorySlotTextures* TextureMap,
	FMasterItemDefinition ItemDefinition, EModuleCapacity CapacityInfo)
{
	if (Entry.EntryTag.IsValid())
	{
		ReplicateEntry(Entry);

		const UModuleDefinition* ModuleCDO = GetDefault<UModuleDefinition>(ItemDefinition.ModuleProps.ModuleClass.Get());
	
		SetModuleClassImage(ItemDefinition.SmallIcon);
		SetModuleIconImage(ItemDefinition.Icon);
		SetModuleSlotTierImage(ItemDefinition.RarityTag);
		SetEnchantCount(ItemDefinition.ItemAcquisitionInfo.MaxLevel, CachedInventoryEntry.Level);
		// equipped slot doesn't need stack text
		SetStackText(1);
		SetModuleNameText(CachedInventoryEntry.ItemName);
		SetModuleDecoImage(ModuleCDO->ChangeTypeTag);
		SetSocketTypeImage(*TextureMap->ModuleSlot.Find(ModuleCDO->SocketTypeTag));
		int TempCapacity = ModuleCDO->Capacity;
		if (SkillModuleDecoImage->GetVisibility() == ESlateVisibility::Visible)
		{
			TempCapacity -= CachedInventoryEntry.Level;
		}
		else if (ToolModuleDecoImage->GetVisibility() != ESlateVisibility::Visible)
		{
			TempCapacity += CachedInventoryEntry.Level;
		}
		SetCapacityText(TempCapacity, CapacityInfo);
	}
}

void UModuleSlotWidget::SetSlotInfoBySlot(UModuleSlotWidget* ModuleSlot)
{
	ReplicateEntry(ModuleSlot->GetCachedInventoryEntry());

	SetImageWithImage(ModuleClassImage, ModuleSlot->ModuleClassImage);
	SetImageWithImage(ModuleIconImage, ModuleSlot->ModuleIconImage);
	SetImageWithImage(ModuleSlotTierImage, ModuleSlot->ModuleSlotTierImage);
	SetEnchantCount(EnchantStackContainer->GetChildrenCount(), CachedInventoryEntry.Level);
	SetStackText(1);
	SetModuleNameText(CachedInventoryEntry.ItemName);
	SetImageWithImage(SkillModuleDecoImage, ModuleSlot->SkillModuleDecoImage);
	SetImageWithImage(ToolModuleDecoImage, ModuleSlot->ToolModuleDecoImage);
	SetImageWithImage(ModuleSocketTypeImage, ModuleSlot->ModuleSocketTypeImage);
	int TempCapacity = FCString::Atoi(*ModuleSlot->ModuleCapacityText->GetText().ToString());
	// if (SkillModuleDecoImage->GetVisibility() == ESlateVisibility::Visible)
	// {
	// 	TempCapacity -= CachedInventoryEntry.Level;
	// }
	// else if (ToolModuleDecoImage->GetVisibility() != ESlateVisibility::Visible)
	// {
	// 	TempCapacity += CachedInventoryEntry.Level;
	// }
	SetCapacityText(TempCapacity);
}

FFDInventoryEntry UModuleSlotWidget::GetCachedInventoryEntry()
{
	return CachedInventoryEntry;
}

int64 UModuleSlotWidget::GetItemID()
{
	if (CachedInventoryEntry.IsValid())
	{
		return CachedInventoryEntry.ItemID;
	}

	return 0;
}

void UModuleSlotWidget::SetImageWithImage(UImage* TargetImage, UImage* SourceImage)
{
	if (SourceImage && TargetImage)
	{
		if (SourceImage->GetVisibility() == ESlateVisibility::Visible)
		{
			TargetImage->SetVisibility(ESlateVisibility::Visible);
		}
		FSlateBrush Brush = SourceImage->GetBrush();
		TargetImage->SetBrush(Brush);

		// if (UTexture2D* Texture = Cast<UTexture2D>(SourceImage->Brush.GetResourceObject()))
		// {
		// 	TargetImage->SetBrushFromTexture(Texture, true); // bMatchSize = true
		// }
	}
}

FText UModuleSlotWidget::GetCapacityText()
{
	return ModuleCapacityText->GetText();
}

void UModuleSlotWidget::SetModuleClassImage(UTexture2D* InTexture)
{
	ModuleClassImage->SetBrushFromTexture(InTexture);
}

void UModuleSlotWidget::SetModuleIconImage(UTexture2D* InTexture)
{
	ModuleIconImage->SetBrushFromTexture(InTexture);
}

void UModuleSlotWidget::SetModuleSlotTierImage(const FGameplayTag& TierTag)
{
	if (TierTag.IsValid())
	{
		ModuleSlotTierImage->SetVisibility(ESlateVisibility::Visible);
		if (TierTag.MatchesTagExact(FGameplayTag::RequestGameplayTag("UI.Equipment.Tier.Transcendent")))
		{
			ModuleSlotTierImage->SetBrushTintColor(FSlateColor(FLinearColor(1.f,0.1f,0.1f,0.9f)));
		}
		else if (TierTag.MatchesTagExact(FGameplayTag::RequestGameplayTag("UI.Equipment.Tier.Ultimate")))
		{
			ModuleSlotTierImage->SetBrushTintColor(FSlateColor(FLinearColor(1.f,1.f,0.f,0.9f)));
		}
		else if (TierTag.MatchesTagExact(FGameplayTag::RequestGameplayTag("UI.Equipment.Tier.Rare")))
		{
			ModuleSlotTierImage->SetBrushTintColor(FSlateColor(FLinearColor(1.f,0.f,1.f,0.9f)));
		}
		else if (TierTag.MatchesTagExact(FGameplayTag::RequestGameplayTag("UI.Equipment.Tier.Normal")))
		{
			ModuleSlotTierImage->SetBrushTintColor(FSlateColor(FLinearColor(0.f,1.f,1.f,0.9f)));
		}
	}
}

void UModuleSlotWidget::SetEnchantCount(int32 MaxCount, int32 Level)
{
	EnchantStackContainer->ClearChildren();
	
	for (int i = 0; i < MaxCount; i++)
	{
		UImage* NewEnchantStackImage = NewObject<UImage>();
		NewEnchantStackImage->SetBrushFromTexture(EnchantStackImage);
		UVerticalBoxSlot* NewSlot = EnchantStackContainer->AddChildToVerticalBox(NewEnchantStackImage);

		NewSlot->SetHorizontalAlignment(HAlign_Fill);
		NewSlot->SetVerticalAlignment(VAlign_Bottom);

		FSlateChildSize ChildSize;
		ChildSize.SizeRule = ESlateSizeRule::Fill; // 또는 Auto
		ChildSize.Value = 1.0f;
		
		NewSlot->SetSize(ChildSize);
		if (i >= MaxCount - Level)
		{
			// NewEnchantStackImage->SetBrushTintColor(FSlateColor(FLinearColor(1.f,1.f,0.f,1.f)));
			NewEnchantStackImage->SetColorAndOpacity(FLinearColor(1.f,1.f,0.f,0.9f));
		}
	}
}

void UModuleSlotWidget::SetStackText(int32 Quantity)
{
	if (Quantity == 1)
	{
		StackCountBox->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		StackCountText->SetText(FText::AsNumber(Quantity));
		StackCountBox->SetVisibility(ESlateVisibility::Visible);
	}
}

void UModuleSlotWidget::SetModuleDecoImage(const FGameplayTag& ChangeTypeTag)
{
	if (ChangeTypeTag.MatchesTagExact(FGameplayTag::RequestGameplayTag("Module.ChangeType.Skill")))
	{
		SkillModuleDecoImage->SetVisibility(ESlateVisibility::Visible);
		ToolModuleDecoImage->SetVisibility(ESlateVisibility::Hidden);
	}
	else if (ChangeTypeTag.MatchesTagExact(FGameplayTag::RequestGameplayTag("Module.ChangeType.Tool")))
	{
		SkillModuleDecoImage->SetVisibility(ESlateVisibility::Hidden);
		ToolModuleDecoImage->SetVisibility(ESlateVisibility::Visible);
	}
	else // Common
	{
		SkillModuleDecoImage->SetVisibility(ESlateVisibility::Hidden);
		ToolModuleDecoImage->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UModuleSlotWidget::SetModuleNameText(FText NameText)
{
	ModuleNameText->SetText(NameText);
}

void UModuleSlotWidget::SetSocketTypeImage(UTexture2D* InTexture)
{
	 ModuleSocketTypeImage->SetBrushFromTexture(InTexture);
}

void UModuleSlotWidget::SetCapacityText(int32 Capacity, EModuleCapacity CapacityInfo)
{
	if (CapacityInfo == EModuleCapacity::None)
	{
		ModuleCapacityText->SetText(FText::AsNumber(Capacity));
	}
	else if (CapacityInfo == EModuleCapacity::Half)
	{
		ModuleCapacityText->SetText(FText::AsNumber((int32)((Capacity+Capacity%2)/2)));
		ModuleCapacityText->SetColorAndOpacity(FSlateColor(FLinearColor(0.f,1.f,0.f,0.8f)));
	}
	else
	{
		ModuleCapacityText->SetText(FText::AsNumber(Capacity*2));
		ModuleCapacityText->SetColorAndOpacity(FSlateColor(FLinearColor(1.f,0.f,0.f,0.8f)));
	}
}

void UModuleSlotWidget::ReplicateEntry(const FFDInventoryEntry& InEntry)
{
	CachedInventoryEntry.ItemTag = InEntry.ItemTag;
	CachedInventoryEntry.ItemID = InEntry.ItemID;
	CachedInventoryEntry.ItemName = InEntry.ItemName;
	CachedInventoryEntry.Level = InEntry.Level;
	CachedInventoryEntry.Quantity = InEntry.Quantity;
	CachedInventoryEntry.EffectPackage = InEntry.EffectPackage;
}

void UModuleSlotWidget::ReplicateEntry(const FFDEquipmentEntry& InEntry)
{
	CachedInventoryEntry.ItemTag = InEntry.EntryTag;
	CachedInventoryEntry.ItemID = InEntry.ItemID;
	CachedInventoryEntry.ItemName = InEntry.ItemName;
	CachedInventoryEntry.Level = InEntry.EquipmentLevel;
	CachedInventoryEntry.EffectPackage = InEntry.EffectPackage;
}
