// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BaseWidget.generated.h"

class UBaseWidgetController;

UCLASS()
class PROJECTFD_API UBaseWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	void SetWidgetController(UBaseWidgetController* InWidgetController);

	UFUNCTION(BlueprintImplementableEvent)
	void BPOnWidgetControllerSet();

	UFUNCTION()
	virtual void OnWidgetControllerSet();

protected:
	UPROPERTY(BlueprintReadOnly, Category="Widget", meta = (AllowPrivateAccess = true))
	TObjectPtr<UBaseWidgetController> WidgetController;
	
	
};
