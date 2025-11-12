// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/HUD/HudSkillInfoSlotWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"


void UHudSkillInfoSlotWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void UHudSkillInfoSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (SkillCoolDownMaterial)
	{
		DynMat = UMaterialInstanceDynamic::Create(SkillCoolDownMaterial, this);
	}

	FSlateBrush NewBrush = SkillCoolTimeImage->GetBrush();
	NewBrush.SetResourceObject(DynMat);
	SkillCoolTimeImage->SetBrush(NewBrush);
	
	SetManaLackImage(false);
	DynMat->SetScalarParameterValue(FName("FillPercent"), 0.f);
}

void UHudSkillInfoSlotWidget::NativeDestruct()
{
	Super::NativeDestruct();

	GetWorld()->GetTimerManager().ClearTimer(SkillTimerHandle);
	SkillTimerHandle.Invalidate();
}

void UHudSkillInfoSlotWidget::SetSlotInfo(FText KeyText, UTexture2D* IconTexture)
{
	SetSkillKeyText(KeyText);
	SetSkillIcon(IconTexture);
}

void UHudSkillInfoSlotWidget::SetSkillKeyText(FText InText)
{
	SkillKeyText->SetText(InText);
}

void UHudSkillInfoSlotWidget::SetSkillIcon(UTexture2D* InTexture)
{
	SkillIconImage->SetBrushFromTexture(InTexture);
}

void UHudSkillInfoSlotWidget::SetManaLackImage(bool bIsReady)
{
	if (bIsReady)
	{
		SkillManaLackImage->SetOpacity(0.f);
	}
	else
	{
		SkillManaLackImage->SetOpacity(0.4f);
	}

	if (SkillTimerHandle.IsValid())
	{
		SkillManaLackImage->SetBrushTintColor(FSlateColor(FLinearColor(1.f, 1.f, 1.f, 0.f)));
	}
	else
	{
		SkillManaLackImage->SetBrushTintColor(FSlateColor(FLinearColor(1.f, 1.f, 1.f, 1.f)));
	}
}

void UHudSkillInfoSlotWidget::SetSkillCoolTimeImage(float OutTimeRemaining, float OutTotalTime)
{
	RemainingTime = OutTimeRemaining;
	TotalTime = OutTotalTime;
	GetWorld()->GetTimerManager().SetTimer(SkillTimerHandle,[this] ()
		{
			if (RemainingTime <= 0 || TotalTime <= 0)
			{
				RemainingTime = 0.f;
				TotalTime = 0.f;
				DynMat->SetScalarParameterValue(FName("FillPercent"), 0.f);
				GetWorld()->GetTimerManager().ClearTimer(SkillTimerHandle);
				SkillTimerHandle.Invalidate();
				SetManaLackImage(false);
				return;
			}

			SetManaLackImage(true);
		
			DynMat->SetScalarParameterValue(FName("FillPercent"), RemainingTime/TotalTime);
			RemainingTime -= 0.1f;
		},
		0.1f,
		true);
}
