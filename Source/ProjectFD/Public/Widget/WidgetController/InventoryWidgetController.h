// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widget/WidgetController/BaseWidgetController.h"
#include "InventoryWidgetController.generated.h"

struct FFDInventoryEntry;
class UInventoryComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FInventoryEntrySignature, const FFDInventoryEntry&, Entry, float, FireTime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPlayerModuleInventoryEntrySignature, const FFDInventoryEntry&, Entry);
DECLARE_MULTICAST_DELEGATE_OneParam(FWidgetModuleCapacityChangeSignature, float)
DECLARE_MULTICAST_DELEGATE_OneParam(FWidgetMaxModuleCapacityChangeSignature, float)
/**
 * 
 */
UCLASS()
class PROJECTFD_API UInventoryWidgetController : public UBaseWidgetController
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintAssignable)
	FInventoryEntrySignature InventoryEntryDelegate;
	FPlayerModuleInventoryEntrySignature PlayerModuleInventoryEntryDelegate;
	FWidgetModuleCapacityChangeSignature ModuleCapacityChangeDelegate;
	FWidgetMaxModuleCapacityChangeSignature MaxModuleCapacityChangeDelegate;

	AActor* GetOwningActor();
	void SetOwningActor(AActor* InOwner);

	void BindCallbacksToDependencies();
	// void BroadcastInitialValues();
	void BroadcastWeaponInventoryInitialValues();
	void BroadcastPlayerModuleInventoryInitialValues();

	void InitializeModuleEffectInfo();
	
private:

	UPROPERTY()
	TObjectPtr<AActor> OwningActor;
	
	UPROPERTY()
	TObjectPtr<UInventoryComponent> OwningInventory;
	
	
};
