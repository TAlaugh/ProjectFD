// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Inventory/Slot/WeaponHoveringWidget.h"

#include "GameplayTagContainer.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Equipment/EquipmentManagerComponent.h"
#include "Inventory/InventoryComponent.h"
#include "Widget/Inventory/Slot/WeaponOptionSlotWidget.h"


void UWeaponHoveringWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void UWeaponHoveringWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UWeaponHoveringWidget::SetSlotInfo(FFDInventoryEntry& InEntry, FGameplayTag RarityTag, float FireTime)
{
	SetTierInfo(RarityTag);
	SetLevelText(InEntry.Level);
	SetWeaponNameText(InEntry.ItemName);

	int Damage = 0;
	int MaxAmmo = 0;
	float FireRate = 0;

	float MultiplyDamage = 0;
	float MultiplyAmmo = 0;
	for (const FEquipmentStatEffectGroup& StatEffect : InEntry.EffectPackage.StatEffects)
	{
		if (StatEffect.StatEffectTag.MatchesTagExact(FGameplayTag::RequestGameplayTag("StatEffect.Weapon.Damage")))
		{
			Damage = StatEffect.CurrentValue;
		}
		else if (StatEffect.StatEffectTag.MatchesTagExact(FGameplayTag::RequestGameplayTag("StatEffect.Weapon.MaxAmmo")))
		{
			MaxAmmo = StatEffect.CurrentValue;
		}
		else if (StatEffect.StatEffectTag.MatchesTagExact(FGameplayTag::RequestGameplayTag("StatEffect.Weapon.FireRate")))
		{
			FireRate = StatEffect.CurrentValue;
		}
		else if (StatEffect.StatEffectTag.MatchesTagExact(FGameplayTag::RequestGameplayTag("StatEffect.Weapon.MultiplyDamage")))
		{
			MultiplyDamage = StatEffect.CurrentValue;
		}
		else if (StatEffect.StatEffectTag.MatchesTagExact(FGameplayTag::RequestGameplayTag("StatEffect.Weapon.MultiplyAmmo")))
		{
			MultiplyAmmo = StatEffect.CurrentValue;
		}
	}

	if (MultiplyDamage > 0)
	{
		Damage *= MultiplyDamage;
	}

	if (MultiplyAmmo > 0)
	{
		MaxAmmo *= MultiplyAmmo;
	}
	
	float TimePerShot = FireTime;
	if (TimePerShot > 0)
	{
		float DPS = Damage * FireRate / TimePerShot;
		SetDPSText(DPS);
		SetRPMText(60 * FireRate / TimePerShot);
	}
	else
	{
		float DPS = Damage * FireRate / 0.3f;
		SetDPSText(DPS);
		SetRPMText(60 * FireRate);
	}
	
	// SetAmmoTypeInfo(FGameplayTag AmmoType);
	SetWeaponDamageText(Damage);
	SetAmmoText(MaxAmmo);

	WeaponOptionContainer->ClearChildren();
	for (FEquipmentStatEffectGroup& StatEffect : InEntry.EffectPackage.StatEffects)
	{
		if (StatEffect.StatEffectTag.MatchesTagExact(FGameplayTag::RequestGameplayTag("StatEffect.Weapon.Damage")) ||
			StatEffect.StatEffectTag.MatchesTagExact(FGameplayTag::RequestGameplayTag("StatEffect.Weapon.MaxAmmo")) ||
			StatEffect.StatEffectTag.MatchesTagExact(FGameplayTag::RequestGameplayTag("StatEffect.Weapon.FireRate")))
		{
			continue;
		}
		TObjectPtr<UWeaponOptionSlotWidget> NewOptionSlot = CreateWidget<UWeaponOptionSlotWidget>(this, WeaponOptionSlotClass);
		NewOptionSlot->SetWeaponOptionSlot(StatEffect.AttributeName, StatEffect.CurrentValue);
		WeaponOptionContainer->AddChildToVerticalBox(NewOptionSlot);
	}
}

