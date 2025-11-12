// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "InventoryInterface.generated.h"

struct FFDInventoryEntry;
class UInventoryComponent;
// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInventoryInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROJECTFD_API IInventoryInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UInventoryComponent* GetInventoryComponent();

	UFUNCTION()
	virtual void SetInventoryComponent(TArray<FFDInventoryEntry> InEntries) = 0;
	
};
