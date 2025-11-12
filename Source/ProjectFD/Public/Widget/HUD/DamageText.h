// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DamageText.generated.h"

class UImage;
class UTextBlock;
/**
 * 
 */
UCLASS()
class PROJECTFD_API UDamageText : public UUserWidget
{
	GENERATED_BODY()
		
public:
	void SetDamageText(float Damage);

protected:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* DamageText;

	UPROPERTY(meta = (BindWidget))
	UImage* ImmuneText;
};
