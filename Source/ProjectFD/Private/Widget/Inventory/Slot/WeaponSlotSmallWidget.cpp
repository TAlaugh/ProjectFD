// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Inventory/Slot/WeaponSlotSmallWidget.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Widget/Inventory/Slot/WeaponHoveringWidget.h"
#include "DataAssets/Widget/InventorySlotTextures.h"
#include "Inventory/ItemTypes.h"
#include "Kismet/GameplayStatics.h"


void UWeaponSlotSmallWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void UWeaponSlotSmallWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	// WeaponButton->OnClicked.AddDynamic(this, &UWeaponSlotSmallWidget::OnWeaponSlotClicked);
}

void UWeaponSlotSmallWidget::NativeDestruct()
{
	Super::NativeDestruct();

	RemoveHoveringWidget();
}

FReply UWeaponSlotSmallWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	RemoveHoveringWidget();
	
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		return UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton).NativeReply;
	}
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

FReply UWeaponSlotSmallWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (!bIsDragged)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), EquipSFX, 0.9f, 1.0f);
		OnWeaponSlotClicked();
		return FReply::Handled();
	}

	bIsDragged = false;
	
	return Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
}

void UWeaponSlotSmallWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

	if (InventoryEntry.ItemTag.IsValid())
	{
		UGameplayStatics::PlaySound2D(GetWorld(), HoverSFX, 1.5f, 1.0f);
		WeaponHoveringWidget = CreateWidget<UWeaponHoveringWidget>(this, WeaponHoveringWidgetClass);
		WeaponHoveringWidget->SetSlotInfo(InventoryEntry, RarityTag, WeaponFireTime);
		WeaponHoveringWidget->SetPositionInViewport(InMouseEvent.GetScreenSpacePosition(), false);
		WeaponHoveringWidget->AddToViewport();
	}
}

void UWeaponSlotSmallWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);

	RemoveHoveringWidget();
}

void UWeaponSlotSmallWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent,
                                                  UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	bIsDragged = true;

	UDragDropOperation* DragOp = UWidgetBlueprintLibrary::CreateDragDropOperation(UDragDropOperation::StaticClass());
    
	DragOp->DefaultDragVisual = this;
	DragOp->Payload = this;

	OutOperation = DragOp;
}

void UWeaponSlotSmallWidget::SetSlotInfo(const FFDInventoryEntry& Entry, UInventorySlotTextures* InventorySlotTextures,
                                         FMasterItemDefinition ItemDefinition, float FireTime)
{
	if (!Entry.ItemTag.IsValid()) return;

	ReplicateInventoryEntry(Entry);
	RarityTag = ItemDefinition.RarityTag;
	WeaponFireTime = FireTime;
	
	SetTierImage(RarityTag);
	SetWeaponImage(ItemDefinition.SmallIcon);

	int Damage = 0;
	float FireRate = 0;
	float MultiplyDamage = 0;
	for (const FEquipmentStatEffectGroup& StatEffect : InventoryEntry.EffectPackage.StatEffects)
	{
		if (StatEffect.StatEffectTag.MatchesTagExact(FGameplayTag::RequestGameplayTag("StatEffect.Weapon.Damage")))
		{
			Damage = StatEffect.CurrentValue;
		}
		else if (StatEffect.StatEffectTag.MatchesTagExact(FGameplayTag::RequestGameplayTag("StatEffect.Weapon.FireRate")))
		{
			FireRate = StatEffect.CurrentValue;
		}
		else if (StatEffect.StatEffectTag.MatchesTagExact(FGameplayTag::RequestGameplayTag("StatEffect.Weapon.MultiplyDamage")))
		{
			MultiplyDamage = StatEffect.CurrentValue;
		}
	}

	if (MultiplyDamage > 0)
	{
		Damage *= MultiplyDamage;
	}

	if (WeaponFireTime > 0)
	{
		float DPS = Damage * FireRate / WeaponFireTime;
		SetDPSText(DPS);
	}
	
	SetLevelText(FText::AsNumber(Entry.Level));
}

void UWeaponSlotSmallWidget::SetTierImage(FGameplayTag TierTag)
{
	if (TierTag.MatchesTagExact(FGameplayTag::RequestGameplayTag("UI.Weapon.WeaponSlot.Tier.Ultimate")))
	{
		TierImage->SetBrushTintColor(FSlateColor(FLinearColor(1.f,1.f,0.f,0.7f)));
	}
	else if (TierTag.MatchesTagExact(FGameplayTag::RequestGameplayTag("UI.Weapon.WeaponSlot.Tier.Rare")))
	{
		TierImage->SetBrushTintColor(FSlateColor(FLinearColor(1.f,0.f,1.f,0.7f)));
	}
	else if (TierTag.MatchesTagExact(FGameplayTag::RequestGameplayTag("UI.Weapon.WeaponSlot.Tier.Normal")))
	{
		TierImage->SetBrushTintColor(FSlateColor(FLinearColor(0.f,1.f,1.f,0.7f)));
	}
}

void UWeaponSlotSmallWidget::SetWeaponImage(UTexture2D* WeaponTexture)
{
	WeaponImage->SetBrushFromTexture(WeaponTexture);
}

void UWeaponSlotSmallWidget::SetDPSText(float InDPS)
{
	FNumberFormattingOptions FormatOptions;
	FormatOptions.MaximumFractionalDigits = 0;
	FormatOptions.MinimumFractionalDigits = 0;
	DPSValueText->SetText(FText::AsNumber(InDPS, &FormatOptions));
}

void UWeaponSlotSmallWidget::SetLevelText(FText InLevelText)
{
	LevelText->SetText(InLevelText);
}

FFDInventoryEntry UWeaponSlotSmallWidget::GetInventoryEntry()
{
	return InventoryEntry;
}

void UWeaponSlotSmallWidget::OnWeaponSlotClicked()
{
	if (InventoryEntry.IsValid() && InventoryEntry.ItemTag.IsValid())
	{
		WeaponSmallSlotClickedDelegate.Broadcast(InventoryEntry);
	}
}

void UWeaponSlotSmallWidget::ReplicateInventoryEntry(const FFDInventoryEntry& InEntry)
{
	InventoryEntry.ItemTag = InEntry.ItemTag;
	InventoryEntry.ItemID = InEntry.ItemID;
	InventoryEntry.ItemName = InEntry.ItemName;
	InventoryEntry.Level = InEntry.Level;
	InventoryEntry.EffectPackage = InEntry.EffectPackage;
	InventoryEntry.Quantity = InEntry.Quantity;
}

void UWeaponSlotSmallWidget::RemoveHoveringWidget()
{
	if (WeaponHoveringWidget && WeaponHoveringWidget->IsInViewport())
	{
		WeaponHoveringWidget->RemoveFromParent();
		WeaponHoveringWidget = nullptr;
	}
}
