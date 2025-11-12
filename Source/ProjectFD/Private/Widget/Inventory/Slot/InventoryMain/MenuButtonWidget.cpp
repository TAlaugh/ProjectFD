// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Inventory/Slot/InventoryMain/MenuButtonWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"


void UMenuButtonWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SizeBox = NewObject<USizeBox>(this, USizeBox::StaticClass());
	SizeBox->SetHeightOverride(70.f);

	Overlay = NewObject<UOverlay>(this, UOverlay::StaticClass());
	SizeBox->AddChild(Overlay);

	BackgroundImage = NewObject<UImage>(this, UImage::StaticClass());
	Overlay->AddChildToOverlay(BackgroundImage);

	MenuButton = NewObject<UButton>(this, UButton::StaticClass());
	Overlay->AddChildToOverlay(MenuButton);

	MenuName = NewObject<UTextBlock>(this, UTextBlock::StaticClass());
	MenuButton->AddChild(MenuName);
	MenuName->SetText(FText::FromString(TEXT("")));

	MenuButton->OnClicked.AddDynamic(this, &UMenuButtonWidget::OnMenuButtonClicked);
	
}

void UMenuButtonWidget::OnMenuButtonClicked()
{
}

void UMenuButtonWidget::SetBackgroundImage(UTexture2D* InTexture)
{
	BackgroundImage->SetBrushFromTexture(InTexture);
}

