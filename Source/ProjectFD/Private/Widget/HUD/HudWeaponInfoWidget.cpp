// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/HUD/HudWeaponInfoWidget.h"

#include "GameplayTagContainer.h"
#include "Components/HorizontalBox.h"
#include "Components/Image.h"
#include "Components/Spacer.h"
#include "Components/TextBlock.h"
#include "DataAssets/Widget/InventorySlotTextures.h"
#include "Equipment/EquipmentManagerComponent.h"


void UHudWeaponInfoWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	SetUnEquipped();

	LeftAmmoChangeDelegate.BindUObject(this, &UHudWeaponInfoWidget::SetLeftAmmoText);
}

void UHudWeaponInfoWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UHudWeaponInfoWidget::NativeDestruct()
{
	Super::NativeDestruct();

	LeftAmmoChangeDelegate.Unbind();
}

void UHudWeaponInfoWidget::SetSlotInfo(const FFDEquipmentEntry& InEntry, float LeftAmmo)
{
	SetEquipped();
	SetWeaponInfoContainer(InEntry.bIsActiveWeapon);
	SetStatusInfoText(InEntry.bIsActiveWeapon);
	SetChargedAmmoText(InEntry.LoadedAmmo);
	SetLeftAmmoText(LeftAmmo);
	SetWeaponTypeImage(InEntry.EntryTag);
	SetAmmoTypeImage(InEntry.AmmoType);
	SetActiveWeaponSpacer(InEntry.bIsActiveWeapon);
}

void UHudWeaponInfoWidget::SetEquipped()
{
	WeaponTypeImage->SetVisibility(ESlateVisibility::Visible);
	AmmoTypeImage->SetVisibility(ESlateVisibility::Visible);
	StatusInfoText->SetVisibility(ESlateVisibility::Visible);
}

void UHudWeaponInfoWidget::SetUnEquipped()
{
	SetWeaponInfoContainer(false);
	StatusInfoText->SetVisibility(ESlateVisibility::Hidden);
	ChargedAmmoText->SetVisibility(ESlateVisibility::Hidden);
	LeftAmmoText->SetVisibility(ESlateVisibility::Hidden);
	WeaponTypeImage->SetVisibility(ESlateVisibility::Hidden);
	AmmoTypeImage->SetVisibility(ESlateVisibility::Hidden);
	StatusInfoText->SetVisibility(ESlateVisibility::Hidden);
}

void UHudWeaponInfoWidget::InitializeSelectGageInfo(int SlotNum)
{
	WeaponSelectGageImage1->SetColorAndOpacity(FLinearColor(0.2f, 0.2f, 0.2f, 0.9));
	WeaponSelectGageImage2->SetColorAndOpacity(FLinearColor(0.2f, 0.2f, 0.2f, 0.9));
	WeaponSelectGageImage3->SetColorAndOpacity(FLinearColor(0.2f, 0.2f, 0.2f, 0.9));

	switch (SlotNum)
	{
	case 0:
		WeaponSelectGageImage1->SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, 0.9));
		break;
	case 1:
		WeaponSelectGageImage2->SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, 0.9));
		break;
	case 2:
		WeaponSelectGageImage3->SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, 0.9));
		break;
	}
}

void UHudWeaponInfoWidget::SetWeaponInfoContainer(bool bIsActive)
{
	if (bIsActive)
	{
		HudWeaponInfoContainer->SetRenderOpacity(1.f);
	}
	else
	{
		HudWeaponInfoContainer->SetRenderOpacity(0.5f);
	}
}

void UHudWeaponInfoWidget::SetStatusInfoText(bool bIsActive)
{
	if (bIsActive)
	{
		StatusInfoText->SetVisibility(ESlateVisibility::Visible);
		ChargedAmmoText->SetVisibility(ESlateVisibility::Collapsed);
		LeftAmmoText->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		StatusInfoText->SetVisibility(ESlateVisibility::Collapsed);
		ChargedAmmoText->SetVisibility(ESlateVisibility::Visible);
		LeftAmmoText->SetVisibility(ESlateVisibility::Visible);
	}
}

void UHudWeaponInfoWidget::SetChargedAmmoText(float ChargedAmmo)
{
	FNumberFormattingOptions FormattingOptions;
	FormattingOptions.MaximumFractionalDigits = 0;
	FormattingOptions.MinimumFractionalDigits = 0;
	ChargedAmmoText->SetText(FText::AsNumber(ChargedAmmo, &FormattingOptions));
}

void UHudWeaponInfoWidget::SetLeftAmmoText(float LeftAmmo)
{
	FNumberFormattingOptions FormattingOptions;
	FormattingOptions.MaximumFractionalDigits = 0;
	FormattingOptions.MinimumFractionalDigits = 0;
	LeftAmmoText->SetText(FText::AsNumber(LeftAmmo, &FormattingOptions));
}

