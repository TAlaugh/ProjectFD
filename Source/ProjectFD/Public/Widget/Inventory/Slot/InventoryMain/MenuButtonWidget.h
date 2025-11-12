// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MenuButtonWidget.generated.h"

class UTextBlock;
class UButton;
class UImage;
class UOverlay;
class USizeBox;
/**
 * 
 */
UCLASS()
class PROJECTFD_API UMenuButtonWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnMenuButtonClicked();

	UFUNCTION()
	void SetBackgroundImage(UTexture2D* InTexture);


private:

	UPROPERTY()
	USizeBox* SizeBox;

	UPROPERTY()
	UOverlay* Overlay;

	UPROPERTY()
	UImage* BackgroundImage;

	UPROPERTY()
	UButton* MenuButton;

	UPROPERTY()
	UTextBlock* MenuName;
	
};
