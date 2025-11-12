// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WeaponOptionSlotWidget.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class PROJECTFD_API UWeaponOptionSlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	virtual void NativeOnInitialized() override;

	virtual void NativeConstruct() override;

	void SetWeaponOptionSlot(FText OptionName, float OptionValue);


private:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess=true))
	TObjectPtr<UTextBlock> WeaponOptionText;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess=true))
	TObjectPtr<UTextBlock> OptionValueText;
	
	
};
