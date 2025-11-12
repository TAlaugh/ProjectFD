// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/BaseWidget.h"


void UBaseWidget::SetWidgetController(UBaseWidgetController* InController)
{
	WidgetController = InController;
	OnWidgetControllerSet();
}

void UBaseWidget::OnWidgetControllerSet()
{
	
}