void UWeaponHoveringWidget::SetSlotInfo(FFDEquipmentEntry& InEntry)
{
	SetTierInfo(InEntry.RarityTag);
	SetLevelText(InEntry.EquipmentLevel);
	SetWeaponNameText(InEntry.ItemName);

	int Damage = 0;
	int MaxAmmo = 0;
	float FireRate = 0;

	float MultiplyDamage = 0;
	float MultiplyAmmo = 0;
	for (const FEquipmentStatEffectGroup& StatEffect : InEntry.EffectPackage.StatEffects)
	{
		if (StatEffect.StatEffectTag.MatchesTagExact(FGameplayTag::RequestGameplayTag("StatEffect.Weapon.Damage")))
		{
			Damage = StatEffect.CurrentValue;
		}
		else if (StatEffect.StatEffectTag.MatchesTagExact(FGameplayTag::RequestGameplayTag("StatEffect.Weapon.MaxAmmo")))
		{
			MaxAmmo = StatEffect.CurrentValue;
		}
		else if (StatEffect.StatEffectTag.MatchesTagExact(FGameplayTag::RequestGameplayTag("StatEffect.Weapon.FireRate")))
		{
			FireRate = StatEffect.CurrentValue;
		}
		else if (StatEffect.StatEffectTag.MatchesTagExact(FGameplayTag::RequestGameplayTag("StatEffect.Weapon.MultiplyDamage")))
		{
			MultiplyDamage = StatEffect.CurrentValue;
		}
		else if (StatEffect.StatEffectTag.MatchesTagExact(FGameplayTag::RequestGameplayTag("StatEffect.Weapon.MultiplyAmmo")))
		{
			MultiplyAmmo = StatEffect.CurrentValue;
		}
	}

	if (MultiplyDamage > 0)
	{
		Damage *= MultiplyDamage;
	}

	if (MultiplyAmmo > 0)
	{
		MaxAmmo *= MultiplyAmmo;
	}
	
	float TimePerShot = InEntry.GetFireTime();
	if (TimePerShot > 0)
	{
		float DPS = Damage * FireRate / TimePerShot;
		SetDPSText(DPS);
		SetRPMText(60 * FireRate / TimePerShot);
	}
	else
	{
		SetRPMText(60 * FireRate);
	}
	
	SetAmmoTypeInfo(InEntry.AmmoType);
	SetWeaponDamageText(Damage);
	SetAmmoText(MaxAmmo);

	WeaponOptionContainer->ClearChildren();
	for (FEquipmentStatEffectGroup& StatEffect : InEntry.EffectPackage.StatEffects)
	{
		if (StatEffect.StatEffectTag.MatchesTagExact(FGameplayTag::RequestGameplayTag("StatEffect.Weapon.Damage")) ||
			StatEffect.StatEffectTag.MatchesTagExact(FGameplayTag::RequestGameplayTag("StatEffect.Weapon.MaxAmmo")) ||
			StatEffect.StatEffectTag.MatchesTagExact(FGameplayTag::RequestGameplayTag("StatEffect.Weapon.FireRate")))
		{
			continue;
		}
		TObjectPtr<UWeaponOptionSlotWidget> NewOptionSlot = CreateWidget<UWeaponOptionSlotWidget>(this, WeaponOptionSlotClass);
		NewOptionSlot->SetWeaponOptionSlot(StatEffect.AttributeName, StatEffect.CurrentValue);
		WeaponOptionContainer->AddChildToVerticalBox(NewOptionSlot);
	}
}

void UWeaponHoveringWidget::SetTierInfo(FGameplayTag TierTag)
{
	if (TierTag.IsValid())
	{
		TierImage->SetVisibility(ESlateVisibility::Visible);
		if (TierTag.MatchesTagExact(FGameplayTag::RequestGameplayTag("UI.Weapon.WeaponSlot.Tier.Ultimate")))
		{
			TierImage->SetBrushTintColor(FSlateColor(FLinearColor(1.f,1.f,0.f,0.7f)));
			WeaponTierText->SetText(FText::FromString(TEXT("궁극")));
		}
		else if (TierTag.MatchesTagExact(FGameplayTag::RequestGameplayTag("UI.Weapon.WeaponSlot.Tier.Rare")))
		{
			TierImage->SetBrushTintColor(FSlateColor(FLinearColor(1.f,0.f,1.f,0.7f)));
			WeaponTierText->SetText(FText::FromString(TEXT("희귀")));
		}
		else if (TierTag.MatchesTagExact(FGameplayTag::RequestGameplayTag("UI.Weapon.WeaponSlot.Tier.Normal")))
		{
			TierImage->SetBrushTintColor(FSlateColor(FLinearColor(0.f,1.f,1.f,0.7)));
			WeaponTierText->SetText(FText::FromString(TEXT("일반")));
		}
	}
}

void UWeaponHoveringWidget::SetLevelText(float InLevel)
{
	WeaponLevelText->SetText(FText::AsNumber(InLevel));
}

void UWeaponHoveringWidget::SetWeaponNameText(FText NameText)
{
	WeaponNameText->SetText(NameText);
}

void UWeaponHoveringWidget::SetDPSText(float InDPS)
{
	FNumberFormattingOptions FormatOptions;
	FormatOptions.MaximumFractionalDigits = 0;
	FormatOptions.MinimumFractionalDigits = 0;
	DPSText->SetText(FText::AsNumber(InDPS, &FormatOptions));
}

void UWeaponHoveringWidget::SetAmmoTypeInfo(FGameplayTag AmmoType)
{
	// Todo
}

void UWeaponHoveringWidget::SetWeaponDamageText(float InDamage)
{
	FNumberFormattingOptions FormatOptions;
	FormatOptions.MaximumFractionalDigits = 0;
	FormatOptions.MinimumFractionalDigits = 0;
	WeaponDamageText->SetText(FText::AsNumber(InDamage, &FormatOptions));
}

void UWeaponHoveringWidget::SetAmmoText(float InAmmo)
{
	FNumberFormattingOptions FormatOptions;
	FormatOptions.MaximumFractionalDigits = 0;
	FormatOptions.MinimumFractionalDigits = 0;
	AmmoText->SetText(FText::AsNumber(InAmmo, &FormatOptions));
}

void UWeaponHoveringWidget::SetRPMText(float InRPM)
{
	FNumberFormattingOptions FormatOptions;
	FormatOptions.MaximumFractionalDigits = 0;
	FormatOptions.MinimumFractionalDigits = 0;
	RoundPerMinText->SetText(FText::AsNumber(InRPM, &FormatOptions));
}

