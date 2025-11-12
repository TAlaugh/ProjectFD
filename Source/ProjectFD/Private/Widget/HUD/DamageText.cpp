// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/HUD/DamageText.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"

void UDamageText::SetDamageText(float Damage)
{
	if (DamageText && Damage <= 0.f)
	{
		DamageText->SetVisibility(ESlateVisibility::Hidden);
		ImmuneText->SetVisibility(ESlateVisibility::Visible);
	}
	if (DamageText && Damage > 0.f)
	{
		DamageText->SetVisibility(ESlateVisibility::Visible);
		DamageText->SetText(FText::AsNumber(FMath::RoundToInt(Damage)));
		ImmuneText->SetVisibility(ESlateVisibility::Hidden);
	}
}
