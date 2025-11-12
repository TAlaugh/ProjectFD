// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Inventory/Slot/WeaponOptionSlotWidget.h"

#include "Components/TextBlock.h"


void UWeaponOptionSlotWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void UWeaponOptionSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UWeaponOptionSlotWidget::SetWeaponOptionSlot(FText OptionName, float OptionValue)
{
	WeaponOptionText->SetText(OptionName);
	
	FNumberFormattingOptions CurrentAmmoFormattingOptions;
	CurrentAmmoFormattingOptions.MaximumFractionalDigits = 1;
	CurrentAmmoFormattingOptions.MinimumFractionalDigits = 1;
	if (OptionValue >= 1)
	{
		OptionValue = (OptionValue - 1) * 100;
		FText FormatedText = FText::Format(
			NSLOCTEXT("UI", "PercentFormat", "{0}%[x]"),
			FText::AsNumber(OptionValue, &CurrentAmmoFormattingOptions)
		);
		
		OptionValueText->SetText(FormatedText);
	}
	else
	{
		OptionValue *= 100;
		FText FormatedText = FText::Format(
			NSLOCTEXT("UI", "PercentFormat", "{0}%[+]"),
			FText::AsNumber(OptionValue, &CurrentAmmoFormattingOptions)
		);
		
		OptionValueText->SetText(FormatedText);
	}
}
