// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DA_FDInput.generated.h"

class UInputAction;
enum class EFDAbilityInputID : uint8;
class UInputMappingContext;
/**
 * 
 */
UCLASS()
class PROJECTFD_API UDA_FDInput : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category="Input")
	UInputMappingContext* BasicInputMappingContext;
	UPROPERTY(EditDefaultsOnly, Category="Input")
	TMap<EFDAbilityInputID, UInputAction*> GameplayAbilityInputActions;

	
	UPROPERTY(EditDefaultsOnly, Category="Input")
	UInputAction* IA_Look;
	UPROPERTY(EditDefaultsOnly, Category="Input")
	UInputAction* IA_Move;
	UPROPERTY(EditDefaultsOnly, Category="Input")
	UInputAction* IA_Sprint;
	UPROPERTY(EditDefaultsOnly, Category="Input")
	UInputAction* IA_Fire;
	UPROPERTY(EditDefaultsOnly, Category="Input")
	UInputAction* IA_Aim;
	UPROPERTY(EditDefaultsOnly, Category="Input")
	UInputAction* IA_WireAction;
	UPROPERTY(EditDefaultsOnly, Category="Input")
	UInputAction* IA_HangOnGrab;
	UPROPERTY(EditDefaultsOnly, Category="Input")
	UInputAction* IA_Rescue;
	UPROPERTY(EditDefaultsOnly, Category="Input")
	UInputAction* IA_Inventory;
};
