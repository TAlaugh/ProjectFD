// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Inventory/Slot/WeaponSlotWidget.h"

#include <ThirdParty/fmt/8.1.1/include/fmt/chrono.h>

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/HorizontalBox.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "DataAssets/Widget/InventorySlotTextures.h"
#include "Inventory/ItemTypes.h"
#include "Widget/Inventory/Slot/WeaponSlotSmallWidget.h"
#include "Widget/Inventory/Slot/WeaponHoveringWidget.h"
#include "Kismet/GameplayStatics.h"


void UWeaponSlotWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	SetUnEquipped();
}

void UWeaponSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UWeaponSlotWidget::NativeDestruct()
{
	Super::NativeDestruct();

	RemoveHoveringWidget();
}

FReply UWeaponSlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	RemoveHoveringWidget();
	
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		return UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton).NativeReply;
	}
	else if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton && CachedEquipmentEntry.EntryTag.IsValid())
	{
		return FReply::Handled();
	}
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

FReply UWeaponSlotWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (bIsDragged == false && InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		OnWeaponSlotClicked();
		
		return FReply::Handled();
	}
	else if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		OnWeaponSlotRightClicked();

		return FReply::Handled();
	}

	bIsDragged = false;
	
	return Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
}

void UWeaponSlotWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

	if (CachedEquipmentEntry.EntryTag.IsValid())
	{
		UGameplayStatics::PlaySound2D(GetWorld(), HoverSFX, 1.5f, 1.0f);
		WeaponHoveringWidget = CreateWidget<UWeaponHoveringWidget>(this, WeaponHoveringWidgetClass);
		WeaponHoveringWidget->SetSlotInfo(CachedEquipmentEntry);
		WeaponHoveringWidget->SetPositionInViewport(InMouseEvent.GetScreenSpacePosition(), false);
		WeaponHoveringWidget->AddToViewport();
	}
}

void UWeaponSlotWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);

	RemoveHoveringWidget();
}

void UWeaponSlotWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent,
                                             UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	bIsDragged = true;

	RemoveHoveringWidget();

	UDragDropOperation* DragOp = UWidgetBlueprintLibrary::CreateDragDropOperation(UDragDropOperation::StaticClass());

	UWeaponSlotWidget* DragVisual = CreateWidget<UWeaponSlotWidget>(GetWorld(), GetClass());
	DragVisual->CachedEquipmentEntry = CachedEquipmentEntry;
	DragVisual->SetSlotInfoBySlot(this);

	DragOp->DefaultDragVisual = DragVisual;
	DragOp->Payload = this;

	OutOperation = DragOp;


	SetUnEquipped();
	WeaponImage->SetVisibility(ESlateVisibility::Hidden);

	DragOp->OnDrop.AddDynamic(this, &UWeaponSlotWidget::OnDragOperationSuccess);
	// return true doesn't mean success... could fail cause of cost or slot location
	DragOp->OnDragCancelled.AddDynamic(this, &UWeaponSlotWidget::OnDragOperationCancelled);

	OutOperation = DragOp;
}

bool UWeaponSlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	if (UWeaponSlotSmallWidget* DropSmallWidget = Cast<UWeaponSlotSmallWidget>(InOperation->Payload))
	{
		UGameplayStatics::PlaySound2D(GetWorld(), EquipSFX, 0.9f, 1.0f);
		WeaponEquipDelegate.ExecuteIfBound(DropSmallWidget->GetInventoryEntry(), CachedEquipmentEntry.SlotNum);
		return true;
	}
	else if (UWeaponSlotWidget* DropWidget = Cast<UWeaponSlotWidget>(InOperation->Payload))
	{
		if (DropWidget->CachedEquipmentEntry.ItemID != this->CachedEquipmentEntry.ItemID)
		{
			WeaponOrderChangeDelegate.ExecuteIfBound(DropWidget->CachedEquipmentEntry, CachedEquipmentEntry.SlotNum);
			return true;
		}
	}
	return false;
}

