// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FilterOptionWidget.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FFilterOptionButtonSignature, int)

class UButton;
/**
 * 
 */
UCLASS()
class PROJECTFD_API UFilterOptionWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	virtual void NativeOnInitialized() override;

	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnSelectButtonClicked();

	void SetSizeBoxSize(float Width = 0, float Height = 0);
	void SetOptionNameText(FText NameText);

	FFilterOptionButtonSignature FilterOptionButtonDelegate;

	void SetOptionNum(int InNum);
	
protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="FilterOption", meta = (BindWidget, AllowPrivateAccess=true))
	class USizeBox* FilterOptionSizeBox;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="FilterOption", meta = (BindWidget, AllowPrivateAccess=true))
	class UTextBlock* OptionNameText;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="FilterOption", meta = (BindWidget, AllowPrivateAccess=true))
	class UButton* SelectButton;

	int OptionNum = 0;
	
};
