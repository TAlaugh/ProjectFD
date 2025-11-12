// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Inventory/Slot/FilterOptionWidget.h"

#include "Components/Button.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"


void UFilterOptionWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	SelectButton->OnClicked.AddDynamic(this, &UFilterOptionWidget::OnSelectButtonClicked);
}

void UFilterOptionWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UFilterOptionWidget::OnSelectButtonClicked()
{
	FilterOptionButtonDelegate.Broadcast(OptionNum);
}

void UFilterOptionWidget::SetSizeBoxSize(float Width, float Height)
{
	if (Width > 0)
	{
		FilterOptionSizeBox->SetWidthOverride(Width);
	}
	if (Height > 0)
	{
		FilterOptionSizeBox->SetHeightOverride(Height);
	}
}

void UFilterOptionWidget::SetOptionNameText(FText NameText)
{
	OptionNameText->SetText(NameText);
}

void UFilterOptionWidget::SetOptionNum(int InNum)
{
	OptionNum = InNum;
}