void UWeaponSlotWidget::SetSlotInfo(const FFDEquipmentEntry& Entry, UInventorySlotTextures* InventorySlotTextures, FMasterItemDefinition ItemDefinition)
{
	SetEquipped();
	ReplicateEntry(Entry);

	if (!CachedEquipmentEntry.EntryTag.IsValid()) return;
	
	SetBackgroundImage(*InventorySlotTextures->WeaponSlot.Find(FGameplayTag::RequestGameplayTag("UI.Weapon.WeaponSlot.Background.Equipped")));
	SetTierImage(CachedEquipmentEntry.RarityTag);
	SetWeaponImage(ItemDefinition.Icon);
	int Damage = 0;
	float FireRate = 0;
	float MultiplyDamage = 0;
	for (const FEquipmentStatEffectGroup& StatEffect : CachedEquipmentEntry.EffectPackage.StatEffects)
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
	
	float TimePerShot = CachedEquipmentEntry.GetFireTime();
	if (TimePerShot > 0)
	{
		float DPS = Damage * FireRate / TimePerShot;
		SetDPSText(DPS);
	}
	else
	{
		float DPS = Damage * FireRate / 0.3f;
		SetDPSText(DPS);
	}
	SetLevelText(FText::AsNumber(CachedEquipmentEntry.EquipmentLevel));

	SetAmmoTypeImage(*InventorySlotTextures->WeaponSlot.Find(Entry.AmmoType));

	SetSelectedImage(CachedEquipmentEntry.bIsActiveWeapon);
}

void UWeaponSlotWidget::SetSlotInfoBySlot(UWeaponSlotWidget* WeaponSlotWidget)
{
	SetUnEquipped();
	if (WeaponSlotWidget->WeaponImage->GetVisibility() == ESlateVisibility::Visible)
	{
		WeaponImage->SetVisibility(ESlateVisibility::Visible);
	}
	FSlateBrush Brush = WeaponSlotWidget->WeaponImage->GetBrush();
	WeaponImage->SetBrush(Brush);
}

void UWeaponSlotWidget::SetSlotDefault()
{
	SetUnEquipped();
	SetWeaponImage(UnEquippedWeaponImage);
	SetTierImage(FGameplayTag());

	int CachedSlotNum = CachedEquipmentEntry.SlotNum;
	
	FFDEquipmentEntry EmptyEntry = FFDEquipmentEntry();
	ReplicateEntry(EmptyEntry);
	InitSlotNum(CachedSlotNum);
}

void UWeaponSlotWidget::SetEquipped()
{
	DPSHorizontalBox->SetVisibility(ESlateVisibility::Visible);
	LevelText->SetVisibility(ESlateVisibility::Visible);
	AmmoTypeImage->SetVisibility(ESlateVisibility::Visible);
}

void UWeaponSlotWidget::SetUnEquipped()
{
	DPSHorizontalBox->SetVisibility(ESlateVisibility::Hidden);
	LevelText->SetVisibility(ESlateVisibility::Hidden);
	AmmoTypeImage->SetVisibility(ESlateVisibility::Hidden);
	SetSelectedImage(false);
}

void UWeaponSlotWidget::SetBackgroundImage(UTexture2D* BGTexture)
{
	if (BGTexture)
	{
		BackgroundImage->SetBrushFromTexture(BGTexture);
	}
}