void UHudWeaponInfoWidget::SetWeaponTypeImage(FGameplayTag ItemTag)
{
	UTexture2D* NewImage = nullptr;
	if (ItemTag.MatchesTag(FGameplayTag::RequestGameplayTag("Item.Equipment.Weapon.MG")))
	{
		NewImage = *HudWeaponInfoTextures->HudWeaponInfo.Find(FGameplayTag::RequestGameplayTag("Item.Equipment.Weapon.MG"));
	}
	else if (ItemTag.MatchesTag(FGameplayTag::RequestGameplayTag("Item.Equipment.Weapon.LNC")))
	{
		NewImage = *HudWeaponInfoTextures->HudWeaponInfo.Find(FGameplayTag::RequestGameplayTag("Item.Equipment.Weapon.LNC"));
	}
	else if (ItemTag.MatchesTag(FGameplayTag::RequestGameplayTag("Item.Equipment.Weapon.ALT")))
	{
		NewImage = *HudWeaponInfoTextures->HudWeaponInfo.Find(FGameplayTag::RequestGameplayTag("Item.Equipment.Weapon.ALT"));
	}
	else if (ItemTag.MatchesTag(FGameplayTag::RequestGameplayTag("Item.Equipment.Weapon.AR")))
	{
		NewImage = *HudWeaponInfoTextures->HudWeaponInfo.Find(FGameplayTag::RequestGameplayTag("Item.Equipment.Weapon.AR"));
	}
	else if (ItemTag.MatchesTag(FGameplayTag::RequestGameplayTag("Item.Equipment.Weapon.BR")))
	{
		NewImage = *HudWeaponInfoTextures->HudWeaponInfo.Find(FGameplayTag::RequestGameplayTag("Item.Equipment.Weapon.BR"));
	}
	else if (ItemTag.MatchesTag(FGameplayTag::RequestGameplayTag("Item.Equipment.Weapon.HC")))
	{
		NewImage = *HudWeaponInfoTextures->HudWeaponInfo.Find(FGameplayTag::RequestGameplayTag("Item.Equipment.Weapon.HC"));
	}
	else if (ItemTag.MatchesTag(FGameplayTag::RequestGameplayTag("Item.Equipment.Weapon.HG")))
	{
		NewImage = *HudWeaponInfoTextures->HudWeaponInfo.Find(FGameplayTag::RequestGameplayTag("Item.Equipment.Weapon.HG"));
	}
	else if (ItemTag.MatchesTag(FGameplayTag::RequestGameplayTag("Item.Equipment.Weapon.SCR")))
	{
		NewImage = *HudWeaponInfoTextures->HudWeaponInfo.Find(FGameplayTag::RequestGameplayTag("Item.Equipment.Weapon.SCR"));
	}
	else if (ItemTag.MatchesTag(FGameplayTag::RequestGameplayTag("Item.Equipment.Weapon.SG")))
	{
		NewImage = *HudWeaponInfoTextures->HudWeaponInfo.Find(FGameplayTag::RequestGameplayTag("Item.Equipment.Weapon.SG"));
	}
	else if (ItemTag.MatchesTag(FGameplayTag::RequestGameplayTag("Item.Equipment.Weapon.SMG")))
	{
		NewImage = *HudWeaponInfoTextures->HudWeaponInfo.Find(FGameplayTag::RequestGameplayTag("Item.Equipment.Weapon.SMG"));
	}
	else if (ItemTag.MatchesTag(FGameplayTag::RequestGameplayTag("Item.Equipment.Weapon.SR")))
	{
		NewImage = *HudWeaponInfoTextures->HudWeaponInfo.Find(FGameplayTag::RequestGameplayTag("Item.Equipment.Weapon.SR"));
	}
	else if (ItemTag.MatchesTag(FGameplayTag::RequestGameplayTag("Item.Equipment.Weapon.TR")))
	{
		NewImage = *HudWeaponInfoTextures->HudWeaponInfo.Find(FGameplayTag::RequestGameplayTag("Item.Equipment.Weapon.TR"));
	}

	WeaponTypeImage->SetBrushFromTexture(NewImage);
}

void UHudWeaponInfoWidget::SetAmmoTypeImage(FGameplayTag AmmoType)
{
	if (UTexture2D* NewTexture = *HudWeaponInfoTextures->HudWeaponInfo.Find(AmmoType))
	{
		AmmoTypeImage->SetBrushFromTexture(NewTexture);
	}
}

void UHudWeaponInfoWidget::SetActiveWeaponSpacer(bool bIsActive)
{
	if (bIsActive)
	{
		ActiveWeaponSpacer->SetSize(FVector2D(20.f, 1.f));
	}
	else
	{
		ActiveWeaponSpacer->SetSize(FVector2D(1.f, 1.f));
	}
}