void UWeaponSlotWidget::SetTierImage(FGameplayTag Tag)
{
	// if (TierImage->GetVisibility() != ESlateVisibility::Visible)
	if (Tag.IsValid())
	{
		TierImage->SetVisibility(ESlateVisibility::Visible);
		if (Tag.MatchesTagExact(FGameplayTag::RequestGameplayTag("UI.Weapon.WeaponSlot.Tier.Ultimate")))
		{
			TierImage->SetBrushTintColor(FSlateColor(FLinearColor(1.f,1.f,0.f,1.f)));
		}
		else if (Tag.MatchesTagExact(FGameplayTag::RequestGameplayTag("UI.Weapon.WeaponSlot.Tier.Rare")))
		{
			TierImage->SetBrushTintColor(FSlateColor(FLinearColor(1.f,0.f,1.f,1.f)));
		}
		else if (Tag.MatchesTagExact(FGameplayTag::RequestGameplayTag("UI.Weapon.WeaponSlot.Tier.Normal")))
		{
			TierImage->SetBrushTintColor(FSlateColor(FLinearColor(0.f,1.f,1.f,1.f)));
		}
	}
	else
	{
		TierImage->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UWeaponSlotWidget::SetWeaponImage(UTexture2D* WeaponTexture)
{
	if (WeaponTexture)
	{
		WeaponImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		WeaponImage->SetBrushFromTexture(WeaponTexture);
	}
	else
	{
		WeaponImage->SetVisibility(ESlateVisibility::Hidden);
		WeaponImage->SetBrushFromTexture(WeaponTexture);
	}
}

void UWeaponSlotWidget::SetDPSText(float InDPS)
{
	FNumberFormattingOptions FormatOptions;
	FormatOptions.MaximumFractionalDigits = 0;
	FormatOptions.MinimumFractionalDigits = 0;
	DPSValueText->SetText(FText::AsNumber(InDPS, &FormatOptions));
}

void UWeaponSlotWidget::SetLevelText(FText InLevelText)
{
	LevelText->SetText(InLevelText);
}

void UWeaponSlotWidget::SetAmmoTypeImage(UTexture2D* AmmoTypeTexture)
{
	AmmoTypeImage->SetVisibility(ESlateVisibility::Visible);
	AmmoTypeImage->SetBrushFromTexture(AmmoTypeTexture);
}

void UWeaponSlotWidget::SetSelectedImage(bool bShow)
{
	if (bShow)
	{
		WeaponSelectedLeftImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		WeaponSelectedRightImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	else
	{
		WeaponSelectedLeftImage->SetVisibility(ESlateVisibility::Hidden);
		WeaponSelectedRightImage->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UWeaponSlotWidget::InitSlotNum(int SlotNum)
{
	CachedEquipmentEntry.SlotNum = SlotNum;
}

int64 UWeaponSlotWidget::GetItemID()
{
	return CachedEquipmentEntry.ItemID;
}

void UWeaponSlotWidget::OnWeaponSlotClicked()
{
	WeaponSlotDelegate.Broadcast(CachedEquipmentEntry);
}

void UWeaponSlotWidget::OnWeaponSlotRightClicked()
{
	WeaponSlotRightClickDelegate.Broadcast(CachedEquipmentEntry);
	UGameplayStatics::PlaySound2D(GetWorld(), UnEquipSFX, 0.9f, 1.0f);
}

void UWeaponSlotWidget::ReplicateEntry(const FFDEquipmentEntry& Entry)
{
	CachedEquipmentEntry.EntryTag = Entry.EntryTag;
	CachedEquipmentEntry.ItemID = Entry.ItemID;
	CachedEquipmentEntry.ItemName = Entry.ItemName;
	CachedEquipmentEntry.SlotTag = Entry.SlotTag;
	CachedEquipmentEntry.SlotNum = Entry.SlotNum;
	CachedEquipmentEntry.EquipmentLevel = Entry.EquipmentLevel;
	CachedEquipmentEntry.AmmoType = Entry.AmmoType;
	CachedEquipmentEntry.RarityTag = Entry.RarityTag;
	CachedEquipmentEntry.EffectPackage = Entry.EffectPackage;
	CachedEquipmentEntry.bIsActiveWeapon = Entry.bIsActiveWeapon;
	CachedEquipmentEntry.SetFireTime(Entry.FireTime);
}

void UWeaponSlotWidget::RemoveHoveringWidget()
{
	if (WeaponHoveringWidget && WeaponHoveringWidget->IsInViewport())
	{
		WeaponHoveringWidget->RemoveFromParent();
        WeaponHoveringWidget = nullptr;
	}
}

void UWeaponSlotWidget::OnDragOperationSuccess(UDragDropOperation* Operation)
{
	// CachedEquipmentEntry = FFDEquipmentEntry();
	UGameplayStatics::PlaySound2D(GetWorld(), EquipSFX, 0.9f, 1.0f);
}

void UWeaponSlotWidget::OnDragOperationCancelled(UDragDropOperation* Operation)
{
	SetEquipped();
	WeaponImage->SetVisibility(ESlateVisibility::Visible);
}
